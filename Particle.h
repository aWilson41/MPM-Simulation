#pragma once

#include "Engine\MathHelper.h"

class Particle
{
public:
	Particle() { }

	Particle(glm::vec3* pos)
	{
		Particle::pos = pos;
	}

public:
	void integrate(float dt)
	{
		if (!isFixed)
		{
			integrateAccel(dt);
			integrateVelocity(dt);
		}
	}

	void resetForce()
	{
		force = glm::vec3(0.0f);
	}

	void integrateAccel(float dt)
	{
		velocity += force * dt;
	}

	void integrateVelocity(float dt)
	{
		*pos += velocity * dt;
	}

	void applyForce(glm::vec3 force)
	{
		Particle::force += force;
	}

	glm::vec3 getPos() { return *pos; }

public:
	bool isFixed = false;
	// Pointer to external position (the particle doesn't hold the position, the dataset does)
	glm::vec3* pos = nullptr;
	glm::vec3 normal = glm::vec3(0.0f);
	glm::vec3 force = glm::vec3(0.0f);
	glm::vec3 velocity = glm::vec3(0.0f);
};