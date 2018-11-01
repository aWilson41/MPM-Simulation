#pragma once
#include "Constants.h"
#include "Engine/MathHelper.h"

class Particle
{
public:
	Particle() { };
	Particle(glm::vec3* pos, GLfloat mass)
	{
		Particle::pos = pos;
		Particle::mass = mass;
	}

	void updatePos(GLfloat dt) { *pos += glm::vec3(velocity, 0.0f) * dt; }// *pos + velocity * dt; }
	void updateGradient(GLfloat dt) { defG = (I + vG * dt) * defG; }
	// Energy derivative
	const glm::mat2 calcStVenantPK2Stress()
	{
		glm::mat2 greenStrain = (glm::transpose(defG) * defG - I) * 0.5f;
		return bulk * MathHelp::trace(greenStrain) * I + 2.0f * shear * greenStrain;
	}
	const glm::mat2 calcModifiedStVenantPK2Stress()
	{
		GLfloat detF = glm::determinant(defG);
		glm::mat2 defGNorm = defG * (1.0f / std::sqrt(detF));
		glm::mat2 isochoricGreenStrain = (glm::transpose(defGNorm) * defGNorm - I) * 0.5f;
		return bulk * (detF - 1.0f) * I + 2.0f * shear * isochoricGreenStrain;
	}
	const glm::mat2 calcLinearPK2Stress()
	{
		// Green strain reduces to this for small strains
		glm::mat2 strain = (glm::transpose(defG) + defG) * 0.5f - I;
		return 2.0f * bulk * strain + shear * MathHelp::trace(strain) * I;
	}

	// Converts PK2 to cauchy
	const glm::mat2 calcCauchyStress()
	{
		const GLfloat J = glm::determinant(defG);
		return (1.0f / J) * calcStVenantPK2Stress() * glm::transpose(defG);
		//return (1.0f / J) * calcModifiedStVenantPK2Stress() * glm::transpose(defG);
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
	glm::mat2 defG = I; // deformation gradient
	glm::mat2 vG = glm::mat2(0.0f); // Velocity gradient

	int gridPosX;
	int gridPosY;
};