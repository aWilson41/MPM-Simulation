#pragma once

#include "MathHelper.h"

namespace geom
{
	enum ShapeType
	{
		POINT = 0,
		CIRCLE = 1,
		RECT = 2,
		POLY = 3
	};

	class Shape
	{
	public:
		Shape();
		Shape(glm::vec2 pos);

	public:
		virtual GLfloat area();

	public:
		ShapeType type = ShapeType::POINT;
		glm::vec2 pos = glm::vec2(0.0f);
	};

	class Circle : public Shape
	{
	public:
		Circle();
		Circle(glm::vec2 pos, GLfloat radius);
		Circle(GLfloat x, GLfloat y, GLfloat radius);

	public:
		GLfloat area() override;

	public:
		GLfloat radius = 0.0f;
	};

	class Rect : public Shape
	{
	public:
		Rect();
		Rect(glm::vec2 center, glm::vec2 size);

	public:
		GLfloat area() override;
		glm::vec2 topLeft();
		glm::vec2 topRight();
		glm::vec2 bottomRight();
		glm::vec2 bottomLeft();

	public:
		glm::vec2 extent = glm::vec2(0.0f);
	};

	class Poly : public Shape
	{
	public:
		Poly();
		Poly(glm::vec2* vertices, unsigned int count);

	public:
		void FromCircle(Circle circle, unsigned int divisions);
		// Signed area
		GLfloat area() override;

	public:
		std::vector<glm::vec2> vertices;
	};


	class Ray
	{
	public:
		Ray(glm::vec3 start, glm::vec3 direction);

	public:
		glm::vec3 start;
		glm::vec3 direction;
	};
}