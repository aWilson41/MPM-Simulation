#include "Grid.h"
#include "Particle.h"

Grid::Grid(glm::vec2 pos, glm::vec2 size, glm::vec2 dim)
{
	bounds = geom::Rect(pos, size);
	Grid::cellSize = size / dim;
	cellArea = cellSize.x * cellSize.y;
	gridWidth = static_cast<UINT>(dim.x);
	gridHeight = static_cast<UINT>(dim.y);
}


void Grid::calcParticleVolume(Particle* particles, UINT particleCount)
{
	for (UINT i = 0; i < particleCount; i++)
	{

	}
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
	for (UINT i = 0; i < nodeCount; i++)
	{
		if (nodes[i].active)
			nodes[i].velocity /= nodes[i].mass;
	}
}

void Grid::computeVelocities(Particle* particles, UINT particleCount, glm::vec2 explicitForce)
{
	// First find the velocity gradient for every particle
	for (unsigned int i = 0; i < particleCount; i++)
	{
		Particle* p = &particles[i];
		calcParticleVelocityGradient(p);
	}
}


void Grid::unplotParticleVolume(Particle* p)
{
	for (UINT i = -1; i < 2; i++)
	{
		UINT x = static_cast<UINT>(p->gridPos.x) + i;
		GLfloat wy = MathHelp::bspline(i);
		for (UINT j = -1; j < 2; j++)
		{
			UINT y = static_cast<UINT>(p->gridPos.y) + j;
			GLfloat wx = MathHelp::bspline(j);

			GLfloat weight = wx * wy;
			if (weight > 1e-4)
			{
				UINT index = y * gridWidth + x;
				p->density += weight * nodes[index].mass;
			}
		}
	}
	p->density /= cellArea;
	p->volume = p->mass / p->density;
}

void Grid::plotParticleMass(Particle* p)
{
	// The plotting takes place over a 2x2 square for each particle
	for (UINT i = -1; i < 2; i++)
	{
		UINT x = static_cast<UINT>(p->gridPos.x) + i;
		GLfloat wy = MathHelp::bspline(i);
		GLfloat dy = MathHelp::bsplineSlope(i);
		for (UINT j = -1; j < 2; j++)
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
	for (UINT i = -1; i < 2; i++)
	{
		UINT x = static_cast<UINT>(p->gridPos.x) + i;
		GLfloat wy = MathHelp::bspline(i);
		//GLfloat dy = MathHelp::bsplineSlope(i);
		for (UINT j = -1; j < 2; j++)
		{
			UINT y = static_cast<UINT>(p->gridPos.y) + j;
			GLfloat wx = MathHelp::bspline(j);
			//GLfloat dx = MathHelp::bsplineSlope(j);

			GLfloat weight = wx * wy;

			UINT index = y * gridWidth + x;
			nodes[index].mass += weight * p->mass;
			nodes[index].velocity += glm::vec2(p->velocity.x, p->velocity.y) * weight * p->mass;
			nodes[index].active = true;
		}
	}
}

void Grid::calcParticleVelocityGradient(Particle* p)
{
	for (UINT i = -1; i < 2; i++)
	{
		UINT x = static_cast<UINT>(p->gridPos.x) + i;
		GLfloat wy = MathHelp::bspline(i);
		GLfloat dy = MathHelp::bsplineSlope(i);
		for (UINT j = -1; j < 2; j++)
		{
			UINT y = static_cast<UINT>(p->gridPos.y) + j;
			GLfloat wx = MathHelp::bspline(j);
			GLfloat dx = MathHelp::bsplineSlope(j);

			/*if (wx * wy > 1e-4)
				nodes[y * gridWidth + x].newVelocity*/
		}
	}
}