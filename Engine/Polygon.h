#pragma once
#include "Engine\MathHelper.h"

#include <QOpenGLFunctions>

class QOpenGLShaderProgram;
class QOpenGLBuffer;
class Material;

struct VertexData
{
	glm::vec3 pos;
	//glm::vec2 texCoords;
	glm::vec3 normal;
};

struct Face
{
	VertexData* v1;
	VertexData* v2;
	VertexData* v3;
};

class Poly : protected QOpenGLFunctions
{
public:
	Poly() { initializeOpenGLFunctions(); }
	~Poly();

	void setVertexBuffer(VertexData* data, int size);
	void setIndexBuffer(GLuint* data, int size);
	void setFaceData(Face* data, int size);

	void setMaterial(Material* mat) { Poly::mat = mat; }

	// Each polygon gets a pointer to the shader it will use
	void setShaderProgram(QOpenGLShaderProgram* program);

	void setUniforms(glm::mat4 viewProj);
	void setName(std::string name) { Poly::name = name; }
	void draw(glm::mat4 viewProj);

public:
	glm::mat4 world = glm::mat4(1.0f);
	std::string name = "None";

public:
	QOpenGLShaderProgram* program;
	int posAttributeLocation = -1;
	int normalAttributeLocation = -1;

	QOpenGLBuffer* vertexBuffer = nullptr;
	QOpenGLBuffer* indexBuffer = nullptr;

	// Contains per vertex data of the mesh
	VertexData* vertexData = nullptr;
	int vertexCount = -1;
	GLuint* indexData = nullptr;
	int indexCount = -1;
	Face* faceData = nullptr;
	int faceCount = -1;

public:
	Material* mat;
};