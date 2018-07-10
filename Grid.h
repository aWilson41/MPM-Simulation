#pragma once

#include "Engine/MathHelper.h"
#include "Engine/Geometry.h"

class Particle;

struct GridNode
{
	GLfloat mass;
	glm::vec2 velocity;

	GridNode()
	{
		mass = 0.0f;
		velocity = glm::vec2(0.0f);
	}
};

class Grid
{
public:
	UINT nodeCount = 0;
	GridNode* nodes = nullptr;

	geom::Rect bounds;
	UINT gridWidth = 0;
	UINT gridHeight = 0;
	glm::vec2 cellSize = glm::vec2(1.0f);

public:
	// Physical position, size, and dimensions of the grid
	Grid(glm::vec2 pos, glm::vec2 size, glm::vec2 dim);

	// Map the mass to the grid
	void initMass(Particle* particles, UINT particleCount);
	// Map the velocities to the grid
	void initVelocities(Particle* particles, UINT particleCount);

private:
	void plotParticleMass(Particle* particle);
	void plotParticleVelocity(Particle* particle);
};