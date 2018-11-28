#include "MPMGrid.h"
#include "Constants.h"
#include "Particle.h"

// If this is on we will collect max values to track instabilities
//#define STATS

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
glm::vec2 MPMGrid::gradWeight(const glm::vec2 pos) { return glm::vec2(dNX(pos.x) * NX(pos.y), dNX(pos.y) * NX(pos.x)) * invCellSize; }

MPMGrid::~MPMGrid()
{
	if (nodes != nullptr)
		delete[] nodes;
}

void MPMGrid::initGrid(glm::vec2 origin, glm::vec2 size, int width, int height)
{
	MPMGrid::size = size;
	gridWidth = width;
	gridHeight = height;
	nodeCount = gridWidth * gridHeight;
	nodes = new GridNode[nodeCount];
	cellSize = size / glm::vec2(gridWidth, gridHeight);
	invCellSize = 1.0f / cellSize;
	MPMGrid::origin = origin;
	bounds[0] = origin.x;
	bounds[1] = origin.x + size.x;
	bounds[2] = origin.y;
	bounds[3] = origin.y + size.y;
}

void MPMGrid::initParticles(Particle* particles, UINT count)
{
	MPMGrid::particles = particles;
	pointCount = count;

	initMass();
	// Calculate densities and volumes
	GLfloat invNodeArea = 1.0f / (cellSize.x * cellSize.y);
	for (UINT i = 0; i < pointCount; i++)
	{
		Particle& p = particles[i];
		p.density = p.volume = 0.0f;
		for (UINT y = p.startY; y < p.endY; y++)
		{
			for (UINT x = p.startX; x < p.endX; x++)
			{
				UINT index = y * gridWidth + x;
				glm::vec2 diff = (p.getPos() - nodes[index].pos) * invCellSize;
				p.density += weight(diff) * nodes[index].mass;
			}
		}
		p.density *= invNodeArea;
		p.volume = p.mass / p.density;
	}
}


void MPMGrid::initMass()
{
	for (int y = 0; y < gridHeight; y++)
	{
		for (int x = 0; x < gridWidth; x++)
		{
			// Physical coordinates of the node, spaced like |-x--x--x--x--x-|
			nodes[y * gridWidth + x] = GridNode(glm::vec2(x, y) * cellSize + cellSize * 0.5f + origin);
		}
	}

	// Weight the mass of the particles into the nodes
	for (UINT i = 0; i < pointCount; i++)
	{
		Particle& p = particles[i];
		glm::vec2 pPos = p.getPos();
		int gridPosX = static_cast<int>((pPos.x - origin.x) * invCellSize.x);
		int gridPosY = static_cast<int>((pPos.y - origin.y) * invCellSize.y);
		p.startX = static_cast<UINT>(MathHelp::clamp(gridPosX - 2, 0, gridWidth));
		p.endX = static_cast<UINT>(MathHelp::clamp(gridPosX + 3, 0, gridWidth));
		p.startY = static_cast<UINT>(MathHelp::clamp(gridPosY - 2, 0, gridHeight));
		p.endY = static_cast<UINT>(MathHelp::clamp(gridPosY + 3, 0, gridHeight));
		p.inBounds = p.startX != p.endX || p.startY != p.endY;
		for (UINT y = p.startY; y < p.endY; y++)
		{
			for (UINT x = p.startX; x < p.endX; x++)
			{
				UINT i = y * gridWidth + x;
				// Diff between particle's and grid nodes physical position, converted back to grid coordinates
				glm::vec2 diff = (pPos - nodes[i].pos) * invCellSize;
				nodes[i].mass += p.mass * weight(diff);
			}
		}
	}
}

// Puts particle momentums onto the grid (as velocity)
void MPMGrid::initVelocities()
{
	// Sum all the particles momentum to their nearby nodes (weighted)
	for (UINT i = 0; i < pointCount; i++)
	{
		Particle& p = particles[i];
		glm::vec2 pPos = p.getPos();
		for (UINT y = p.startY; y < p.endY; y++)
		{
			for (UINT x = p.startX; x < p.endX; x++)
			{
				UINT i = y * gridWidth + x;
				// Diff between particle's and grid nodes physical position, converted back to grid coordinates
				glm::vec2 diff = (pPos - nodes[i].pos) * invCellSize;
				nodes[i].velocity += p.velocity * p.mass * weight(diff);
				// As long as the mass isn't 0 and it has some velocity it is active
				nodes[i].active = (nodes[i].mass != 0.0f);
			}
		}
	}
	// Calculate the actual velocity. Mark zero mass nodes as non-active
	for (int i = 0; i < nodeCount; i++)
	{
		if (nodes[i].mass != 0.0f)
			nodes[i].velocity /= nodes[i].mass;
	}
}

// Updates velocities based on stress and gravity
void MPMGrid::updateGridVelocities(GLfloat dt)
{
	for (UINT i = 0; i < pointCount; i++)
	{
		Particle& p = particles[i];
		// Initial volume * cauchy. Energy derivative is force.
		glm::mat2 energyDerivative = -p.volume * p.calcCauchyStress();
		for (UINT y = p.startY; y < p.endY; y++)
		{
			for (UINT x = p.startX; x < p.endX; x++)
			{
				UINT i = y * gridWidth + x;
				// Diff between particle's and grid nodes physical position, converted back to grid coordinates
				glm::vec2 diff = (p.getPos() - nodes[i].pos) * invCellSize;
				nodes[i].force += energyDerivative * gradWeight(diff);
			}
		}
	}
#ifdef STATS
	maxNodeForceMag = 0.0f;
	maxNodeForce = glm::vec2(0.0f, 0.0f);
	maxNodeVelocity = 0.0f;
#endif
	glm::vec2 g = glm::vec2(0.0f, -9.8f);
	for (int i = 0; i < nodeCount; i++)
	{
		// Only update the node velocity if it is active
		if (nodes[i].mass != 0.0f)
		{
			nodes[i].force += g * nodes[i].mass;
			// Update node velocity given force (we don't really need to do the above unless we want a "valid" force)
			nodes[i].newVelocity = nodes[i].velocity + (nodes[i].force / nodes[i].mass) * dt;

#ifdef STATS
			GLfloat nodeV = glm::length(nodes[i].newVelocity);
			if (nodeV > maxNodeVelocity)
				maxNodeVelocity = nodeV;
			GLfloat nodeF = glm::length(nodes[i].force);
			if (nodeF > maxNodeForceMag)
			{
				maxNodeForceMag = nodeF;
				maxNodeForce = nodes[i].force;
			}
#endif
		}
	}
}

void MPMGrid::updateParticleVelocities()
{
#ifdef STATS
	maxParticleVGDet = 0.0f;
	maxParticleVG = glm::mat2(0.0f);
	maxParticleVelocityMag = 0.0f;
	maxParticleVelocity = glm::vec2(0.0f, 0.0f);
#endif
	GLfloat invNodeArea = 1.0f / (cellSize.x * cellSize.y);
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

		for (UINT y = p.startY; y < p.endY; y++)
		{
			for (UINT x = p.startX; x < p.endX; x++)
			{
				UINT i = y * gridWidth + x;
				GridNode node = nodes[i];
				// Diff between particle's and grid nodes physical position, converted back to grid coordinates
				glm::vec2 diff = (p.getPos() - node.pos) * invCellSize;
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

		GLfloat v = glm::length(p.velocity);
		if (v > maxParticleVelocityMag)
		{
			maxParticleVelocityMag = v;
			maxParticleVelocity = p.velocity;
		}
#endif
		p.density *= invNodeArea;
	}
}

void MPMGrid::collision(glm::vec2 pos, glm::vec2& v, GLfloat dt)
{
	// Collision
	glm::vec2 normal = glm::vec2(0.0f);
	bool collision = false;
	glm::vec2 vt = glm::vec2(0.0f);
	GLfloat vn = 0.0f;

	GLfloat bounds[4] = { origin.x, origin.x + size.x, origin.y, origin.y + size.y };
	for (UINT i = 0; i < 2; i++)
	{
		collision = false;
		if (i == 0)
		{
			if (pos.x <= bounds[0])
			{
				collision = true;
				normal = glm::vec2(1.0f, 0.0f);
			}
			else if (pos.x >= bounds[1])
			{
				collision = true;
				normal = glm::vec2(-1.0f, 0.0f);
			}
		}
		if (i == 1)
		{
			if (pos.y <= bounds[2])
			{
				printf("test\n");
				collision = true;
				normal = glm::vec2(0.0f, 1.0f);
			}
			else if (pos.y >= bounds[3])
			{
				collision = true;
				normal = glm::vec2(0.0f, -1.0f);
			}
		}

		if (collision)
		{
			// If separating, do nothing
			vn = glm::dot(v, normal);
			if (vn >= 0)
				continue;

			// Get tangent velocity by removing velocity in normal dir
			vt = v - vn * normal;
			// Until vt surpasses this value don't let it move (static friction)
			/*if (glm::length(vt) <= -FRICTION * vn)
			{
				v = glm::vec3(0.0f);
				return;
			}*/
			// Apply dynamic friction
			v = vt + FRICTION * vn * normalize(vt);
		}
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
	maxParticleDefe = maxParticleDefp = glm::mat2(1.0f);
	maxParticleDefDete = maxParticleDefDetp = 0.0f;
#endif

	// Calculates node velocities from particles (3, 4)
	updateGridVelocities(dt);

	// Solve velocities on node level (5)
	for (int i = 0; i < nodeCount; i++)
	{
		// Still working on this
		collision(nodes[i].pos + nodes[i].newVelocity * cellSize * dt, nodes[i].newVelocity, dt);
	}

	// Calculates particle velocities from grid velocities (also calc velocity gradient)
	updateParticleVelocities();

	for (UINT i = 0; i < pointCount; i++)
	{
		// Solve velocities per particle (doesn't introduce anything to velocity gradient)
		collision(particles[i].getPos(), particles[i].velocity, dt);

		// Update the particle position using the velocity
		particles[i].updatePos(dt);
		// Update the deformation gradient using the velocity gradient (which was calculated from the velocity)
		particles[i].updateGradient(dt);

#ifdef STATS
		GLfloat defGDet = glm::determinant(particles[i].defG);
		if (defGDet > maxParticleDefDete)
		{
			maxParticleDefDete = defGDet;
			maxParticleDefe = particles[i].defGe;
		}
		defGDet = glm::determinant(particles[i].defGp);
		if (defGDet > maxParticleDefDetp)
		{
			maxParticleDefDetp = defGDet;
			maxParticleDefp = particles[i].defGp;
		}
#endif
	}
}