#include "Particle.h"

static const GLfloat CRIT_COMPRESS = 1.0f - 1.9e-2f;
static const GLfloat CRIT_STRETCH = 1.0f + 7.5e-3f;

glm::mat2x2 Particle::getForce()
{
	// Adjust lame parameters to account for hardening
	float harden = exp(HARDENING * (1.0f - glm::determinant(defPlastic)));
	float Je = svd_e.x * svd_e.y;
	// This is the co-rotational term
	glm::mat2x2 temp = 2.0f * mu * (defElastic - svd_w * glm::transpose(svd_v)) * glm::transpose(defElastic);
	// Add in the primary contour term
	MathHelp::diagSum(lambda * Je * (Je - 1.0f), &temp);
	// Add hardening and volume
	return volume * harden * temp;
}

void Particle::updatePos(GLfloat dt)
{
	*pos += dt * velocity;
}

void Particle::updateGradient(GLfloat dt)
{
	// So, initially we make all updates elastic
	velocityGradient *= dt;
	MathHelp::diagSum(1.0f, &velocityGradient);
	defElastic = velocityGradient * defElastic;
}

void Particle::applyPlasticity()
{
	glm::mat2x2 fAll = defElastic * defPlastic;
	// We compute the SVD decomposition
	// The singular values (basically a scale transform) tell us if 
	// the particle has exceeded critical stretch/compression
	MathHelp::svd(defElastic, &svd_w, &svd_e, &svd_v);

	glm::mat2x2 svdVtrans = glm::transpose(svd_v);
	// Clamp singular values to within elastic region
	for (UINT i = 0; i < 2; i++)
	{
		if (svd_e[i] < CRIT_COMPRESS)
			svd_e[i] = CRIT_COMPRESS;
		else if (svd_e[i] > CRIT_STRETCH)
			svd_e[i] = CRIT_STRETCH;
	}
	// Recompute elastic and plastic gradient
	// We're basically just putting the SVD back together again
	glm::mat2x2 v_cpy(svd_v);
	glm::mat2x2 w_cpy(svd_w);
	MathHelp::diagProductInv(svd_e, &v_cpy);
	MathHelp::diagProduct(svd_e, &w_cpy);
	defPlastic = v_cpy * glm::transpose(svd_w) * fAll;
	defElastic = w_cpy * glm::transpose(svd_v);
}