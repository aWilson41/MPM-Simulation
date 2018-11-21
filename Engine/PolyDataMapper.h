#pragma once
#include "MathHelper.h"

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
	void setInput(PolyData* input) { polyData = input; }
	void setShaderProgram(ShaderProgram* shaderProgram) { PolyDataMapper::shaderProgram = shaderProgram; }

	// Updates the buffer based on the set poly data. If it's the first time it initializes the buffer
	void update();

	void draw(Renderer* ren);

protected:
	PolyData* polyData = nullptr;
	GLuint vboID = -1;

	ShaderProgram* shaderProgram = nullptr;
};