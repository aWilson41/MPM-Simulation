#include "PolyDataMapper.h"
#include "Camera.h"
#include "Material.h"
#include "PolyData.h"
#include "Renderer.h"
#include "Shaders.h"

PolyDataMapper::~PolyDataMapper()
{
	glUseProgram(0);
	glDeleteVertexArrays(1, &vaoID);
	glDeleteBuffers(1, &vboID);
}

void PolyDataMapper::update()
{
	if (shaderProgram == nullptr)
		shaderProgram = Shaders::getShader("Norm Shader");

	// Initialize if it hasn't been created
	if (vaoID == -1)
	{
		// Generate the buffer, bind, then set data
		glGenBuffers(1, &vboID);
		glBindBuffer(GL_ARRAY_BUFFER, vboID);
		glBufferData(GL_ARRAY_BUFFER, polyData->getNumOfPoints() * sizeof(VertexData), polyData->getData(), GL_DYNAMIC_DRAW);

		// Generate the vao
		glGenVertexArrays(1, &vaoID);
		glBindVertexArray(vaoID);

		// Position
		GLuint shaderID = shaderProgram->getProgramID();
		GLuint posAttribLocation = glGetAttribLocation(shaderID, "inPos");
		glEnableVertexAttribArray(posAttribLocation);
		glVertexAttribPointer(posAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
		// Normal
		GLuint normalAttribLocation = glGetAttribLocation(shaderID, "inNormal");
		glEnableVertexAttribArray(normalAttribLocation);
		glVertexAttribPointer(normalAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(glm::vec3)));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		// Point polygon data can only be represented by points
		CellType type = polyData->getCellType();
		if (type == POINT)
			representation = POINTREP;
		// Line poly data can only be represented by points or lines
		else if (type == LINE)
		{
			if (representation > LINEREP)
				representation = LINEREP;
		}
	}
	else
	{
		GLint bufferSize;
		glBindBuffer(GL_ARRAY_BUFFER, vboID);
		glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
		GLint targetSize = polyData->getNumOfPoints() * sizeof(VertexData);

		// If the size hasn't changed don't reallocate
		if (bufferSize == targetSize)
			glBufferSubData(GL_ARRAY_BUFFER, 0, bufferSize, polyData->getData());
		else
		{
			// Orphan the data
			glBufferData(GL_ARRAY_BUFFER, bufferSize, NULL, GL_DYNAMIC_DRAW);
			// Reallocate
			glBufferData(GL_ARRAY_BUFFER, bufferSize, polyData->getData(), GL_DYNAMIC_DRAW);
		}
	}
}

void PolyDataMapper::draw(Renderer* ren)
{
	if (polyData == nullptr || vaoID == -1)
		return;

	// Save the polygon mode
	GLint polyMode;
	glGetIntegerv(GL_POLYGON_MODE, &polyMode);

	// Set the polygon mode needed
	if (representation == TRIANGLEREP)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (representation == LINEREP)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else if (representation == POINTREP)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		glPointSize(pointSize);
	}

	// If the currently bound shader is diff bind the new one
	if (shaderProgram != ren->getCurrentShaderProgram())
	{
		ren->setCurrentShaderProgram(shaderProgram);
		glUseProgram(shaderProgram->getProgramID());
	}

	glm::mat4 mvp = ren->getCamera()->proj * ren->getCamera()->view * model;

	GLuint programId = shaderProgram->getProgramID();
	glm::vec3 tmp = glm::normalize(glm::vec3(0.0f, 1.0f, 1.0f));
	glUniform3fv(glGetUniformLocation(programId, "lightDir"), 1, &tmp[0]);
	glUniformMatrix4fv(glGetUniformLocation(programId, "mvp_matrix"), 1, GL_FALSE, &mvp[0][0]);
	glm::vec3 diffuse = glm::vec3(0.7f, 0.7f, 0.7f);
	glm::vec3 ambient = glm::vec3(0.0f, 0.0f, 0.0f);
	if (material != nullptr)
	{
		diffuse = material->getDiffuse();
		ambient = material->getAmbient();
	}
	glUniform3fv(glGetUniformLocation(programId, "mat.diffuseColor"), 1, &diffuse[0]);
	glUniform3fv(glGetUniformLocation(programId, "mat.ambientColor"), 1, &ambient[0]);

	glBindVertexArray(vaoID);
	glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(polyData->getNumOfPoints()));
	glBindVertexArray(0);

	// Set the poly mode back to what it was
	glPolygonMode(GL_FRONT_AND_BACK, polyMode);
}