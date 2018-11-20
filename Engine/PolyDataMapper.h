#pragma once

class PolyData;
class Renderer;
class ShaderProgram;

// Does rendering and pre rendering operations (mapping input to visual representation)
class PolyDataMapper
{
public:
	PolyData* getInput() { return polyData; }
	void setShaderProgram(ShaderProgram* shaderProgram) { PolyDataMapper::shaderProgram = shaderProgram; }

	void draw(Renderer* ren);

protected:
	PolyData* polyData = nullptr;
	GLuint vboID = -1;

	ShaderProgram* shaderProgram = nullptr;
};