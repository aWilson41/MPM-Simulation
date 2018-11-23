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
	outputData->allocate(2, TRIANGLE);
	VertexData* vertexData = static_cast<VertexData*>(outputData->getData());
	// Defines a 1x1x1 plane
	vertexData[0].pos = origin;
	vertexData[1].pos = p2;
	vertexData[2].pos = p1;

	vertexData[3].pos = p2;
	vertexData[4].pos = p1 + p2 - origin;
	vertexData[5].pos = p1;

	vertexData[0].normal = vertexData[1].normal =
		vertexData[2].normal = vertexData[3].normal =
		vertexData[4].normal = vertexData[5].normal = glm::vec3(0.0f, 1.0f, 0.0f);

	vertexData[0].texCoords = glm::vec2(0.0f, 0.0f);
	vertexData[1].texCoords = vertexData[3].texCoords = glm::vec2(0.0f, 1.0f);
	vertexData[2].texCoords = vertexData[5].texCoords = glm::vec2(1.0f, 0.0f);
	vertexData[4].texCoords = glm::vec2(1.0f, 1.0f);
}