#pragma once
#include "Constants.h"
#include "Engine/MathHelper.h"

class Particle
{
public:
	Particle() { }
	Particle(glm::vec3* pos, GLfloat mass)
	{
		Particle::pos = pos;
		Particle::mass = mass;
	}

	void updatePos(GLfloat dt) { *pos += glm::vec3(velocity, 0.0f) * dt; }// *pos + velocity * dt; }
	void updateGradient(GLfloat dt) { defGe = (I + vG * dt) * defGe; }
	// Energy derivative
	const glm::mat2 calcStVenantPK2Stress()
	{
		glm::mat2 greenStrain = (glm::transpose(defGe) * defGe - I) * 0.5f;
		return bulk * MathHelp::trace(greenStrain) * I + 2.0f * shear * greenStrain;
	}
	const glm::mat2 calcModifiedStVenantPK2Stress()
	{
		GLfloat detF = glm::determinant(defGe);
		glm::mat2 defGNorm = defGe * (1.0f / std::sqrt(detF));
		glm::mat2 isochoricGreenStrain = (glm::transpose(defGNorm) * defGNorm - I) * 0.5f;
		return bulk * (detF - 1.0f) * I + 2.0f * shear * isochoricGreenStrain;
	}
	const glm::mat2 calcLinearPK2Stress()
	{
		// Green strain reduces to this for small strains
		glm::mat2 strain = (glm::transpose(defGe) + defGe) * 0.5f - I;
		return 2.0f * bulk * strain + shear * MathHelp::trace(strain) * I;
	}

	const glm::mat2 calcCauchyStress()
	{
		GLfloat harden = std::exp(HARDENING * (1 - glm::determinant(defGp)));
		GLfloat Je = s[0] * s[1];

		glm::mat2 tmp = 2.0f * shear * (defGp - svdU * glm::transpose(svdV)) * glm::transpose(defGe);
		tmp = MathHelp::diagSum(tmp, bulk * Je * (Je - 1.0f));
		return harden * tmp;
	}

	// Take the elastic deformation that exceeds the critical stretch and compression ratio
	// and put it into the plastic deformation
	void applyPlasticity()
	{
		// Compute the SVD of the deformation gradient
		MathHelp::svd(defG, &svdU, &s, &svdV);
		// Clamp to remove the excess compression and stretch
		s[0] = MathHelp::clamp(s[0], CRIT_COMPRESS, CRIT_STRETCH);
		s[1] = MathHelp::clamp(s[1], CRIT_COMPRESS, CRIT_STRETCH);

		glm::mat2x2 ucpy(svdU), vcpy(svdV);
		
		// Divide diagonal by clamped singular values (so if has a value over max (excess) like 2.4 it would produce a value > 1 ie: 2.4 / 2 = 1.2)
		vcpy = MathHelp::diagInvProduct(vcpy, s);
		// Multiple diagonal by clamped singular values (conversly if it has an excess like 2.4 it would produce value < 1  ie: 2 / 2.4 = 0.84)
		ucpy = MathHelp::diagProduct(ucpy, s);
		// So we can say diagonal values are removed from ucpy and put in vcpy when we exceed the stretch ratio
		defGp = vcpy * glm::transpose(svdU) * defG;
		defGe = ucpy * glm::transpose(svdV);
	}

	glm::vec2 getPos() { return glm::vec2(pos->x, pos->y); }

public:
	GLfloat volume = 0.0f;
	GLfloat mass = 0.0f;
	GLfloat density = 0.0f;
	GLfloat bulk = 400.0f;
	GLfloat shear = 600.0f;
	glm::vec3* pos = nullptr;// glm::vec2(0.0f);
	glm::vec2 velocity = glm::vec2(0.0f);

	glm::mat2 defG = I; // Deformation gradient
	glm::mat2 defGe = I; // Elastic deformation gradient
	glm::mat2 defGp = I; // Plastic deformation gradient

	glm::mat2 svdU = I;
	glm::vec2 s = glm::vec2(1.0f, 1.0f);
	glm::mat2 svdV = I;

	glm::mat2 vG = glm::mat2(0.0f); // Velocity gradient

	int gridPosX;
	int gridPosY;
};