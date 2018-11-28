#include "PolyDataMapper.h"
#include "Camera.h"
#include "Material.h"
#include "PolyData.h"
#include "Renderer.h"
#include "Shaders.h"

PolyDataMapper::~PolyDataMapper()
{
	glUseProgram(0);
	if (vaoID != -1)
		glDeleteVertexArrays(1, &vaoID);
	if (vboID != -1)
		glDeleteBuffers(1, &vboID);
}

void PolyDataMapper::update()
{
	const GLfloat* vertexData = polyData->getVertexData();
	const GLfloat* normalData = polyData->getNormalData();
	const GLfloat* texCoordData = polyData->getTexCoordData();
	const GLfloat* scalarData = polyData->getScalarData();

	// Poly data must have vertex data to be visually mapped
	if (vertexData == nullptr)
		return;

	pickShader();
	// If no shader was set don't map anything
	if (shaderProgram == nullptr)
		return;

	glUseProgram(shaderProgram->getProgramID());

	// Initialize if it hasn't been created
	const GLuint numPts = polyData->getNumOfPoints();
	const GLuint shaderID = shaderProgram->getProgramID();
	// Determine size of gpu mem to allocate
	vboSize = sizeof(GLfloat) * 3 * numPts; // Position
	if (polyData->getNormalData() != nullptr)
		vboSize += sizeof(GLfloat) * 3 * numPts; // Normals
	if (polyData->getTexCoordData() != nullptr)
		vboSize += sizeof(GLfloat) * 2 * numPts; // Tex coords
	if (polyData->getScalarData() != nullptr)
		vboSize += sizeof(GLfloat) * 3 * numPts; // Scalars

	// If the vao and vbo haven't been created yet
	if (vaoID == -1)
	{
		// Generate the vao
		glGenVertexArrays(1, &vaoID);
		glBindVertexArray(vaoID);
		// Gen and allocate space for vbo
		glGenBuffers(1, &vboID);
		glBindBuffer(GL_ARRAY_BUFFER, vboID);
		glBufferData(GL_ARRAY_BUFFER, vboSize, NULL, GL_DYNAMIC_DRAW);

		updateBuffer();
	}
	else
	{
		// Get the current vbo size
		GLint bufferSize;
		glBindBuffer(GL_ARRAY_BUFFER, vboID);
		glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);

		// If the size has changed reallocate
		if (vboSize != bufferSize)
		{
			glBufferData(GL_ARRAY_BUFFER, vboSize, NULL, GL_DYNAMIC_DRAW);
			updateBuffer();
		}
		else
			updateBuffer();
	}

	// Verify the desired representation
	CellType type = polyData->getCellType();
	representation = MathHelp::clamp(representation, POINT, type);
}
void PolyDataMapper::updateBuffer()
{
	const GLfloat* vertexData = polyData->getVertexData();
	const GLfloat* normalData = polyData->getNormalData();
	const GLfloat* texCoordData = polyData->getTexCoordData();
	const GLfloat* scalarData = polyData->getScalarData();
	bool hasNormals = (polyData->getNormalData() != nullptr && useNormals);
	bool hasTexCoords = (polyData->getTexCoordData() != nullptr && useTexCoords);
	bool hasScalars = (polyData->getScalarData() != nullptr && useScalars);
	const GLuint shaderID = shaderProgram->getProgramID();
	const GLint numPts = polyData->getNumOfPoints();

	glBindBuffer(GL_ARRAY_BUFFER, vboID);

	// Load positional data
	GLint size = sizeof(GLfloat) * 3 * numPts;
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, vertexData);
	// Set it's location and access scheme in vao
	GLuint posAttribLocation = glGetAttribLocation(shaderID, "inPos");
	glEnableVertexAttribArray(posAttribLocation);
	glVertexAttribPointer(posAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (void*)0);

	GLint offset = size;

	// If it has normal data
	if (hasNormals)
	{
		size = sizeof(GLfloat) * 3 * numPts;
		glBufferSubData(GL_ARRAY_BUFFER, offset, size, normalData);
		// Set it's location and access scheme in vao
		GLuint normalAttribLocation = glGetAttribLocation(shaderID, "inNormal");
		glEnableVertexAttribArray(normalAttribLocation);
		glVertexAttribPointer(normalAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (void*)offset);
		offset += size;
	}
	if (hasTexCoords)
	{
		size = sizeof(GLfloat) * 2 * numPts;
		glBufferSubData(GL_ARRAY_BUFFER, offset, size, texCoordData);
		// Set it's location and access scheme in vao
		GLuint texCoordAttribLocation = glGetAttribLocation(shaderID, "inTexCoord");
		glEnableVertexAttribArray(texCoordAttribLocation);
		glVertexAttribPointer(texCoordAttribLocation, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, (void*)offset);
		offset += size;
	}
	if (hasScalars)
	{
		size = sizeof(GLfloat) * 3 * numPts;
		glBufferSubData(GL_ARRAY_BUFFER, offset, size, scalarData);
		// Set it's location and access scheme in vao
		GLuint scalarsAttribLocation = glGetAttribLocation(shaderID, "inScalars");
		glEnableVertexAttribArray(scalarsAttribLocation);
		glVertexAttribPointer(scalarsAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (void*)offset);
		//offset += size;
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void PolyDataMapper::pickShader()
{
	// If force shader is on then user of class must set the shader
	if (forceShader)
		return;

	// We'll determine what shader to use based on what data is available and what the user wants to use
	bool hasNormals = (polyData->getNormalData() != nullptr && useNormals);
	bool hasTexCoords = (polyData->getTexCoordData() != nullptr && useTexCoords);
	bool hasScalars = (polyData->getScalarData() != nullptr && useScalars);
	// Put each into a separate bit to produce a unique number for each decision
	UINT flag = static_cast<UINT>(hasScalars) |
		(static_cast<UINT>(hasTexCoords) * 2) |
		(static_cast<UINT>(hasNormals) * 4);
	switch (flag)
	{
		// No normals, no texCoords, no scalars (000)
	case 0: shaderProgram = Shaders::getShader("Point Shader");
		break;
		// No normals, no texCoords, has scalars (001)
	case 1: shaderProgram = Shaders::getShader("Color Shader");
		break;
		// No normals, has texCoords, no scalars (010)
	case 2: shaderProgram = Shaders::getShader("Tex Shader");
		break;
		// No normals, has texCoords, has scalars (011)
			//case 3: // There is no shader for this currently. Not that useful
			//	break;
		// Has normals, no texCoords, no scalars (100)
	case 4: shaderProgram = Shaders::getShader("Normal Shader");
		break;
		// Has normals, no texCoords, has scalars (101)
	case 5: shaderProgram = Shaders::getShader("NormalColor Shader");
		break;
		// Has normals, has texCoords, no scalars (110)
	case 6: shaderProgram = Shaders::getShader("NormalTex Shader");
		break;
		// Has normals, has texCoords, has scalars (111)
			//case 7: // There is no shader for this currently. Not that useful
			//	break;
	default:
		shaderProgram = nullptr;
		break;
	};
}

void PolyDataMapper::draw(Renderer* ren)
{
	if (polyData == nullptr || vaoID == -1)
		return;

	// Save the polygon mode
	GLint polyMode;
	glGetIntegerv(GL_POLYGON_MODE, &polyMode);

	// Set the polygon mode needed
	if (representation == TRIANGLE)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (representation == LINE)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else if (representation == POINT)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		glPointSize(pointSize);
	}

	// If the currently bound shader is diff bind the new one
	GLuint programId = shaderProgram->getProgramID();
	glUseProgram(programId);

	// Set the uniforms
	glm::mat4 mvp = ren->getCamera()->proj * ren->getCamera()->view * model;
	glm::vec3 tmp = glm::normalize(glm::vec3(0.0f, 1.0f, 1.0f));
	GLuint lightDirLocation = glGetUniformLocation(programId, "lightDir");
	if (lightDirLocation != -1)
		glUniform3fv(lightDirLocation, 1, &tmp[0]);
	GLuint mvpMatrixLocation = glGetUniformLocation(programId, "mvp_matrix");
	if (mvpMatrixLocation != -1)
		glUniformMatrix4fv(mvpMatrixLocation, 1, GL_FALSE, &mvp[0][0]);
	glm::vec3 diffuse = glm::vec3(0.7f, 0.7f, 0.7f);
	glm::vec3 ambient = glm::vec3(0.0f, 0.0f, 0.0f);
	if (material != nullptr)
	{
		diffuse = material->getDiffuse();
		ambient = material->getAmbient();
	}
	GLuint diffuseColorLocation = glGetUniformLocation(programId, "mat.diffuseColor");
	if (diffuseColorLocation != -1)
		glUniform3fv(diffuseColorLocation, 1, &diffuse[0]);
	GLuint ambientColorLocation = glGetUniformLocation(programId, "mat.ambientColor");
	if (ambientColorLocation != -1)
		glUniform3fv(ambientColorLocation, 1, &ambient[0]);

	glBindVertexArray(vaoID);
	glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(polyData->getNumOfPoints()));
	glBindVertexArray(0);

	// Set the poly mode back to what it was
	glPolygonMode(GL_FRONT_AND_BACK, polyMode);
}