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
		// Generate the buffer, bind, then set data
		glGenBuffers(1, &vboID);
		glBindBuffer(GL_ARRAY_BUFFER, vboID);
		glBufferData(GL_ARRAY_BUFFER, polyData->getNumOfPoints() * sizeof(VertexData), polyData->getData(), GL_STATIC_DRAW);

		// Generate the vao
		glGenVertexArrays(1, &vaoID);
		glBindVertexArray(vaoID);

		// Position
		GLuint shaderID = shaderProgram->getProgramID();
		GLuint posAttribLocation = glGetAttribLocation(shaderID, "inPos");
		glEnableVertexAttribArray(posAttribLocation);
		glVertexAttribPointer(glGetAttribLocation(shaderID, "inPos"), 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);

		// Normal
		GLuint normalAttribLocation = glGetAttribLocation(shaderID, "inNormal");
		glEnableVertexAttribArray(normalAttribLocation);
		glVertexAttribPointer(glGetAttribLocation(shaderID, "inNormal"), 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(glm::vec3)));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// If it has been created it still may not be useable
	/*else
	{
		glBindBuffer(GL_ARRAY_BUFFER, vboID);
		glBufferData(GL_ARRAY_BUFFER, polyData->getNumOfPoints() * sizeof(VertexData), polyData->getData(), GL_STATIC_DRAW);
	}*/
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
	glm::vec3 tmp = glm::normalize(glm::vec3(0.0f, 1.0f, 1.0f));
	glUniform3f(glGetUniformLocation(programId, "lightDir"), tmp[0], tmp[1], tmp[2]);
	glUniformMatrix4fv(glGetUniformLocation(programId, "mvp_matrix"), 1, GL_FALSE, &mvp[0][0]);
	glUniform3f(glGetUniformLocation(programId, "mat.diffuseColor"), material->getDiffuse().r, material->getDiffuse().g, material->getDiffuse().b);
	glUniform3f(glGetUniformLocation(programId, "mat.ambientColor"), material->getAmbient().r, material->getAmbient().g, material->getAmbient().b);

	glBindVertexArray(vaoID);

	glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(polyData->getNumOfPoints()));

	glBindVertexArray(0);
}