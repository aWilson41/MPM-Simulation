#include "Primitives.h"
#include "PolyData.h"

PlaneSource::PlaneSource()
{
	outputData = new PolyData();
}
PlaneSource::~PlaneSource()
{
	if (outputData != nullptr)
		delete outputData;
}
void PlaneSource::update()
{
	if (outputData == nullptr)
		return;

	// Allocate room for two triangles (6 vertices)
	outputData->allocate(2);
	VertexData* vertexData = static_cast<VertexData*>(outputData->getData());
	// Defines a 1x1x1 plane
	vertexData[0].pos = glm::vec3(-0.5f, 0.0f, -0.5f);
	vertexData[1].pos = glm::vec3(-0.5f, 0.0f, 0.5f);
	vertexData[2].pos = glm::vec3(0.5f, 0.0f, -0.5f);

	vertexData[3].pos = glm::vec3(-0.5f, 0.0f, 0.5f);
	vertexData[4].pos = glm::vec3(0.5f, 0.0f, 0.5f);
	vertexData[5].pos = glm::vec3(0.5f, 0.0f, -0.5f);
	vertexData[0].normal = vertexData[1].normal =
		vertexData[2].normal = vertexData[3].normal =
		vertexData[4].normal = vertexData[5].normal = glm::vec3(0.0f, 1.0f, 0.0f);
}