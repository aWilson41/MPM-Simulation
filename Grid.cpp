#include "Grid.h"
#include "Particle.h"

static const GLfloat FLIP_PERCENT = 0.95f;

Grid::Grid(glm::vec2 pos, glm::vec2 size, glm::vec2 dim)
{
	bounds = geom::Rect(pos, size);
	Grid::cellSize = size / dim;
	cellArea = cellSize.x * cellSize.y;
	gridWidth = static_cast<UINT>(dim.x);
	gridHeight = static_cast<UINT>(dim.y);
	nodeCount = gridWidth * gridHeight;
	nodes = new GridNode[nodeCount];
}


void Grid::calcParticleVolume(Particle* particles, UINT particleCount)
{
	for (UINT i = 0; i < particleCount; i++)
	{
		Particle* p = &particles[i];
		unplotParticleVolume(p);
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

void Grid::computeVelocities(Particle* particles, UINT particleCount, glm::vec2 explicitForce, GLfloat dt)
{
	// First find the velocity gradient for every particle
	for (UINT i = 0; i < particleCount; i++)
	{
		Particle* p = &particles[i];
		calcParticleVelocityGradient(p);
	}

	//Now we have all grid forces, compute velocities (euler integration)
	for (UINT i = 0; i < nodeCount; i++)
	{
		GridNode& node = nodes[i];
		if (node.active)
			node.newVelocity = node.velocity + dt * (explicitForce - node.newVelocity / node.mass);
	}
}

void Grid::updateVelocities(Particle* particles, UINT particleCount)
{
	for (UINT i = 0; i < particleCount; i++)
	{
		Particle* p = &particles[i];
		// We calculate PIC and FLIP velocities separately
		glm::vec2 pic;
		glm::vec2 flip = p->velocity;
		// Also keep track of velocity gradient
		glm::mat2x2& vGrad = p->velocityGradient;
		vGrad[0][0] = vGrad[0][1] = vGrad[1][0] = vGrad[1][1] = 0.0f;
		// VISUALIZATION PURPOSES ONLY:
		// Recompute density
		p->density = 0.0f;
		UINT ox = p->gridPos.x;
		UINT oy = p->gridPos.y;
		for (int i = -1; i < 2; i++)
		{
			int x = static_cast<UINT>(p->gridPos.x) + i;
			GLfloat wy = MathHelp::bspline(i);
			GLfloat dy = MathHelp::bsplineSlope(i);
			for (int j = -1; j < 2; j++)
			{
				int y = static_cast<UINT>(p->gridPos.y) + j;
				GLfloat wx = MathHelp::bspline(j);
				GLfloat dx = MathHelp::bsplineSlope(j);
				
				GLfloat weight = wx * wy;
				if (weight > 1e-4)
				{
					glm::vec2 gradientWeight = glm::vec2(dx * wy, wx * dy) / cellSize;
					UINT index = y * gridWidth + x;
					GridNode* node = &nodes[index];
					// Particle in cell
					pic += node->newVelocity * weight;
					// Fluid implicit particle
					flip += (node->newVelocity - node->velocity) * weight;
					// Velocity gradient
					vGrad += glm::outerProduct(node->newVelocity, gradientWeight);
					// VISUALIZATION ONLY: Update density
					p->density += weight * node->mass;
				}
			}
		}

		// Final velocity is a linear combination of PIC and FLIP components
		glm::vec2 v = flip * FLIP_PERCENT + pic * (1.0f - FLIP_PERCENT);
		p->velocity = glm::vec3(v.x, v.y, 0.0f);
		// VISUALIZATION: Update density
		p->density /= cellArea;
	}
}


void Grid::unplotParticleVolume(Particle* p)
{
	p->density = 0.0f;
	for (int i = -1; i < 2; i++)
	{
		int x = static_cast<UINT>(p->gridPos.x) + i;
		GLfloat wy = MathHelp::bspline(i);
		for (int j = -1; j < 2; j++)
		{
			int y = static_cast<UINT>(p->gridPos.y) + j;
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
	for (int i = -1; i < 2; i++)
	{
		int x = static_cast<UINT>(p->gridPos.x) + i;
		GLfloat wy = MathHelp::bspline(i);
		GLfloat dy = MathHelp::bsplineSlope(i);
		for (int j = -1; j < 2; j++)
		{
			int y = static_cast<UINT>(p->gridPos.y) + j;
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
	for (int i = -1; i < 2; i++)
	{
		int x = static_cast<UINT>(p->gridPos.x) + i;
		GLfloat wy = MathHelp::bspline(i);
		//GLfloat dy = MathHelp::bsplineSlope(i);
		for (int j = -1; j < 2; j++)
		{
			int y = static_cast<UINT>(p->gridPos.y) + j;
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
	glm::mat2x2 energy = p->getForce();
	for (int i = -1; i < 2; i++)
	{
		int x = static_cast<UINT>(p->gridPos.x) + i;
		GLfloat wy = MathHelp::bspline(i);
		GLfloat dy = MathHelp::bsplineSlope(i);
		for (int j = -1; j < 2; j++)
		{
			int y = static_cast<UINT>(p->gridPos.y) + j;
			GLfloat wx = MathHelp::bspline(j);
			GLfloat dx = MathHelp::bsplineSlope(j);
			glm::vec2 gradientWeight = glm::vec2(dx * wy, wx * dy) / cellSize;

			if (wx * wy > 1e-4)
				nodes[y * gridWidth + x].newVelocity += energy * gradientWeight;
		}
	}
}