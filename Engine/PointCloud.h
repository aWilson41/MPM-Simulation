#pragma once
#include "MathHelper.h"
#include <QOpenGLFunctions>

class QOpenGLShaderProgram;
class QOpenGLBuffer;

class PointCloud : protected QOpenGLFunctions
{
public:
	PointCloud() { initializeOpenGLFunctions(); }

	void setPoints(glm::vec3* pts, UINT size);
	void setUniforms(glm::mat4 viewProj);
	void setShaderProgram(QOpenGLShaderProgram* program);

	// Updates the full buffer
	void updateBuffer();

	void Draw(glm::mat4 viewProj);

public:
	glm::mat4 world = glm::mat4(1.0f);
	std::vector<glm::vec3> pts;

	QOpenGLBuffer* vertexBuffer = nullptr;

protected:
	QOpenGLShaderProgram* shaderProgram;
	int posAttributeLocation = -1;
};