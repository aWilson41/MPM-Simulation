#pragma once
#include "MathHelper.h"

class PolyDataMapper;
class ShaderProgram;

// Does the rendering, mostly just managing the scene (there is no scene object)
class Renderer
{
public:
	Renderer();
	~Renderer();

public:
	void render();

	// Might split mapper into actor where this becomes addActor
	void addRenderItem(PolyDataMapper* mapper) { mappers.push_back(mapper); }

	// Returns the currently bound shader
	ShaderProgram* getCurrentShaderProgram() { return currShaderProgram; }
	void setCurrentShaderProgram(ShaderProgram* program) { currShaderProgram = program; }

protected:
	// Will eventually hold actors instead of mappers
	std::vector<PolyDataMapper*> mappers;
	ShaderProgram* currShaderProgram = nullptr;
	bool initialized = false;
};