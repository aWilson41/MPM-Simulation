#pragma once
#include "MathHelper.h"

class PolyDataMapper;

// Does the rendering, mostly just managing the scene (there is no scene object)
class Renderer
{
public:
	void render();

	// Returns the currently bound shader
	GLuint getCurrentShaderProgram() { return currShaderProgram; }

protected:
	// Will eventually hold actors instead of mappers
	std::vector<PolyDataMapper*> mappers;

	GLuint currShaderProgram = -1;
};