#include "PlaneSource.h"
#include "PolyData.h"

PlaneSource::PlaneSource() { outputData = new PolyData(); }
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
	outputData->allocateVertexData(2, TRIANGLE);
	GLfloat* vData = outputData->getVertexData();
	glm::vec3* vertexData = reinterpret_cast<glm::vec3*>(outputData->getVertexData());
	// Defines a 1x1x1 plane
	vertexData[0] = origin;
	vertexData[1] = p2;
	vertexData[2] = p1;

	vertexData[3] = p2;
	vertexData[4] = p1 + p2 - origin;
	vertexData[5] = p1;

	outputData->allocateNormalData();
	glm::vec3* normalData = reinterpret_cast<glm::vec3*>(outputData->getNormalData());
	normalData[0] = normalData[1] =
		normalData[2] = normalData[3] =
		normalData[4] = normalData[5] = glm::vec3(0.0f, 1.0f, 0.0f);

	outputData->allocateTexCoords();
	glm::vec2* texCoordData = reinterpret_cast<glm::vec2*>(outputData->getTexCoordData());
	texCoordData[0] = glm::vec2(0.0f, 0.0f);
	texCoordData[1] = texCoordData[3] = glm::vec2(0.0f, 1.0f);
	texCoordData[2] = texCoordData[5] = glm::vec2(1.0f, 0.0f);
	texCoordData[4] = glm::vec2(1.0f, 1.0f);
}