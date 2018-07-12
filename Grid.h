#pragma once

#include "Engine/MathHelper.h"
#include "Engine/Geometry.h"

class Particle;

struct GridNode
{
	GLfloat mass;
	glm::vec2 velocity;
	glm::vec2 newVelocity;
	bool active;

	GridNode()
	{
		mass = 0.0f;
		velocity = glm::vec2(0.0f);
		newVelocity = glm::vec2(0.0f);
		active = false;
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
	GLfloat cellArea = 1.0f;

public:
	// Physical position, size, and dimensions of the grid
	Grid(glm::vec2 pos, glm::vec2 size, glm::vec2 dim);

	// Computes the volume and density of every particle from the mass in the grid
	void calcParticleVolume(Particle* particles, UINT particleCount);

	// Map the mass to the grid
	void initMass(Particle* particles, UINT particleCount);
	// Map the velocities to the grid
	void initVelocities(Particle* particles, UINT particleCount);

	// Computes the velocities of the grid nodes
	void computeVelocities(Particle* particles, UINT particleCount, glm::vec2 explicitForce);

private:
	void unplotParticleVolume(Particle* particle);
	void plotParticleMass(Particle* particle);
	void plotParticleVelocity(Particle* particle);
	void calcParticleVelocityGradient(Particle* particle);
};