#include "PolyDataMapper.h"
#include "Camera.h"
#include "Material.h"
#include "PolyData.h"
#include "Renderer.h"
#include "ShaderProgram.h"
#include <GL/glew.h>

PolyDataMapper::~PolyDataMapper()
{
	glDeleteBuffers(1, &vboID);
}

void PolyDataMapper::update()
{
	// Initialize if it hasn't been created
	if (vboID == -1)
	{
		// Generate the buffer, bind, then set
		glGenBuffers(1, &vboID);
		glBindBuffer(GL_ARRAY_BUFFER, vboID);
		glBufferData(GL_ARRAY_BUFFER, polyData->getNumOfPoints() * sizeof(VertexData), polyData->getData(), GL_STATIC_DRAW);
	}
	// If it has been created it still may not be useable
	else
	{
		glBindBuffer(GL_ARRAY_BUFFER, vboID);
		glBufferData(GL_ARRAY_BUFFER, polyData->getNumOfPoints() * sizeof(VertexData), polyData->getData(), GL_STATIC_DRAW);
	}
}

void PolyDataMapper::draw(Renderer* ren)
{
	if (polyData == nullptr || vboID == -1)
		return;

	// If the currently bound shader is diff bind the new one
	if (shaderProgram != ren->getCurrentShaderProgram())
	{
		ren->setCurrentShaderProgram(shaderProgram);
		glUseProgram(shaderProgram->getProgramID());
	}

	glm::mat4 mvp = ren->getCamera()->proj * ren->getCamera()->view * model;

	GLuint programId = shaderProgram->getProgramID();
	glUniform3f(glGetUniformLocation(programId, "lightDir"), 0.0f, 0.707f, 0.707f);
	glUniformMatrix4fv(glGetUniformLocation(programId, "mvp_matrix"), 1, GL_FALSE, &mvp[0][0]);
	glUniform3f(glGetUniformLocation(programId, "mat.diffuseColor"), material->getDiffuse().r, material->getDiffuse().g, material->getDiffuse().b);
	glUniform3f(glGetUniformLocation(programId, "mat.ambientColor"), material->getAmbient().r, material->getAmbient().g, material->getAmbient().b);

	glBindBuffer(GL_ARRAY_BUFFER, vboID);

	// Position
	//int p = glGetAttribLocation(programId, "inPos");
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);

	// Normal
	//p = glGetAttribLocation(programId, "inNormal");
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);

	// Tex coords
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, sizeof(VertexData), (void*)0);

	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(polyData->getNumOfPoints()));

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	/*glDisableVertexAttribArray(2);*/

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}