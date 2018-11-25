#include "MPMGrid.h"
#include "Constants.h"
#include "Particle.h"
#include <iostream>

// If this is on we will collect max values to track instabilities
#define STATS

static GLfloat NX(GLfloat x)
{
	x = fabs(x);
	if (x < 1.0f)
		return x * x * (x * 0.5f - 1.0f) + 2.0f / 3.0f;
	else if (x < 2.0f)
		return x * (x * (-x / 6.0f + 1.0f) - 2.0f) + 4.0f / 3.0f;
	else
		return 0.0f;
}
static GLfloat dNX(const GLfloat x)
{
	GLfloat absx = fabs(x);
	if (absx < 1.0f)
		return x * (1.5f * absx - 2.0f);
	else if (absx < 2.0f)
		return x * (-0.5f * absx + 2.0f - 2.0f / absx);
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
	for (int y = 0; y < gridHeight; y++)
	{
		for (int x = 0; x < gridWidth; x++)
		{
			// Physical coordinates of the node
			nodes[y * gridWidth + x] = GridNode(glm::vec2(x, y) * cellSize + origin);
		}
	}

	// Weight the mass of the particles into the nodes
	for (UINT i = 0; i < pointCount; i++)
	{
		Particle& p = particles[i];
		glm::vec2 pPos = p.getPos();
		p.gridPosX = static_cast<int>((pPos.x - origin.x) / cellSize.x);
		p.gridPosY = static_cast<int>((pPos.y - origin.y) / cellSize.y);
		UINT startX = static_cast<UINT>(MathHelp::clamp(p.gridPosX - 2, 0, gridWidth));
		UINT endX = static_cast<UINT>(MathHelp::clamp(p.gridPosX + 3, 0, gridWidth));
		UINT startY = static_cast<UINT>(MathHelp::clamp(p.gridPosY - 2, 0, gridHeight));
		UINT endY = static_cast<UINT>(MathHelp::clamp(p.gridPosY + 3, 0, gridHeight));
		for (UINT y = startY; y < endY; y++)
		{
			for (UINT x = startX; x < endX; x++)
			{
				UINT i = y * gridWidth + x;
				// Diff between particle's and grid nodes physical position, converted back to grid coordinates
				glm::vec2 diff = (pPos - nodes[i].pos) / cellSize;
				nodes[i].mass += p.mass * weight(diff);
				if (nodes[i].mass > 0.0f)
					nodes[i].active = true;
			}
		}
	}
}

// Puts particle momentums onto the grid (as velocity)
void MPMGrid::initVelocities()
{
	// Weight the velocities of the particles into the nodes
	for (UINT i = 0; i < pointCount; i++)
	{
		Particle& p = particles[i];
		glm::vec2 pPos = p.getPos();
		UINT startX = static_cast<UINT>(MathHelp::clamp(p.gridPosX - 2, 0, gridWidth));
		UINT endX = static_cast<UINT>(MathHelp::clamp(p.gridPosX + 3, 0, gridWidth));
		UINT startY = static_cast<UINT>(MathHelp::clamp(p.gridPosY - 2, 0, gridHeight));
		UINT endY = static_cast<UINT>(MathHelp::clamp(p.gridPosY + 3, 0, gridHeight));
		for (UINT y = startY; y < endY; y++)
		{
			for (UINT x = startX; x < endX; x++)
			{
				UINT i = y * gridWidth + x;
				if (nodes[i].active) // No need to calculate for nodes with no mass
				{
					// Diff between particle's and grid nodes physical position, converted back to grid coordinates
					glm::vec2 diff = (pPos - nodes[i].pos) / cellSize;
					nodes[i].velocity += p.velocity * p.mass * weight(diff);
				}
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

// Updates velocities based on stress and gravity
void MPMGrid::updateGridVelocities(GLfloat dt)
{
#ifdef STATS
	maxParticleEnergyDerivativeDet = 0.0f;
	maxParticleEnergyDerivative = glm::mat2(0.0f);
#endif
	for (UINT i = 0; i < pointCount; i++)
	{
		Particle& p = particles[i];
		// Initial volume * cauchy. Energy derivative is force. Change in energy of distance (strain)
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
				UINT i = y * gridWidth + x;
				if (nodes[i].active)
				{
					// Diff between particle's and grid nodes physical position, converted back to grid coordinates
					glm::vec2 diff = (p.getPos() - nodes[i].pos) / cellSize;
					nodes[i].force += energyDerivative * gradWeight(diff);
				}
			}
		}
	}
#ifdef STATS
	maxNodeF = 0.0f;
	maxNodeVelocity = 0.0f;
#endif
	glm::vec2 fG = glm::vec2(0.0f, -9.8f);
	for (int i = 0; i < nodeCount; i++)
	{
		// Only update the node velocity if it is active
		if (nodes[i].active)
		{
			GLfloat invMass = 1.0f / nodes[i].mass;
			// Previously we took the sum of all particles of momentum for the node so we must divide by cells mass
			// I would do this in the velocity update but doing it here is more efficient (one less loop through all the nodes)
			// Unfortunately it does mean the velocity isn't correct inbetween these function calls
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
		p.velocity = glm::vec2(0.0f);
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
				UINT i = y * gridWidth + x;
				const GridNode node = nodes[i];
				if (node.active)
				{
					// Diff between particle's and grid nodes physical position, converted back to grid coordinates
					glm::vec2 diff = (p.getPos() - node.pos) / cellSize;
					GLfloat w = weight(diff);
					glm::vec2 wG = gradWeight(diff);

					picVelocity += node.newVelocity * w;
					flipVelocity += (node.newVelocity - node.velocity) * w;

					p.vG += MathHelp::outer(node.newVelocity, wG);

					p.density += node.mass * w;
				}
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

		GLfloat v = glm::length(p.velocity);
		if (v > maxParticleVelocity)
			maxParticleVelocity = v;
#endif
		p.density *= invNodeArea;
	}
}

void MPMGrid::collision(GLfloat* pos, glm::vec2& v, GLfloat dt)
{
	// Collision
	GLfloat bounds[4] = { origin.x, origin.x + size.x, origin.y, origin.y + size.y };

	// Calculate where it is moving too (without dt)
	glm::vec2 p = glm::vec2(pos[0], pos[1]);
	glm::vec2 advPos = p + v * dt;

	// Lower
	// If the position on the next update goes past the boundary
	if (advPos.y < bounds[2])
		// Adjust the velocity to meet the boundary on the next update
		v.y = p.y - bounds[2];

	// Upper
	if (advPos.y > bounds[3])
		v.y = p.y - bounds[3];

	// Left
	if (advPos.x < bounds[0])
		v.x = p.x - bounds[0];

	// Right
	if (advPos.x > bounds[1])
		v.x = p.x - bounds[1];

	//	if (collision)
	//	{
	//		//if separating, do nothing
	//		vn = dot(velocity, normal);

	//		if (vn >= 0) {
	//			continue;
	//		}

	//		//if get here, need to respond to collision in some way
	//		//if sticky, unconditionally set collision to 0
	//		if (sp.stickyWalls) {
	//			velocity = make_float3(0);
	//			return;
	//		}

	//		//get tangential component of velocity
	//		vt = velocity - vn * normal;

	//		//see if sticking impulse required
	//		//TODO: could have separate static and sliding friction
	//		if (length(vt) <= -sp.frictionCoeff * vn) {
	//			velocity = make_float3(0);
	//			return;
	//		}

	//		//apply dynamic friction
	//		velocity = vt + sp.frictionCoeff * vn * normalize(vt);
	//	}
	//}
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

	// Calculates node velocities from particles
	updateGridVelocities(dt);
	
	// Solving velocities on node level
	/*for (UINT i = 0; i < nodeCount; i++)
	{
		collision(&nodes[i].pos.x, nodes[i].velocity, dt);
	}*/

	// Calculates particle velocities from grid velocities (also calc velocity gradient)
	updateParticleVelocities();

	for (UINT i = 0; i < pointCount; i++)
	{
		// Resolve velocities per particle
		collision(&particles[i].pos->x, particles[i].velocity, dt);

		// Update the particle position using the velocity
		particles[i].updatePos(dt);
		// Update the deformation gradient using the velocity gradient (which was calculated from the velocity)
		particles[i].updateGradient(dt);

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