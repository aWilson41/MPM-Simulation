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

	void updatePos(GLfloat dt);
	void updateGradient(GLfloat dt);

	glm::mat2 calcCauchyStress();

	glm::vec2 getPos() { return glm::vec2(pos->x, pos->y); }
	//GLfloat* getPos() { return &pos->x; }

public:
	GLfloat volume = 0.0f;
	GLfloat mass = 0.0f;
	GLfloat density = 0.0f;
	GLfloat bulk = 400.0f;
	GLfloat shear = 600.0f;
	glm::vec3* pos = nullptr;// glm::vec2(0.0f);
	glm::vec2 velocity = glm::vec2(0.0f);

	glm::mat2 defG = I2; // Deformation gradient
	glm::mat2 defGe = I2; // Elastic deformation gradient
	glm::mat2 defGp = I2; // Plastic deformation gradient

	glm::mat2 svdU = I2;
	glm::vec2 s = glm::vec2(1.0f, 1.0f);
	glm::mat2 svdV = I2;

	glm::mat2 vG = glm::mat2(0.0f); // Velocity gradient

	// Grid bounds
	UINT startX = -1;
	UINT endX = -1;
	UINT startY = -1;
	UINT endY = -1;
	bool inBounds = true;
};