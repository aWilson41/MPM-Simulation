#pragma once
#include "Engine\MathHelper.h"
#include "Engine\Polygon.h"
#include "Spring.h"
#include "Particle.h"

class SpringMassMesh : public Poly
{
public:
	void setupSpringMesh();
	void setupSpringMesh(GLuint* indices, int size);

	void update(GLfloat dt, GLfloat g);
	void calculateNormals();

protected:
	std::vector<Particle> particles;
	std::vector<Spring> springs;

public:
	std::vector<std::vector<Face*>> neighbors;
};