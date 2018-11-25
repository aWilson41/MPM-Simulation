#pragma once
#include <GL/glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <math.h>
#include <vector>

typedef unsigned int UINT;

// Conversion of Math double of PI to float PI
const GLfloat PI = glm::pi<GLfloat>();
const GLfloat HALFPI = PI * 0.5f;
const GLfloat TWOPI = PI * 2.0f;
const GLfloat TODEGREES = PI / 180.0f;
const GLfloat TORADIANS = 180.0f / PI;
const glm::mat2 I2 = glm::mat2(1.0f);
const glm::mat3 I3 = glm::mat3(1.0f);
const glm::mat4 I4 = glm::mat4(1.0f);

// Forward declartions
namespace geom2d
{
	class Rect;
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
	static glm::mat4 matrixScale(GLfloat s);

	// We define the 2d cross product as the length of the 3d
	static GLfloat cross(glm::vec2 a, glm::vec2 b);

	// Makes a unit vector from radians
	static glm::vec2 slope(GLfloat theta);

	static glm::vec2 projAToB(glm::vec2 a, glm::vec2 b);
	static glm::vec3 projAToB(glm::vec3 a, glm::vec3 b);

    // Triangles
	static GLfloat triangleAreaSigned(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3);
	static GLfloat triangleArea(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3);

    // Computes bary centric coordinates of point p in triangle a,b,c
	static glm::vec3 baryCentric(glm::vec2 p, glm::vec2 a, glm::vec2 b, glm::vec2 c);
	static glm::vec3 baryCentric(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c);

	// Computes ray given screen position, width, height, fov, and near plane
	//static geom::Ray computeEyeRay(glm::vec2 pos, UINT width, UINT height, GLfloat fov, GLfloat nearZ);

	// Computes intersection point on triangle and then bary centric coordinate, returns vec4(u, v, w, intersectionDepth)
	// If failed, returns vec4(0, 0, 0, max)
	//static glm::vec4 triangleRayIntersection(geom::Ray ray, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 n);

	// Simple lerp between start and end given t [0, 1]
	static glm::vec3 lerp(glm::vec3 start, glm::vec3 end, GLfloat t);

	// Catmull rom interpolation between p1 and p2 given t [0, 1]
	static glm::vec3 catmullRom(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, GLfloat t);

	// Calculates the centroid of a polygon
	static glm::vec2 calculateCentroid(glm::vec2* vertices, UINT count);

	// Calculates a bounding rectangle around the point set
	static geom2d::Rect get2dBounds(glm::vec2* vertices, UINT count);
	static geom2d::Rect get2dBounds(glm::vec3* vertices, UINT count);

	// Generates a point cloud in a polygon
	static std::vector<glm::vec3> generatePointCloud(geom2d::Poly* poly, UINT ptCount);

	static bool isPointInPolygon(geom2d::Poly* poly, glm::vec2 pt);

	static GLfloat polygonArea(geom2d::Poly* poly);

	static GLfloat trace(glm::mat2 mat) { return mat[0][0] + mat[1][1]; }

	static glm::mat2 outer(glm::vec2 a, glm::vec2 b);

	template<glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
	static void printMat(glm::mat<C, R, T, Q> m)
	{
		for (glm::length_t r = 0; r < R; r++)
		{
			for (glm::length_t c = 0; c < C - 1; c++)
			{
				printf("%f, ", m[c][r]);
			}
			printf("%f\n", m[C - 1][r]);
		}
	}

	template<glm::length_t N, typename T, glm::qualifier Q>
	static void printVec(glm::vec<N, T, Q> vec)
	{
		for (glm::length_t n = 0; n < N - 1; n++)
		{
			printf("%f, ", vec[n]);
		}
		printf("%f\n", vec[N - 1]);
	}

	template<typename T>
	static T clamp(T val, T min, T max)
	{
		if (val > max)
			return max;
		else if (val < min)
			return min;
		else
			return val;
	}

	static void setData(glm::mat2x2& m, GLfloat m00, GLfloat m01, GLfloat m10, GLfloat m11);

	// Singular value decomp source = u * (s * Identity) * v^T
	static void svd(glm::mat2x2 source, glm::mat2x2* u, glm::vec2* s, glm::mat2x2* v);

	// Polar decomp but only returns the rotational
	static void pd(glm::mat2x2 source, glm::mat2x2* r);

	// Only for square matrices
	template<glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
	static glm::mat<C, R, T, Q> diagProduct(glm::mat<C, R, T, Q> m, glm::vec<C, T, Q> vec)
	{
		glm::mat<C, R, T, Q> results = m;
		for (glm::length_t i = 0; i < C; i++)
		{
			results[i][i] *= vec[i];
		}
		return results;
	}
	// Only for square matrices
	template<glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
	static glm::mat<C, R, T, Q> diagSum(glm::mat<C, R, T, Q> m, glm::vec<C, T, Q> vec)
	{
		glm::mat<C, R, T, Q> results = m;
		for (glm::length_t i = 0; i < C; i++)
		{
			results[i][i] += vec[i];
		}
		return results;
	}
	// Only for square matrices
	template<glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
	static glm::mat<C, R, T, Q> diagSum(glm::mat<C, R, T, Q> m, GLfloat a)
	{
		glm::mat<C, R, T, Q> results = m;
		for (glm::length_t i = 0; i < C; i++)
		{
			results[i][i] += a;
		}
		return results;
	}
};