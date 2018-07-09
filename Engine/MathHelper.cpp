#include "MathHelper.h"
#include "Geometry.h"

glm::mat4 MathHelp::matrixRotateX(GLfloat radians)
{
	glm::mat4 results = glm::mat4(1.0f);
	results[1][1] = cos(radians);
	results[1][2] = -sin(radians);
	results[2][1] = sin(radians);
	results[2][2] = cos(radians);
	return results;
}
glm::mat4 MathHelp::matrixRotateY(GLfloat radians)
{
	glm::mat4 results = glm::mat4(1.0f);
	results[0][0] = cos(radians);
	results[0][2] = sin(radians);
	results[2][0] = -sin(radians);
	results[2][2] = cos(radians);
	return results;
}
glm::mat4 MathHelp::matrixRotateZ(GLfloat radians)
{
	glm::mat4 results = glm::mat4(1.0f);
	results[0][0] = cos(radians);
	results[0][1] = -sin(radians);
	results[1][0] = sin(radians);
	results[1][1] = cos(radians);
	return results;
}
glm::mat4 MathHelp::matrixTranslate(GLfloat x, GLfloat y, GLfloat z)
{
	glm::mat4 results = glm::mat4(1.0f);
	results[3][0] = x;
	results[3][1] = y;
	results[3][2] = z;
	return results;
}
glm::mat4 MathHelp::matrixScale(GLfloat x, GLfloat y, GLfloat z)
{
	glm::mat4 results = glm::mat4(1.0f);
	results[0][0] = x;
	results[1][1] = y;
	results[2][2] = z;
	return results;
}
glm::mat4 MathHelp::matrixScale(GLfloat s) { return matrixScale(s, s, s); }

// We define the 2d cross product as the length of the 3d
GLfloat MathHelp::cross(glm::vec2 a, glm::vec2 b) { return a.x * b.y - a.y * b.x; }

glm::vec2 MathHelp::slope(GLfloat theta) { return glm::vec2(glm::cos(theta), glm::sin(theta)); }

glm::vec2 MathHelp::projAToB(glm::vec2 a, glm::vec2 b)
{
	GLfloat m = b.length();
	return b * (glm::dot(a, b) / (m * m));
}
glm::vec3 MathHelp::projAToB(glm::vec3 a, glm::vec3 b)
{
	GLfloat m = b.length();
	return b * (glm::dot(a, b) / (m * m));
}

// Triangles
GLfloat MathHelp::triangleAreaSigned(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3) { return cross(p2 - p1, p3 - p1) * 0.5f; }
GLfloat MathHelp::triangleArea(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3) { return abs(triangleAreaSigned(p1, p2, p3)); }
// Computes intersection point on triangle and then bary centric coordinate, returns vec4(u, v, w, intersectionDepth)
// If failed, returns vec4(0, 0, 0, max)
glm::vec4 MathHelp::triangleRayIntersection(geom::Ray ray, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 n)
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

// Computes bary centric coordinates of point p in triangle a,b,c
glm::vec3 MathHelp::baryCentric(glm::vec2 p, glm::vec2 a, glm::vec2 b, glm::vec2 c)
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
glm::vec3 MathHelp::baryCentric(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c)
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
geom::Ray MathHelp::computeEyeRay(glm::vec2 pos, UINT width, UINT height, GLfloat fov, GLfloat nearZ)
{
	GLfloat aspectRatio = static_cast<GLfloat>(width) / height;
	GLfloat s = 2.0f * tan(fov * 0.5f);
	glm::vec3 start = glm::vec3((pos.x / width - 0.5f) * s /** aspectRatio*/, (pos.y / height - 0.5f) * s, 1.0f) * nearZ;
	return geom::Ray(start, glm::normalize(start));
}

glm::vec3 MathHelp::lerp(glm::vec3 start, glm::vec3 end, GLfloat t) { return (start + t * (end - start)); }

glm::vec3 MathHelp::catmullRom(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, GLfloat t)
{
	GLfloat t2 = t * t;
	GLfloat t3 = t2 * t;
	return 0.5f * ((2.0f * p1) + (-p0 + p2) * t +
		(2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
		(-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3);
}

glm::vec2 MathHelp::calculateCentroid(glm::vec2* vertices, UINT count)
{
	glm::vec2 centroid(0.0f);
	GLfloat signedArea = 0.0f;
	glm::vec2 v1; // Current vertex
	glm::vec2 v2; // Next vertex
	GLfloat a = 0.0f; // Partial signed area
	UINT i = 0;
	for (i = 0; i < count - 1; ++i)
	{
		v1 = vertices[i];
		v2 = vertices[i + 1];
		a = cross(v1, v2);
		signedArea += a;
		centroid = (v1 + v2) * a;
	}

	// Do last vertex separately to avoid modulus operation in each iteration.
	v1 = vertices[i];
	v2 = vertices[0];
	a = cross(v1, v2);
	signedArea += a;
	signedArea *= 0.5f;

	return ((centroid + (v1 + v2) * a) / (6.0f * signedArea));
}

geom::Rect MathHelp::getBounds(glm::vec2* vertices, UINT count)
{
	GLfloat maxX = std::numeric_limits<GLfloat>::min();
	GLfloat minX = std::numeric_limits<GLfloat>::max();
	GLfloat maxY = std::numeric_limits<GLfloat>::min();
	GLfloat minY = std::numeric_limits<GLfloat>::max();

	for (UINT i = 0; i < count; i++)
	{
		if (vertices[i].x > maxX)
			maxX = vertices[i].x;
		if (vertices[i].x < minX)
			minX = vertices[i].x;
		if (vertices[i].y > maxY)
			maxY = vertices[i].y;
		if (vertices[i].y < minY)
			minY = vertices[i].y;
	}
	glm::vec2 size = glm::vec2(maxX - minX, maxY - minY);
	glm::vec2 center = glm::vec2(minX, minY) + size * 0.5f;

	return geom::Rect(center, size);
}

std::vector<glm::vec2> MathHelp::generatePointCloud(geom::Poly* poly, UINT numPts)
{
	std::vector<glm::vec2> results;

	geom::Rect bounds = getBounds(poly->vertices.data(), static_cast<UINT>(poly->vertices.size()));
	glm::vec2 size = bounds.extent * 2.0f;

	while (results.size() < numPts)
	{
		// Generate a random point
		// Generate random in 0, 1000
		glm::vec2 newPt = glm::vec2(static_cast<GLfloat>(rand() % 1000), static_cast<GLfloat>(rand() % 1000));
		// Change random to [-1, 1]
		newPt = newPt / 500.0f - 1.0f;
		// Change random to [-(width or height) / 2, (width or height) / 2] and add center
		newPt = newPt * bounds.extent + bounds.pos;

		// Check if the point lies in the polygon
		if (isPointInPolygon(poly, newPt))
			results.push_back(newPt);
	}

	return results;
}

bool MathHelp::isPointInPolygon(geom::Poly* poly, glm::vec2 pt)
{
	bool result = false;
	UINT len = static_cast<UINT>(poly->vertices.size());
	for (UINT i = 0, j = len - 1; i < len; j = i++)
	{
		glm::vec2& vi = poly->vertices[i];
		glm::vec2& vj = poly->vertices[j];
		if ((vi.y > pt.y) != (vj.y > pt.y) && (pt.x < (vj.x - vi.x) * (pt.y - vi.y) / (vj.y - vi.y) + vi.x))
			result = !result;
	}
	return result;
}

GLfloat MathHelp::polygonArea(geom::Poly* poly)
{
	GLfloat area = 0.0f;
	std::vector<glm::vec2>& vertices = poly->vertices;
	UINT count = static_cast<UINT>(vertices.size());
	for (UINT i = 0; i < count - 1; i++)
	{
		area += vertices[i].x * vertices[i + 1].y - vertices[i + 1].x * vertices[i].y;
	}
	area += vertices[count - 1].x * vertices[0].y - vertices[0].x * vertices[count - 1].y;
	return area * 0.5f;
}