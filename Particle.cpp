#include "Particle.h"

void Particle::updatePos(GLfloat dt)
{
	*pos += glm::vec3(velocity, 0.0f) * dt;
}
void Particle::updateGradient(GLfloat dt)
{
	// First we take the movement and put it into the elastic deformation
	defGe = (I2 + vG * dt) * defGe;

	// Calculate the combined deformation
	defG = defGe * defGp;

	// Anything over stretch ratio or compression ratio is dumped into plastic deformation
	// To do this compute the SVD of the elastic deformation gradient defGe = U * s * v^T
	MathHelp::svd(defGe, &svdU, &s, &svdV);

	// Clamp to remove the excess compression and stretch
	s[0] = MathHelp::clamp(s[0], CRIT_COMPRESS, CRIT_STRETCH);
	s[1] = MathHelp::clamp(s[1], CRIT_COMPRESS, CRIT_STRETCH);

	// For elastic just put back together Fe = u * s * v^T after the s clamp
	defGe = MathHelp::diagProduct(svdU, s) * glm::transpose(svdV);

	// For plastic since F = Fe * Fp then Fp = [(Fe)^-1 * F] = [(u * s * v^T)^-1 * F] = (v * s^-1 * u^T) * F
	defGp = MathHelp::diagProduct(svdV, glm::vec2(1.0f / s.x, 1.0f / s.y)) * glm::transpose(svdU) * defG;
}
// Energy derivative
glm::mat2 Particle::calcCauchyStress()
{
	// Ratios of volumetric change
	GLfloat Jp = glm::determinant(defGp);
	GLfloat Je = glm::determinant(defGe);

	// Plastic deformation contributes exponentially to hardening term
	GLfloat hardening = std::exp(HARDENING * (1.0f - Jp));	
	GLfloat lambda = bulk * hardening;
	GLfloat mu = shear * hardening;

	// Shearing term on left (achieved through polar decomposition to remove rigid rotational from elastic deformation), compressional/bulk on the right
	glm::mat2 defGe_r;
	MathHelp::pd(defGe, &defGe_r);
	return 2.0f * mu * (defGe - defGe_r) * glm::transpose(defGe) + glm::mat2(lambda * (Je - 1.0f) * Je);
}