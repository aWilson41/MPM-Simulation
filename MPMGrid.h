#pragma once
#include "Engine/MathHelper.h"

class Particle;

class GridNode
{
public:
	GridNode() { }
	GridNode(glm::vec2 pos) { GridNode::pos = pos; }

public:
	GLfloat mass = 0.0f;
	bool active = false;
	//bool active = false;
	glm::vec2 pos = glm::vec2(0.0f);
	glm::vec2 velocity = glm::vec2(0.0f);
	glm::vec2 newVelocity = glm::vec2(0.0f);
	glm::vec2 force = glm::vec2(0.0f);
};

class MPMGrid
{
public:
	~MPMGrid();

	// Physical bounds and grid size
	void initGrid(glm::vec2 origin, glm::vec2 size, int width, int height);
	// Sets the particles by intializing the mass of the grid and updating particle volumes/densities
	void initParticles(Particle* particles, UINT count);

	// Update loop
	// Calculates the mass of the grid nodes given the particles
	void initMass();
	// Calculates the velocities of the grid nodes given the particles
	void initVelocities();
	// Updates grid node velocities
	void updateGridVelocities(GLfloat dt);
	// Particles velocities are updated according to the grid velocities
	void updateParticleVelocities();

	// 3d pos used for 
	void collision(GLfloat* pos, glm::vec2& v, GLfloat dt);

	void projectToGrid();
	void update(GLfloat dt);

protected:
	glm::vec2 gradWeight(const glm::vec2 pos);

public:
	GridNode* nodes = nullptr;

	Particle* particles = nullptr;
	UINT pointCount = 0;

	glm::vec2 origin;
	glm::vec2 size;
	int gridWidth;
	int gridHeight;
	int nodeCount;
	glm::vec2 cellSize;
	glm::vec2 invCellSize;

	GLfloat maxParticleVelocity = 0.0f;
	GLfloat maxParticleDefDet = 0.0f;
	glm::mat2 maxParticleDef = glm::mat2(0.0f);
	GLfloat maxParticleVGDet = 0.0f;
	glm::mat2 maxParticleVG = glm::mat2(0.0f);
	GLfloat maxNodeF = 0.0f;
	GLfloat maxParticleEnergyDerivativeDet = 0.0f;
	glm::mat2 maxParticleEnergyDerivative = glm::mat2(0.0f);
	GLfloat maxNodeVelocity = 0.0f;
};