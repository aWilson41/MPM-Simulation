#include "PointCloud.h"
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

void PointCloud::setPoints(glm::vec3* pts, UINT size)
{
	// Copy the data locally
	PointCloud::pts = std::vector<glm::vec3>(size);
	for (UINT i = 0; i < size; i++)
	{
		PointCloud::pts[i] = pts[i];
	}

	// Setup the vertex buffer
	vertexBuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
	//vertexBuffer->setUsagePattern(QOpenGLBuffer::DynamicDraw);
	vertexBuffer->create();

	// Transfer vertex data to VBO 0
	vertexBuffer->bind();
	vertexBuffer->allocate(PointCloud::pts.data(), static_cast<int>(sizeof(glm::vec3) * PointCloud::pts.size()));
	vertexBuffer->release();
}

void PointCloud::setUniforms(glm::mat4 viewProj)
{
	glUniformMatrix4fv(shaderProgram->uniformLocation("mvp_matrix"), 1, GL_FALSE, &(viewProj * world)[0][0]);
}

void PointCloud::setShaderProgram(QOpenGLShaderProgram* program)
{
	shaderProgram = program;
	posAttributeLocation = shaderProgram->attributeLocation("inPos");
}

// Updates the full buffer
void PointCloud::updateBuffer()
{
	// Update the buffer
	vertexBuffer->bind();
	void* bufferData = vertexBuffer->map(QOpenGLBuffer::WriteOnly);
	memcpy(bufferData, pts.data(), pts.size() * sizeof(glm::vec3));
	vertexBuffer->unmap();
	vertexBuffer->release();
}

void PointCloud::draw(glm::mat4 viewProj)
{
	//glEnable(GL_PROGRAM_POINT_SIZE);
	//glPointSize(10);
	setUniforms(viewProj);

	// Tell OpenGL which VBO to use
	vertexBuffer->bind();

	quintptr offset = 0;

	// Tell the pipeline where to find the vertex data
	shaderProgram->setAttributeBuffer(posAttributeLocation, GL_FLOAT, offset, 3, sizeof(glm::vec3));
	shaderProgram->enableAttributeArray(posAttributeLocation);
	offset += sizeof(glm::vec3);

	// Draw the Points
	glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(pts.size()));

	shaderProgram->disableAttributeArray(posAttributeLocation);
	vertexBuffer->release();
}