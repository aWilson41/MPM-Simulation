#include "ImageMapper.h"
#include "Camera.h"
#include "PolyData.h"
#include "PlaneSource.h"
#include "Renderer.h"
#include "ImageData.h"
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

void ImageMapper::update()
{
	// Failed to get shader or has no input
	if (imageData == nullptr)
		return;

	UINT numComps = imageData->getNumComps();
	UINT* dim = imageData->getDimensions();
	if (numComps == 1)
		shaderProgram = Shaders::getShader("GrayImage Shader");
	else if (numComps == 3)
		shaderProgram = Shaders::getShader("Image Shader");
	else
		return;

	// Create the plane
	// If it already exists delete it and recreate it
	if (planeSource == nullptr)
	{
		planeSource = new PlaneSource();
		planeSource->update();
	}

	// Plane is unit plane (-0.5, 0.5)
	double* bounds = imageData->getBounds();
	glm::vec2 size = glm::vec2(static_cast<GLfloat>(bounds[1] - bounds[0]), static_cast<GLfloat>(bounds[3] - bounds[2]));
	glm::vec2 cellSize = size / glm::vec2(dim[0], dim[1]);
	// Center of border pixels is the boundary so add the cellSize
	//imageSizeMat = MathHelp::matrixScale(size.x + cellSize.x, size.y + cellSize.y, 1.0f) * MathHelp::matrixRotateX(HALFPI);
	imageSizeMat = MathHelp::matrixScale(size.x, size.y, 1.0f) * MathHelp::matrixRotateX(HALFPI);

	// Setup the planes vbo if it hasn't already been created
	if (vaoID == -1)
	{
		// Generate the buffer, bind, then set data
		glGenBuffers(1, &vboID);
		glBindBuffer(GL_ARRAY_BUFFER, vboID);
		glBufferData(GL_ARRAY_BUFFER, planeSource->getOutput()->getNumOfPoints() * sizeof(VertexData), planeSource->getOutput()->getData(), GL_STATIC_DRAW);

		// Generate the vao
		glGenVertexArrays(1, &vaoID);
		glBindVertexArray(vaoID);

		// Position
		GLuint shaderID = shaderProgram->getProgramID();
		GLuint posAttribLocation = glGetAttribLocation(shaderID, "inPos");
		glEnableVertexAttribArray(posAttribLocation);
		glVertexAttribPointer(posAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);

		GLint offset = sizeof(glm::vec3);

		// Normal
		/*GLuint normalAttribLocation = glGetAttribLocation(shaderID, "inNormal");
		glEnableVertexAttribArray(normalAttribLocation);
		glVertexAttribPointer(normalAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offset);*/

		offset += sizeof(glm::vec3);

		GLuint texAttribLocation = glGetAttribLocation(shaderID, "inTexCoord");
		glEnableVertexAttribArray(texAttribLocation);
		glVertexAttribPointer(texAttribLocation, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offset);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	// Setup the texture if it hasn't already been created
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

void ImageMapper::draw(Renderer* ren)
{
	if (imageData == nullptr || vaoID == -1)
		return;

	// Save the polygon mode
	GLint polyMode;
	glGetIntegerv(GL_POLYGON_MODE, &polyMode);

	// Set the polygon mode needed
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// If the currently bound shader is diff bind the new one
	if (shaderProgram != ren->getCurrentShaderProgram())
	{
		ren->setCurrentShaderProgram(shaderProgram);
		glUseProgram(shaderProgram->getProgramID());
	}

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