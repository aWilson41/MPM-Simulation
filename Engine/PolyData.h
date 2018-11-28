#pragma once
#include "MathHelper.h"
#include "Types.h"
#include <vector>

// Actual container for polygonal data. Can stores points, lines, triangles, and quads
class PolyData
{
public:
	~PolyData()
	{
		if (vertexData != nullptr)
			delete[] vertexData;
		if (normalData != nullptr)
			delete[] normalData;
		if (texCoordData != nullptr)
			delete[] texCoordData;
		if (scalarData != nullptr)
			delete[] scalarData;
	}

public:
	UINT getNumOfPoints() { return numPts; }
	UINT getNumOfCells() { return numCells; }
	CellType getCellType() { return type; }

	GLfloat* getVertexData() { return vertexData; }
	GLfloat* getNormalData() { return normalData; }
	GLfloat* getTexCoordData() { return texCoordData; }
	GLfloat* getScalarData() { return scalarData; }
	void allocateVertexData(UINT cellCount, CellType type);
	void allocateNormalData();
	void allocateTexCoords();
	void allocateScalarData(UINT numComps);

protected:
	// Series of optional data arrays
	// Will probably refactor to be a table of arrays soon, more flexible (VTK does this)
	GLfloat* vertexData = nullptr; // 3 comp float array
	GLfloat* normalData = nullptr; // 3 comp float array
	GLfloat* texCoordData = nullptr; // 2 comp float array
	GLfloat* scalarData = nullptr; // N comp float array

	UINT numPts = -1;
	UINT numCells = -1;
	CellType type = TRIANGLE;
};