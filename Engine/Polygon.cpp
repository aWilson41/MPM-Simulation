#include "Polygon.h"
#include "Material.h"

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <qopenglvertexarrayobject.h>

Poly::~Poly()
{
	if (vertexBuffer != nullptr)
	{
		vertexBuffer->destroy();
		delete vertexBuffer;
	}
	if (indexBuffer != nullptr)
	{
		indexBuffer->destroy();
		delete indexBuffer;
	}
	delete[] vertexData;
	// Indices don't have to be used
	if (indexData != nullptr)
		delete[] indexData;
	if (faceData != nullptr)
		delete[] faceData;
}

void Poly::setVertexBuffer(VertexData* data, int size)
{
	vertexData = data;
	vertexCount = size;
	vertexBuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
	//vertexBuffer->setUsagePattern(QOpenGLBuffer::DynamicDraw);
	vertexBuffer->create();

	// Transfer vertex data to VBO 0
	vertexBuffer->bind();
	vertexBuffer->allocate(data, sizeof(VertexData) * vertexCount);
	vertexBuffer->release();
	
}

void Poly::setIndexBuffer(GLuint* data, int size)
{
	indexData = data;
	indexCount = size;
	indexBuffer = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
	indexBuffer->create();

	indexBuffer->bind();
	indexBuffer->allocate(indexData, sizeof(GLuint) * indexCount);
	indexBuffer->release();
}

void Poly::setFaceData(Face* data, int size)
{
	faceData = data;
	faceCount = size;
}

void Poly::setShaderProgram(QOpenGLShaderProgram* program)
{
	Poly::program = program;
	posAttributeLocation = program->attributeLocation("inPos");
	normalAttributeLocation = program->attributeLocation("inNormal");
}

void Poly::setUniforms(glm::mat4 viewProj)
{
	glUniformMatrix4fv(program->uniformLocation("mvp_matrix"), 1, GL_FALSE, &(viewProj * world)[0][0]);
	glUniform3f(program->uniformLocation("mat.diffuseColor"), mat->getDiffuse().r, mat->getDiffuse().g, mat->getDiffuse().b);
	glUniform3f(program->uniformLocation("mat.ambientColor"), mat->getAmbient().r, mat->getAmbient().g, mat->getAmbient().b);
}

void Poly::draw(glm::mat4 viewProj)
{
	setUniforms(viewProj);

	if (indexBuffer == nullptr)
	{
		// Tell OpenGL which VBOs to use
		vertexBuffer->bind();

		quintptr offset = 0;

		// Tell the pipeline where to find the vertex data
		program->setAttributeBuffer(posAttributeLocation, GL_FLOAT, offset, 3, sizeof(VertexData));
		program->enableAttributeArray(posAttributeLocation);

		offset += sizeof(glm::vec3);

		// Tell the pipeline where to find the normal data
		program->setAttributeBuffer(normalAttributeLocation, GL_FLOAT, offset, 3, sizeof(VertexData));
		program->enableAttributeArray(normalAttributeLocation);

		// Draw the triangles
		glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertexCount));

		program->disableAttributeArray(posAttributeLocation);
		program->disableAttributeArray(normalAttributeLocation);
		vertexBuffer->release();
	}
	else
	{
		// Tell OpenGL which VBOs to use
		vertexBuffer->bind();
		indexBuffer->bind();

		quintptr offset = 0;

		// Tell the pipeline where to find the vertex data
		program->setAttributeBuffer(posAttributeLocation, GL_FLOAT, offset, 3, sizeof(VertexData));
		program->enableAttributeArray(posAttributeLocation);

		offset += sizeof(glm::vec3);

		// Tell the pipeline where to find the normal data
		program->setAttributeBuffer(normalAttributeLocation, GL_FLOAT, offset, 3, sizeof(VertexData));
		program->enableAttributeArray(normalAttributeLocation);

		// Draw the triangles
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, 0);

		program->disableAttributeArray(posAttributeLocation);
		program->disableAttributeArray(normalAttributeLocation);
		vertexBuffer->release();
		indexBuffer->release();
	}
}