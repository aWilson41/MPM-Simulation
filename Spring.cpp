//#include "Spring.h"
//#include "Particle.h"
//
//void Spring::applySpringForce()
//{
//	// Calculate distance between the two particles
//	glm::vec3 dist = p2->getPos() - p1->getPos();
//	float length = glm::length(dist);
//	// Normalize the distance for direciton
//	glm::vec3 nDist = glm::normalize(dist);
//
//	// Get the velocity along the direction
//	float v1 = glm::dot(p1->velocity, nDist);
//	float v2 = glm::dot(p2->velocity, nDist);
//
//	// Calculate the force of the spring
//	float fSD = -ks * (restingLength - length) - kd * (v1 - v2);
//	glm::vec3 force = fSD * nDist;
//
//	p1->applyForce(force);
//	p2->applyForce(-force);
//}