#include "MPMGrid.h"
#include "Constants.h"
#include "Particle.h"
#include <iostream>

// If this is on we will collect max values to track instabilities
//#define STATS

static GLfloat NX(GLfloat x)
{
	x = fabs(x);
	//GLfloat w;
	if (x < 1.0f)
		return x * x*(x * 0.5f - 1.0f) + 2.0f / 3.0f;
	else if (x < 2.0f)
		return x * (x * (-x / 6.0f + 1.0f) - 2.0f) + 4.0f / 3.0f;
	else
		return 0.0f;
}
static GLfloat dNX(const GLfloat x)
{
	GLfloat absx = fabs(x);
	if (absx < 1.0f)
		return 1.5f * absx * x - 2.0f * x;
	else if (absx < 2.0f)
		return -0.5f * absx * x + 2.0f * x - 2.0f * x / absx;
	else
		return 0.0f;
}

static GLfloat weight(const glm::vec2 pos) { return NX(pos.x) * NX(pos.y); }
glm::vec2 MPMGrid::gradWeight(const glm::vec2 pos)
{
	return glm::vec2(dNX(pos.x) * NX(pos.y), dNX(pos.y) * NX(pos.x)) / cellSize;
}

void MPMGrid::initGrid(glm::vec2 origin, glm::vec2 size, int width, int height)
{
	MPMGrid::size = size;
	gridWidth = width;
	gridHeight = height;
	nodeCount = gridWidth * gridHeight;
	nodes = new GridNode[nodeCount];
	cellSize = size / glm::vec2(gridWidth, gridHeight);
	MPMGrid::origin = origin;
}

void MPMGrid::initGrid(GLfloat x1, GLfloat x2, GLfloat y1, GLfloat y2, int width, int height)
{
	size = glm::vec2(x2 - x1, y2 - y1);
	gridWidth = width;
	gridHeight = height;
	nodeCount = gridWidth * gridHeight;
	nodes = new GridNode[nodeCount];
	cellSize = size / glm::vec2(gridWidth, gridHeight);
	origin = glm::vec2(x1, y1);
}

void MPMGrid::initParticles(Particle* particles, UINT count)
{
	MPMGrid::particles = particles;
	pointCount = count;

	initMass();
	// Calculate densities and volumes
	const GLfloat invNodeArea = 1.0f / (cellSize.x * cellSize.y);
	for (UINT i = 0; i < pointCount; i++)
	{
		Particle& p = particles[i];
		p.density = p.volume = 0.0f;
		UINT startX = static_cast<UINT>(MathHelp::clamp(p.gridPosX - 2, 0, gridWidth));
		UINT endX = static_cast<UINT>(MathHelp::clamp(p.gridPosX + 3, 0, gridWidth));
		UINT startY = static_cast<UINT>(MathHelp::clamp(p.gridPosY - 2, 0, gridHeight));
		UINT endY = static_cast<UINT>(MathHelp::clamp(p.gridPosY + 3, 0, gridHeight));
		for (UINT y = startY; y < endY; y++)
		{
			for (UINT x = startX; x < endX; x++)
			{
				UINT index = y * gridWidth + x;
				glm::vec2 gridPos = glm::vec2(x, y) * cellSize + origin;
				glm::vec2 diff = (p.getPos() - gridPos) / cellSize;
				p.density += weight(diff) * nodes[index].mass;
				//nodes[index].active = true; // Node active if it has mass
			}
		}
		p.density *= invNodeArea;
		p.volume = p.mass / p.density;
	}
}


void MPMGrid::initMass()
{
	//memset(nodes, 0, sizeof(GridNode) * nodeCount);
	for (int i = 0; i < nodeCount; i++)
	{
		nodes[i] = GridNode();
	}

	// Weight the mass of the particles into the nodes
	for (UINT i = 0; i < pointCount; i++)
	{
		Particle& p = particles[i];
		glm::vec2 pPos = p.getPos();
		p.gridPosX = static_cast<int>((pPos.x - origin.x) / cellSize.x - 0.5f);
		p.gridPosY = static_cast<int>((pPos.y - origin.y) / cellSize.y - 0.5f);
		UINT startX = static_cast<UINT>(MathHelp::clamp(p.gridPosX - 2, 0, gridWidth));
		UINT endX = static_cast<UINT>(MathHelp::clamp(p.gridPosX + 3, 0, gridWidth));
		UINT startY = static_cast<UINT>(MathHelp::clamp(p.gridPosY - 2, 0, gridHeight));
		UINT endY = static_cast<UINT>(MathHelp::clamp(p.gridPosY + 3, 0, gridHeight));
		for (UINT y = startY; y < endY; y++)
		{
			for (UINT x = startX; x < endX; x++)
			{
				// Physical position/center of grid node
				glm::vec2 gridPos = glm::vec2(x, y) * cellSize + origin;
				// Diff between particle's and grid nodes physical position, converted back to grid coordinates
				glm::vec2 diff = (pPos - gridPos) / cellSize;
				nodes[y * gridWidth + x].mass += p.mass * weight(diff);
			}
		}
	}
}

void MPMGrid::initVelocities()
{
	// Weight the velocities of the particles into the nodes
	for (UINT i = 0; i < pointCount; i++)
	{
		Particle& p = particles[i];
		UINT startX = static_cast<UINT>(MathHelp::clamp(p.gridPosX - 2, 0, gridWidth));
		UINT endX = static_cast<UINT>(MathHelp::clamp(p.gridPosX + 3, 0, gridWidth));
		UINT startY = static_cast<UINT>(MathHelp::clamp(p.gridPosY - 2, 0, gridHeight));
		UINT endY = static_cast<UINT>(MathHelp::clamp(p.gridPosY + 3, 0, gridHeight));
		for (UINT y = startY; y < endY; y++)
		{
			for (UINT x = startX; x < endX; x++)
			{
				UINT index = y * gridWidth + x;
				// Physical position/center of grid node
				glm::vec2 gridPos = glm::vec2(x, y) * cellSize + origin;
				// Diff between particle's and grid nodes physical position, converted back to grid coordinates
				glm::vec2 diff = (p.getPos() - gridPos) / cellSize;
				nodes[index].velocity += p.velocity * weight(diff);
			}
		}
	}
	// This step is done when updating the velocities of the grid
	/*for (int i = 0; i < nodeCount; i++)
	{
		if (nodes[i].active)
			nodes[i].velocity /= nodes[i].mass;
	}*/
}

void MPMGrid::updateGridVelocities(GLfloat dt)
{
#ifdef STATS
	maxParticleEnergyDerivativeDet = 0.0f;
	maxParticleEnergyDerivative = glm::mat2(0.0f);
#endif
	for (UINT i = 0; i < pointCount; i++)
	{
		Particle& p = particles[i];
		// Initial volume * cauchy
		glm::mat2 energyDerivative = -p.volume * p.calcCauchyStress();
#ifdef STATS
		GLfloat det = glm::determinant(energyDerivative);
		if (det > maxParticleEnergyDerivativeDet)
		{
			maxParticleEnergyDerivativeDet = det;
			maxParticleEnergyDerivative = energyDerivative;
		}
#endif
		UINT startX = static_cast<UINT>(MathHelp::clamp(p.gridPosX - 2, 0, gridWidth));
		UINT endX = static_cast<UINT>(MathHelp::clamp(p.gridPosX + 3, 0, gridWidth));
		UINT startY = static_cast<UINT>(MathHelp::clamp(p.gridPosY - 2, 0, gridHeight));
		UINT endY = static_cast<UINT>(MathHelp::clamp(p.gridPosY + 3, 0, gridHeight));
		for (UINT y = startY; y < endY; y++)
		{
			for (UINT x = startX; x < endX; x++)
			{
				UINT index = y * gridWidth + x;
				// Physical position/center of grid node
				glm::vec2 gridPos = glm::vec2(x, y) * cellSize + origin;
				// Diff between particle's and grid nodes physical position, converted back to grid coordinates
				glm::vec2 diff = (p.getPos() - gridPos) / cellSize;
				nodes[index].force += energyDerivative * gradWeight(diff);
			}
		}
	}

	maxNodeF = 0.0f;
	maxNodeVelocity = 0.0f;
	glm::vec2 fG = glm::vec2(0.0f, -0.8f);
	for (int i = 0; i < nodeCount; i++)
	{
		// Only update the node velocity if it is active (and non zero mass)
		if (nodes[i].mass != 0.0f)
		{
			GLfloat invMass = 1.0f / nodes[i].mass;
			// Previously we took the sum of velocities from all the particles so divide by total mass
			nodes[i].velocity *= invMass;
			nodes[i].newVelocity = nodes[i].velocity + (nodes[i].force * invMass + fG) * dt;

#ifdef STATS
			GLfloat nodeV = glm::length(nodes[i].newVelocity);
			if (nodeV > maxNodeVelocity)
				maxNodeVelocity = nodeV;
			GLfloat nodeF = glm::length(nodes[i].force);
			if (nodeF > maxNodeF)
				maxNodeF = nodeF;
#endif
		}
		else
			nodes[i].velocity = nodes[i].newVelocity = glm::vec2(0.0f);
	}
}

void MPMGrid::updateParticleVelocities()
{
#ifdef STATS
	maxParticleVGDet = 0.0f;
	maxParticleVG = glm::mat2(0.0f);
	maxParticleVelocity = 0.0f;
#endif
	const GLfloat invNodeArea = 1.0f / (cellSize.x * cellSize.y);
	// We calculate both the pic and flip velocity and interpolate between the two
	// We also calculate a velocity gradient to integrate the deformation gradient with
	for (UINT i = 0; i < pointCount; i++)
	{
		Particle& p = particles[i];
		p.vG = glm::mat2(0.0f);
		p.density = 0.0f;
		glm::vec2 picVelocity = glm::vec2(0.0f);
		glm::vec2 flipVelocity = p.velocity;
		UINT startX = static_cast<UINT>(MathHelp::clamp(p.gridPosX - 2, 0, gridWidth));
		UINT endX = static_cast<UINT>(MathHelp::clamp(p.gridPosX + 3, 0, gridWidth));
		UINT startY = static_cast<UINT>(MathHelp::clamp(p.gridPosY - 2, 0, gridHeight));
		UINT endY = static_cast<UINT>(MathHelp::clamp(p.gridPosY + 3, 0, gridHeight));
		for (UINT y = startY; y < endY; y++)
		{
			for (UINT x = startX; x < endX; x++)
			{
				const GridNode node = nodes[y * gridWidth + x];
				// Physical position/center of grid node
				glm::vec2 gridPos = glm::vec2(x, y) * cellSize + origin;
				// Diff between particle's and grid nodes physical position, converted back to grid coordinates
				glm::vec2 diff = (p.getPos() - gridPos) / cellSize;
				GLfloat w = weight(diff);
				glm::vec2 wG = gradWeight(diff);

				picVelocity += node.newVelocity * w;
				flipVelocity += (node.newVelocity - node.velocity) * w;

				p.vG += MathHelp::outer(node.newVelocity, wG);

				p.density += node.mass * w;
			}
		}
		// A mix between flip and pic velocity (node velocity already multiplied with dt)
		p.velocity = flipVelocity * FLIP_PERCENT + picVelocity * (1.0f - FLIP_PERCENT);

#ifdef STATS
		GLfloat det = glm::determinant(p.vG);
		if (det > maxParticleVGDet)
		{
			maxParticleVGDet = det;
			maxParticleVG = p.vG;
		}
#endif

		GLfloat v = glm::length(p.velocity);
		if (v > maxParticleVelocity)
			maxParticleVelocity = v;
		p.density *= invNodeArea;
	}
}

void MPMGrid::projectToGrid()
{
	initMass();
	initVelocities();
}

void MPMGrid::update(GLfloat dt)
{
#ifdef STATS
	maxParticleDef = glm::mat2(0.0f);
	maxParticleDefDet = 0.0f;
#endif

	updateGridVelocities(dt);
	updateParticleVelocities();
	// Finally the particles positions are updated
	for (UINT i = 0; i < pointCount; i++)
	{
		particles[i].updatePos(dt);
		particles[i].updateGradient(dt);
		particles[i].applyPlasticity();

#ifdef STATS
		GLfloat defGDet = glm::determinant(particles[i].defG);
		if (defGDet > maxParticleDefDet)
		{
			maxParticleDefDet = defGDet;
			maxParticleDef = particles[i].defG;
		}
#endif
	}
}


MPMGrid::~MPMGrid()
{
	if (nodes != nullptr)
		delete[] nodes;
}