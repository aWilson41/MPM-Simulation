#pragma once
#include "MathHelper.h"

class Material;
class PolyData;
class Renderer;
class ShaderProgram;

// Does rendering and pre rendering operations (mapping input to visual representation)
class PolyDataMapper
{
public:
	~PolyDataMapper();

public:
	PolyData* getInput() { return polyData; }
	Material* getMaterial() { return material; }

	void setInput(PolyData* input) { polyData = input; }
	void setShaderProgram(ShaderProgram* shaderProgram) { PolyDataMapper::shaderProgram = shaderProgram; }
	void setMaterial(Material* material) { PolyDataMapper::material = material; }
	void setModelMatrix(glm::mat4 model) { PolyDataMapper::model = model; }

	// Updates the buffer based on the set poly data. If it's the first time it initializes the buffer
	void update();

	void draw(Renderer* ren);

protected:
	PolyData* polyData = nullptr;
	Material* material = nullptr;
	glm::mat4 model = glm::mat4(1.0f);
	GLuint vboID = -1;
	GLuint vaoID = -1;

	ShaderProgram* shaderProgram = nullptr;
};