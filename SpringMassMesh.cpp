#include "SpringMassMesh.h"

#include <QOpenGLBuffer>

void SpringMassMesh::setupSpringMesh()
{
	setupSpringMesh(indexData, indexCount);
}

void SpringMassMesh::setupSpringMesh(GLuint* indices, int size)
{
	particles = std::vector<Particle>(vertexCount);
	for (int i = 0; i < vertexCount; i++)
	{
		particles[i].pos = &vertexData[i].pos;
	}

	// Add springs for every edge
	int springCount = size / 2;
	springs = std::vector<Spring>(springCount);
	for (int i = 0; i < springCount; i++)
	{
		Particle* p1 = &particles[indices[i * 2]];
		Particle* p2 = &particles[indices[i * 2 + 1]];
		springs[i].p1 = p1;
		springs[i].p2 = p2;
		float length = glm::distance(p2->getPos(), p1->getPos());
		springs[i].restingLength = length;
		springs[i].kd = 0.9f;
		springs[i].ks = 350.0f;// 0.5f * length;
	}
}

void SpringMassMesh::update(GLfloat dt, GLfloat g)
{
	for (UINT i = 0; i < springs.size(); i++)
	{
		springs[i].applySpringForce();
	}

	// If hit's plane
	glm::vec3 n = glm::vec3(0.0f, 1.0f, 0.0f);
	for (unsigned int i = 0; i < particles.size(); i++)
	{
		// Resolve it to -30 if less than -30
		if (particles[i].getPos()[1] < -80.0f)
		{
			particles[i].getPos()[1] = -80.0f;

			// Remove the velocity in the direction of the plane
			float l = glm::dot(n, particles[i].velocity);
			if (l < 0.0f)
				particles[i].velocity -= l * n;
			l = glm::dot(n, particles[i].force);
			if (l < 0.0f)
				particles[i].force -= l * n;
		}
	}

	for (UINT i = 0; i < particles.size(); i++)
	{
		particles[i].applyForce(glm::vec3(0.0f, g, 0.0f));
		particles[i].integrate(dt);
	}

	// Update the normals
	calculateNormals();

	for (UINT i = 0; i < particles.size(); i++)
	{
		particles[i].resetForce();
	}
}

// 0 <= stepNumber <= lastStepNumber
GLfloat lerp(GLfloat a, GLfloat b, GLfloat t)
{
	return a + (b - a) * t;
}

glm::vec3 lerpColor(glm::vec3 a, glm::vec3 b, GLfloat t)
{
	glm::vec3 color = glm::vec3(lerp(a[0], b[0], t), lerp(a[1], b[1], t), lerp(a[2], b[2], t));
	return color;
}

// Normalized force 0-1
glm::vec3 getColor(float force)
{
	// 4 Colors
	glm::vec3 black = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 red = glm::vec3(1.0f, 0.0f, 0.0f);
	GLfloat redThreshold = 0.2f;
	glm::vec3 yellow = glm::vec3(1.0f, 1.0f, 0.0f);
	GLfloat yellowThreshold = 0.4f;
	glm::vec3 white = glm::vec3(1.0f, 1.0f, 1.0f);
	GLfloat whiteThreshold = 1.0f;

	// Threshold
	// In between black and red
	if (force < redThreshold)
		return lerpColor(black, red, force / redThreshold);
	else if (force < yellowThreshold)
		return lerpColor(red, yellow, (force - redThreshold) / (yellowThreshold - redThreshold));
	else if (force < whiteThreshold)
		return lerpColor(yellow, white, (force - yellowThreshold) / (whiteThreshold - yellowThreshold));
	else
		return white;
}

void SpringMassMesh::calculateNormals()
{
	// For every neighbor
	for (int i = 0; i < neighbors.size(); i++)
	{
		int numNeighborFaces = neighbors[i].size();
		glm::vec3 sum = glm::vec3(0.0f, 0.0f, 0.0f);
		// For every face
		for (int j = 0; j < numNeighborFaces; j++)
		{
			glm::vec3 diff1 = neighbors[i][j]->v3->pos - neighbors[i][j]->v2->pos;
			glm::vec3 diff2 = neighbors[i][j]->v3->pos - neighbors[i][j]->v1->pos;
			sum += glm::normalize(glm::cross(diff1, diff2));
		}
		//vertexData[i].normal = getColor(glm::length(particles[i].force / 800.0f));
		vertexData[i].normal = glm::normalize(sum / static_cast<float>(numNeighborFaces));
	}

	// Update the buffer (Only holds boundary points to draw the mesh)
	vertexBuffer->bind();
	void* bufferData = vertexBuffer->map(QOpenGLBuffer::WriteOnly);
	memcpy(bufferData, vertexData, vertexCount * sizeof(VertexData));
	vertexBuffer->unmap();
	vertexBuffer->release();
}