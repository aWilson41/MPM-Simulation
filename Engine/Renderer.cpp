#include "Renderer.h"
#include "PolyDataMapper.h"

void Renderer::render()
{
	for (UINT i = 0; i < mappers.size(); i++)
	{
		mappers[i]->draw();
	}
}