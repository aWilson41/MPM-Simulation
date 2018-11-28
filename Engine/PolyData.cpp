#include "PolyData.h"

void PolyData::allocateVertexData(UINT cellCount, CellType type)
{
	numCells = cellCount;
	if (vertexData != nullptr)
		delete[] vertexData;

	switch (type)
	{
	case POINT:
		numPts = cellCount;
		break;
	case LINE:
		numPts = cellCount * 2;
		break;
	case TRIANGLE:
		numPts = cellCount * 3;
		break;
	case QUAD:
		numPts = cellCount * 4;
		break;
	default:
		break;
	}

	vertexData = new GLfloat[numPts * 3];
	memset(vertexData, 0, sizeof(GLfloat) * numPts * 3);
}
void PolyData::allocateNormalData()
{
	if (normalData != nullptr)
		delete[] normalData;
	normalData = new GLfloat[numPts * 3];
	memset(normalData, 0, sizeof(GLfloat) * numPts * 3);
}
void PolyData::allocateTexCoords()
{
	if (texCoordData != nullptr)
		delete[] texCoordData;
	texCoordData = new GLfloat[numPts * 2];
	memset(texCoordData, 0, sizeof(GLfloat) * numPts * 2);
}
void PolyData::allocateScalarData(UINT numComps)
{
	if (scalarData != nullptr)
		delete[] scalarData;
	scalarData = new GLfloat[numPts * numComps];
	memset(scalarData, 0, sizeof(GLfloat) * numPts * numComps);
}