#include "Grid.h"
#include "Particle.h"

Grid::Grid(glm::vec2 pos, glm::vec2 size, glm::vec2 dim)
{
	bounds = geom::Rect(pos, size);
	Grid::cellSize = size / dim;
	gridWidth = static_cast<UINT>(dim.x);
	gridHeight = static_cast<UINT>(dim.y);
}

// Map the mass to the grid
void Grid::initMass(Particle* particles, UINT particleCount)
{
	// Reset the grid
	memset(nodes, 0, sizeof(GridNode) * nodeCount);

	// We calculate the mass at every node using our basis function
	for (UINT i = 0; i < particleCount; i++)
	{
		Particle* p = &particles[i];
		glm::vec2 pos = glm::vec2(p->getPos().x, p->getPos().y);
		// Transform point to grid
		p->gridPos = (pos - bounds.topLeft()) / cellSize;

		plotParticleMass(p);
	}
}

// Map the velocities to the grid
void Grid::initVelocities(Particle* particles, UINT particleCount)
{
	// We calculate the velocity at every node using our basis function
	for (UINT i = 0; i < particleCount; i++)
	{
		Particle* p = &particles[i];
		plotParticleVelocity(p);
	}
}

void Grid::plotParticleMass(Particle* p)
{
	// The plotting takes place over a 2x2 square for each particle
	for (UINT i = 0; i < 3; i++)
	{
		UINT x = static_cast<UINT>(p->gridPos.x) + i;
		GLfloat wy = MathHelp::bspline(i);
		GLfloat dy = MathHelp::bsplineSlope(i);
		for (UINT j = 0; j < 3; j++)
		{
			UINT y = static_cast<UINT>(p->gridPos.y) + j;
			GLfloat wx = MathHelp::bspline(j);
			GLfloat dx = MathHelp::bsplineSlope(j);

			GLfloat weight = wx * wy;

			nodes[y * gridWidth + x].mass += weight * p->mass;
		}
	}
}

void Grid::plotParticleVelocity(Particle* p)
{
	// The plotting takes place over a 2x2 square for each particle
	for (UINT i = 0; i < 3; i++)
	{
		UINT x = static_cast<UINT>(p->gridPos.x) + i;
		GLfloat wy = MathHelp::bspline(i);
		GLfloat dy = MathHelp::bsplineSlope(i);
		for (UINT j = 0; j < 3; j++)
		{
			UINT y = static_cast<UINT>(p->gridPos.y) + j;
			GLfloat wx = MathHelp::bspline(j);
			GLfloat dx = MathHelp::bsplineSlope(j);

			GLfloat weight = wx * wy;

			nodes[y * gridWidth + x].mass += weight * p->mass;
			nodes[y * gridWidth + x].velocity += glm::vec2(p->velocity.x, p->velocity.y) * weight * p->mass;
		}
	}
}