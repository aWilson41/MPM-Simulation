#include "Shaders.h"
#include <string>

#define LoadShader(shaderName, vsPath, fsPath) \
	{ ShaderProgram* shader = new ShaderProgram(shaderName); \
		shader->loadVertexShader(vsPath); \
		shader->loadFragmentShader(fsPath); \
		shader->compileProgram(); \
		shaders.push_back(shader); }

namespace Shaders
{
	std::vector<ShaderProgram*> shaders;

	ShaderProgram* getShader(std::string name)
	{
		for (unsigned int i = 0; i < shaders.size(); i++)
		{
			if (shaders[i]->getName() == name)
				return shaders[i];
		}
		return nullptr;
	}

	// Initialize some basic shaders
	void initShaders()
	{
		LoadShader("Point Shader", "Shaders/pointVS.glsl", "Shaders/pointFS.glsl");
		LoadShader("Color Shader", "Shaders/colorVS.glsl", "Shaders/colorFS.glsl");
		LoadShader("Tex3 Shader", "Shaders/texVS.glsl", "Shaders/tex3FS.glsl");
		LoadShader("Tex1 Shader", "Shaders/texVS.glsl", "Shaders/tex1FS.glsl");
		LoadShader("Normal Shader", "Shaders/normalVS.glsl", "Shaders/normalFS.glsl");
		LoadShader("NormalColor Shader", "Shaders/normalColorVS.glsl", "Shaders/normalColorFS.glsl");
		LoadShader("NormalTex3 Shader", "Shaders/normalTexVS.glsl", "Shaders/normalTex3FS.glsl");
	}

	void deleteShaders()
	{
		for (unsigned int i = 0; i < shaders.size(); i++)
		{
			shaders[i]->release();
			delete shaders[i];
		}
	}
};