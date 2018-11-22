#pragma once
#include "ShaderProgram.h"
#include <vector>

namespace Shaders
{
	extern std::vector<ShaderProgram*> shaders;

	extern ShaderProgram* getShader(std::string name);

	// Initialize some basic shaders
	extern void initShaders();

	extern void deleteShaders();
};