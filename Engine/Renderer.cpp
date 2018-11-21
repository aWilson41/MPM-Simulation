#include "Renderer.h"
#include "PolyDataMapper.h"
#include "Engine/Shaders.h"

Renderer::Renderer()
{
	Shaders::initShaders();
}

Renderer::~Renderer()
{
	Shaders::deleteShaders();
}

void Renderer::render()
{
	for (UINT i = 0; i < mappers.size(); i++)
	{
		mappers[i]->draw(this);
	}
}