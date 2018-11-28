#include "ImageMapper.h"
#include "Camera.h"
#include "ImageData.h"
#include "PlaneSource.h"
#include "PolyData.h"
#include "Renderer.h"
#include "Shaders.h"

ImageMapper::ImageMapper()
{
	planeSource = new PlaneSource();
	planeSource->update();
}

ImageMapper::~ImageMapper()
{
	if (planeSource != nullptr)
		delete planeSource;
}

void ImageMapper::setInput(ImageData* data)
{
	if (imageData != nullptr)
		delete imageData;
	imageData = data;
}

void ImageMapper::update()
{
	// Failed to get shader or has no input
	if (imageData == nullptr)
		return;

	pickShader();
	// If one still wasn't picked then don't update
	if (shaderProgram == nullptr)
		return;

	glUseProgram(shaderProgram->getProgramID());

	// Create the plane if it doesn't exist
	if (planeSource == nullptr)
	{
		planeSource = new PlaneSource();
		planeSource->update();
	}

	// Transform the plane the image goes on to be in the XY plane and be the size of the image
	double* bounds = imageData->getBounds();
	glm::vec2 size = glm::vec2(static_cast<GLfloat>(bounds[1] - bounds[0]), static_cast<GLfloat>(bounds[3] - bounds[2]));
	imageSizeMat = MathHelp::matrixScale(size.x, size.y, 1.0f) * MathHelp::matrixRotateX(HALFPI);

	// If the vao and vbo haven't been created yet
	if (vaoID == -1)
	{
		// Generate the vao
		glGenVertexArrays(1, &vaoID);
		glBindVertexArray(vaoID);
		// Gen and allocate space for vbo
		glGenBuffers(1, &vboID);
		glBindBuffer(GL_ARRAY_BUFFER, vboID);
		const GLuint numPts = planeSource->getOutput()->getNumOfPoints();
		glBufferData(GL_ARRAY_BUFFER, numPts * sizeof(GLfloat) * 5, NULL, GL_DYNAMIC_DRAW);

		updateBuffer();
	}
	else
		// VBO size should be constant so no reallocating needed
		updateBuffer();

	// Setup the texture if it hasn't already been created
	const GLuint numComps = imageData->getNumComps();
	GLuint* dim = imageData->getDimensions();
	if (texID == -1)
	{
		glGenTextures(1, &texID);
		glBindTexture(GL_TEXTURE_2D, texID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		if (numComps == 1)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, dim[0], dim[1], 0, GL_RED, GL_UNSIGNED_BYTE, imageData->getData());
		else if (numComps == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, dim[0], dim[1], 0, GL_RGB, GL_UNSIGNED_BYTE, imageData->getData());

		glUniform1i(glGetUniformLocation(shaderProgram->getProgramID(), "tex"), 0);
	}
	// If it already has update the one in there
	else
	{
		glBindTexture(GL_TEXTURE_2D, texID);
		if (numComps == 1)
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, dim[0], dim[1], GL_RED, GL_UNSIGNED_BYTE, imageData->getData());
		else if (numComps == 3)
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, dim[0], dim[1], GL_RGB, GL_UNSIGNED_BYTE, imageData->getData());
	}
}
void ImageMapper::updateBuffer()
{
	const GLfloat* vertexData = planeSource->getOutput()->getVertexData();
	const GLfloat* texCoordData = planeSource->getOutput()->getTexCoordData();
	const GLuint shaderID = shaderProgram->getProgramID();
	const GLint numPts = planeSource->getOutput()->getNumOfPoints();

	glBindBuffer(GL_ARRAY_BUFFER, vboID);

	// Load positional data
	GLint size1 = sizeof(GLfloat) * 3 * numPts;
	glBufferSubData(GL_ARRAY_BUFFER, 0, size1, vertexData);
	// Set it's location and access scheme in vao
	GLuint posAttribLocation = glGetAttribLocation(shaderID, "inPos");
	glEnableVertexAttribArray(posAttribLocation);
	glVertexAttribPointer(posAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (void*)0);

	GLint size2 = sizeof(GLfloat) * 2 * numPts;
	glBufferSubData(GL_ARRAY_BUFFER, size1, size2, texCoordData);
	// Set it's location and access scheme in vao
	GLuint texCoordAttribLocation = glGetAttribLocation(shaderID, "inTexCoord");
	glEnableVertexAttribArray(texCoordAttribLocation);
	glVertexAttribPointer(texCoordAttribLocation, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, (void*)size1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void ImageMapper::pickShader()
{
	// If force shader is on then user of class must set the shader
	if (forceShader)
		return;

	// Shader really only depends on the number of components
	const GLuint numComps = imageData->getNumComps();
	if (numComps == 1)
		shaderProgram = Shaders::getShader("Tex1 Shader");
	else if (numComps == 3)
		shaderProgram = Shaders::getShader("Tex3 Shader");
	else
		shaderProgram = nullptr;
}

void ImageMapper::draw(Renderer* ren)
{
	if (imageData == nullptr || vaoID == -1)
		return;

	// Save and set the poly mode
	GLint polyMode;
	glGetIntegerv(GL_POLYGON_MODE, &polyMode);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glUseProgram(shaderProgram->getProgramID());

	glm::mat4 mvp = ren->getCamera()->proj * ren->getCamera()->view * model * imageSizeMat;
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram->getProgramID(), "mvp_matrix"), 1, GL_FALSE, &mvp[0][0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texID);

	glBindVertexArray(vaoID);
	glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(planeSource->getOutput()->getNumOfPoints()));
	glBindVertexArray(0);

	// Set the poly mode back to what it was
	glPolygonMode(GL_FRONT_AND_BACK, polyMode);
}