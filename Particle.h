#pragma once

#include "Engine\MathHelper.h"

class Particle
{
public:
	Particle() { }

	Particle(glm::vec3* pos) { Particle::pos = pos; }

public:
	/*void integrate(float dt)
	{
		if (!isFixed)
		{
			integrateAccel(dt);
			integrateVelocity(dt);
		}
	}

	void resetForce() { force = glm::vec3(0.0f); }

	void integrateAccel(float dt) { velocity += force * dt; }

	void integrateVelocity(float dt) { *pos += velocity * dt; }

	void applyForce(glm::vec3 force) { Particle::force += force; }*/

	glm::vec3 getPos() { return *pos; }

public:
	bool isFixed = false;
	// Pointer to external position (the particle doesn't hold the position, the dataset does)
	glm::vec3* pos = nullptr;
	glm::vec2 gridPos = glm::vec2(0.0f);
	GLfloat mass = 0.0f;
	GLfloat density = 0.0f;
	GLfloat volume = 0.0f;
	glm::vec3 velocity = glm::vec3(0.0f);
	glm::vec3 velocityGradient = glm::vec3(0.0f);
};