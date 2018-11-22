#pragma once
#include "MathHelper.h"

class Camera;
class Material;
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
	void addMaterial(Material* material) { materials.push_back(material); }

	Material* getMaterial(UINT i) { return materials[i]; }
	PolyDataMapper* getRenderItem(UINT i) { return mappers[i]; }

	// Returns the currently bound shader
	ShaderProgram* getCurrentShaderProgram() { return currShaderProgram; }
	Camera* getCamera() { return cam; }

	void setCurrentShaderProgram(ShaderProgram* program) { currShaderProgram = program; }
	void setCamera(Camera* cam) { Renderer::cam = cam; }

protected:
	// Will eventually hold actors instead of mappers
	std::vector<PolyDataMapper*> mappers;
	std::vector<Material*> materials;
	Camera* cam = nullptr;
	ShaderProgram* currShaderProgram = nullptr;
	bool initialized = false;
};