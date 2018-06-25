#pragma once
#include <math.h>
#include <vector>
#include <map>
#include <limits>
#include <QtOpenGL\qgl.h>
#include <gtc\matrix_transform.hpp>
#include <glm.hpp>

typedef unsigned int UINT;

namespace mathHelper
{
	class Ray
	{
	public:
		glm::vec3 start;
		glm::vec3 direction;

		Ray(glm::vec3 start, glm::vec3 direction)
		{
			Ray::start = start;
			Ray::direction = direction;
		}
	};

    static glm::mat4 matrixRotateX(GLfloat radians)
    {
        glm::mat4 results = glm::mat4(1.0f);
        results[1][1] = cos(radians);
        results[1][2] = -sin(radians);
        results[2][1] = sin(radians);
        results[2][2] = cos(radians);
        return results;
    }

    static glm::mat4 matrixRotateY(GLfloat radians)
    {
        glm::mat4 results = glm::mat4(1.0f);
        results[0][0] = cos(radians);
        results[0][2] = sin(radians);
        results[2][0] = -sin(radians);
        results[2][2] = cos(radians);
        return results;
    }

    static glm::mat4 matrixRotateZ(GLfloat radians)
    {
        glm::mat4 results = glm::mat4(1.0f);
        results[0][0] = cos(radians);
        results[0][1] = -sin(radians);
        results[1][0] = sin(radians);
        results[1][1] = cos(radians);     
        return results;
    }

    static glm::mat4 matrixTranslate(GLfloat x, GLfloat y, GLfloat z)
    {
        glm::mat4 results = glm::mat4(1.0f);
        results[3][0] = x;
        results[3][1] = y;
        results[3][2] = z;
        return results;
    }

    static glm::mat4 matrixScale(GLfloat x, GLfloat y, GLfloat z)
    {
        glm::mat4 results = glm::mat4(1.0f);
        results[0][0] = x;
        results[1][1] = y;
        results[2][2] = z;
        return results;
    }

    static glm::mat4 matrixScale(float s)
    {
	    return matrixScale(s, s, s);
    }

	// We define the 2d cross product as the length of the 3d
	static GLfloat cross(glm::vec2 a, glm::vec2 b)
	{
		return a.x * b.y - a.y * b.x;
	}

    static glm::vec2 projAToB(glm::vec2 a, glm::vec2 b)
    {
		GLfloat m = b.length();
	    return b * (glm::dot(a, b) / (m * m));
    }

    static glm::vec3 projAToB(glm::vec3 a, glm::vec3 b)
    {
	    GLfloat m = b.length();
	    return b * (glm::dot(a, b) / (m * m));
    }

    // Triangles
    static GLfloat triangleArea(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3)
    {
	    return abs(cross(p2 - p1, p3 - p1)) * 0.5f;
    }

    // Computes bary centric coordinates of point p in triangle a,b,c
    static glm::vec3 baryCentric(glm::vec2 p, glm::vec2 a, glm::vec2 b, glm::vec2 c)
    {
		glm::vec2 v0 = b - a;
	    glm::vec2 v1 = c - a;
		glm::vec2 v2 = p - a;
		GLfloat d00 = glm::dot(v0, v0);
		GLfloat d01 = glm::dot(v0, v1);
		GLfloat d11 = glm::dot(v1, v1);
		GLfloat d20 = glm::dot(v2, v0);
		GLfloat d21 = glm::dot(v2, v1);
		GLfloat denom = d00 * d11 - d01 * d01;
		GLfloat v = (d11 * d20 - d01 * d21) / denom;
		GLfloat w = (d00 * d21 - d01 * d20) / denom;
		GLfloat u = 1.0f - v - w;
	    return glm::vec3(u, v, w);
    }

    static glm::vec3 baryCentric(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c)
    {
		glm::vec3 v0 = b - a;
		glm::vec3 v1 = c - a;
		glm::vec3 v2 = p - a;
		GLfloat d00 = glm::dot(v0, v0);
		GLfloat d01 = glm::dot(v0, v1);
		GLfloat d11 = glm::dot(v1, v1);
		GLfloat d20 = glm::dot(v2, v0);
		GLfloat d21 = glm::dot(v2, v1);
		GLfloat denom = d00 * d11 - d01 * d01;
		GLfloat v = (d11 * d20 - d01 * d21) / denom;
		GLfloat w = (d00 * d21 - d01 * d20) / denom;
		GLfloat u = 1.0f - v - w;
	    return glm::vec3(u, v, w);
    }

	// Computes ray given screen position, width, height, fov, and near plane
    static Ray computeEyeRay(glm::vec2 pos, UINT width, UINT height, float fov, float nearZ)
    {
		GLfloat aspectRatio = static_cast<GLfloat>(width) / height;
		GLfloat s = 2.0f * tan(fov * 0.5f);
	    glm::vec3 start = glm::vec3((pos.x / width - 0.5f) * s /** aspectRatio*/, (pos.y / height - 0.5f) * s, 1.0f) * nearZ;
		return Ray(start, glm::normalize(start));
    }

	// Computes intersection point on triangle and then bary centric coordinate, returns vec4(u, v, w, intersectionDepth)
	// If failed, returns vec4(0, 0, 0, max)
    static glm::vec4 triangleRayIntersection(Ray ray, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 n)
    {
	    // Ray plane intersection
		GLfloat denom = glm::dot(ray.direction, n);
	    if (abs(denom) > 0.0001f) // epsilon
	    {
			GLfloat t = glm::dot(p1 - ray.start, n) / denom;
			glm::vec3 p = ray.start + ray.direction * t;

			glm::vec3 bCoords = baryCentric(p, p1, p2, p3);

		    if (bCoords.x >= 0.0f && bCoords.y >= 0.0f && bCoords.z >= 0.0f)
			    return glm::vec4(bCoords.x, bCoords.y, bCoords.z, p.z);
	    }
	
	    return glm::vec4(0.0f, 0.0f, 0.0f, std::numeric_limits<GLfloat>::max());
    }

	static glm::vec3 lerp(glm::vec3 start, glm::vec3 end, float t)
	{
		return (start + t * (end - start));
	}

	static glm::vec3 catmullRom(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t)
	{
		float t2 = t * t;
		float t3 = t2 * t;
		return 0.5f * ((2.0f * p1) +
			(-p0 + p2) * t +
			(2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
			(-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3);
	}

	static glm::vec2 qPointToVec2(QPoint val)
	{
		return glm::vec2(static_cast<GLfloat>(val.x()), static_cast<GLfloat>(val.y()));
	}
}