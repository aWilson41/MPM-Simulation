#pragma once
#include "AbstractMapper.h"
#include "MathHelper.h"
#include "Types.h"

class Material;
class PolyData;
class ShaderProgram;

// Does rendering and pre rendering operations (mapping input to visual representation)
// Generalized class, makes no assumptions about data provided too it at the cost of some optimization
class PolyDataMapper : public AbstractMapper
{
public:
	~PolyDataMapper();

public:
	PolyData* getInput() { return polyData; }
	ShaderProgram* getShaderProgram() { return shaderProgram; }
	Material* getMaterial() { return material; }
	glm::mat4 getModelMatrix() { return model; }
	CellType getPolyRepresentation() { return representation; }
	GLfloat getPointSize() { return pointSize; }

	void setInput(PolyData* input);
	void setShaderProgram(ShaderProgram* shaderProgram)
	{
		PolyDataMapper::shaderProgram = shaderProgram;
		forceShader = true;
	}
	void setMaterial(Material* material) { PolyDataMapper::material = material; }
	void setModelMatrix(glm::mat4 model) { PolyDataMapper::model = model; }
	void setPolyRepresentation(CellType representation) { PolyDataMapper::representation = representation; }
	void setPointSize(GLfloat pointSize) { PolyDataMapper::pointSize = pointSize; }
	void setUseNormals(bool val) { useNormals = val; }
	void setUseTexCoords(bool val) { useTexCoords = val; }
	void setUseScalars(bool val) { useScalars = val; }

	// Updates the buffer based on the set poly data. If it's the first time it initializes the buffer
	void update() override;

	void draw(Renderer* ren) override;

protected:
	void updateBuffer();
	void pickShader();

protected:
	PolyData* polyData = nullptr;
	Material* material = nullptr;
	glm::mat4 model = glm::mat4(1.0f);

	GLuint vboID = -1;
	GLint vboSize = -1;
	GLuint vaoID = -1;

	CellType representation = TRIANGLE;
	GLfloat pointSize = 4.0f;

	ShaderProgram* shaderProgram = nullptr;
	bool useNormals = true;
	bool useTexCoords = false; // Turns true went texture is set
	bool useScalars = true;
};