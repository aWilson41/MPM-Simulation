#pragma once
#include <math.h>
#include <QtOpenGL\qgl.h>
#include <gtc\matrix_transform.hpp>

typedef unsigned int UINT;

// Conversion of Math double of PI to float PI
const GLfloat PI = glm::pi<GLfloat>();
const GLfloat HALFPI = PI * 0.5f;
const GLfloat TWOPI = HALFPI * 2.0f;
const GLfloat TODEGREES = PI / 180.0f;
const GLfloat TORADIANS = 180.0f / PI;

// Forward declartions
namespace geom
{
	class Rect;
	class Ray;
	class Poly;
}

class MathHelp
{
public:
	static glm::mat4 matrixRotateX(GLfloat radians);
	static glm::mat4 matrixRotateY(GLfloat radians);
	static glm::mat4 matrixRotateZ(GLfloat radians);
	static glm::mat4 matrixTranslate(GLfloat x, GLfloat y, GLfloat z);
	static glm::mat4 matrixScale(GLfloat x, GLfloat y, GLfloat z);
	static glm::mat4 matrixScale(float s);

	// We define the 2d cross product as the length of the 3d
	static GLfloat cross(glm::vec2 a, glm::vec2 b);

	// Makes a unit vector from radians
	static glm::vec2 slope(float theta);

	static glm::vec2 projAToB(glm::vec2 a, glm::vec2 b);
	static glm::vec3 projAToB(glm::vec3 a, glm::vec3 b);

    // Triangles
	static GLfloat triangleAreaSigned(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3);
	static GLfloat triangleArea(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3);

    // Computes bary centric coordinates of point p in triangle a,b,c
	static glm::vec3 baryCentric(glm::vec2 p, glm::vec2 a, glm::vec2 b, glm::vec2 c);
	static glm::vec3 baryCentric(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c);

	// Computes ray given screen position, width, height, fov, and near plane
	static geom::Ray computeEyeRay(glm::vec2 pos, UINT width, UINT height, float fov, float nearZ);

	// Computes intersection point on triangle and then bary centric coordinate, returns vec4(u, v, w, intersectionDepth)
	// If failed, returns vec4(0, 0, 0, max)
	static glm::vec4 triangleRayIntersection(geom::Ray ray, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 n);

	// Simple lerp between start and end given t [0, 1]
	static glm::vec3 lerp(glm::vec3 start, glm::vec3 end, float t);

	// Catmull rom interpolation between p1 and p2 given t [0, 1]
	static glm::vec3 catmullRom(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t);

	// Convience function to convert QPoint to glm::vec2
	static glm::vec2 qPointToVec2(QPoint val) { return glm::vec2(static_cast<GLfloat>(val.x()), static_cast<GLfloat>(val.y())); }

	// Calculates the centroid of a polygon
	static glm::vec2 calculateCentroid(glm::vec2* vertices, unsigned int count);

	// Calculates a bounding rectangle around the point set
	static geom::Rect getBounds(glm::vec2* vertices, unsigned int count);

	// Generates a point cloud in a polygon
	static std::vector<glm::vec2> generatePointCloud(geom::Poly poly, unsigned int numPts);
};