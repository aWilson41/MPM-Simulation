#pragma once

#include "Engine\MathHelper.h"

static const GLfloat HARDENING = 5.0f;

class Particle
{
public:
	Particle() { }
	Particle(glm::vec3* pos, GLfloat lambda, GLfloat mu)
	{
		Particle::pos = pos;
		Particle::lambda = lambda;
		Particle::mu = mu;
		defElastic = glm::mat2x2(1.0f);
		defPlastic = glm::mat2x2(1.0f);
		svd_w = glm::mat2x2(1.0f);
		svd_v = glm::mat2x2(1.0f);
		svd_e = glm::vec2(1.0f, 1.0f);
	}

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

	glm::mat2x2 getForce();
	//Update position, based on velocity
	void updatePos(GLfloat dt);
	//Update deformation gradient
	void updateGradient(GLfloat dt);
	void applyPlasticity();

public:
	bool isFixed = false;
	// Pointer to external position (the particle doesn't hold the position, the dataset does)
	glm::vec3* pos = nullptr;
	glm::vec2 gridPos = glm::vec2(0.0f);
	GLfloat mass = 1.0f;
	GLfloat density = 0.0f;
	GLfloat volume = 0.0f;
	glm::vec3 velocity = glm::vec3(0.0f);
	glm::mat2x2 velocityGradient = glm::mat2x2(1.0f);

	//Lame parameters (_s denotes starting configuration)
	GLfloat lambda = 0.0f;
	GLfloat mu = 0.0f;
	//Deformation gradient (elastic and plastic parts)
	glm::mat2x2 defElastic;
	glm::mat2x2 defPlastic;
	//Cached SVD's for elastic deformation gradient
	glm::mat2x2 svd_w;
	glm::mat2x2 svd_v;
	glm::vec2 svd_e;
};