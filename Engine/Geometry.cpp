#include "Geometry.h"

namespace geom
{
	Shape::Shape()
	{
		pos = glm::vec2(0.0f);
		type = ShapeType::POINT;
	}
	Shape::Shape(glm::vec2 pos)
	{
		Shape::pos = pos;
		type = ShapeType::POINT;
	}
	GLfloat Shape::area() { return 0.0f; }

	Circle::Circle()
	{
		radius = 0.0f;
		type = ShapeType::CIRCLE;
	}
	Circle::Circle(glm::vec2 pos, GLfloat radius)
	{
		Circle::pos = pos;
		Circle::radius = radius;
		type = ShapeType::CIRCLE;
	}
	Circle::Circle(GLfloat x, GLfloat y, GLfloat radius)
	{
		Circle::pos = glm::vec2(x, y);
		Circle::radius = radius;
		type = ShapeType::CIRCLE;
	}
	GLfloat Circle::area() { return TWOPI * radius; }

	Rect::Rect()
	{
		extent = glm::vec2(0.0f);
		type = ShapeType::RECT;
	}
	Rect::Rect(glm::vec2 center, glm::vec2 size)
	{
		pos = center;
		extent = size * 0.5f;
		type = ShapeType::RECT;
	}

	GLfloat Rect::area() { return extent.x * extent.y * 2.0f; }

	Poly::Poly() { type = ShapeType::POLY; }
	Poly::Poly(glm::vec2* vertices, unsigned int count)
	{
		// Copy the vertices
		for (unsigned int i = 0; i < count; i++)
		{
			this->vertices[i] = vertices[i];
		}
		type = ShapeType::POLY;
		pos = MathHelp::calculateCentroid(vertices, count);
	}
	void Poly::FromCircle(Circle circle, unsigned int divisions)
	{
		vertices = std::vector<glm::vec2>(divisions);
		float dTheta = TWOPI / divisions;
		for (unsigned int i = 0; i < divisions; i++)
		{
			vertices[i] = MathHelp::slope(dTheta * i) * circle.radius + circle.pos;
		}
		pos = circle.pos;
	}


	Ray::Ray(glm::vec3 start, glm::vec3 direction)
	{
		Ray::start = start;
		Ray::direction = direction;
	}
}