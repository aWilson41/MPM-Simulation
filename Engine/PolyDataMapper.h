#pragma once
#include "AbstractMapper.h"
#include "MathHelper.h"

class Material;
class PolyData;
class ShaderProgram;

enum PolyRep
{
	POINTREP,
	LINEREP,
	TRIANGLEREP
};

// Does rendering and pre rendering operations (mapping input to visual representation)
class PolyDataMapper : public AbstractMapper
{
public:
	~PolyDataMapper();

public:
	PolyData* getInput() { return polyData; }
	ShaderProgram* getShaderProgram() { return shaderProgram; }
	Material* getMaterial() { return material; }
	glm::mat4 getModelMatrix() { return model; }
	PolyRep getPolyRepresentation() { return representation; }
	GLfloat getPointSize() { return pointSize; }

	void setInput(PolyData* input) { polyData = input; }
	void setShaderProgram(ShaderProgram* shaderProgram) { PolyDataMapper::shaderProgram = shaderProgram; }
	void setMaterial(Material* material) { PolyDataMapper::material = material; }
	void setModelMatrix(glm::mat4 model) { PolyDataMapper::model = model; }
	void setPolyRepresentation(PolyRep representation) { PolyDataMapper::representation = representation; }
	void setPointSize(GLfloat pointSize) { PolyDataMapper::pointSize = pointSize; }

	// Updates the buffer based on the set poly data. If it's the first time it initializes the buffer
	void update() override;

	void draw(Renderer* ren) override;

protected:
	PolyData* polyData = nullptr;
	Material* material = nullptr;
	glm::mat4 model = glm::mat4(1.0f);
	GLuint vboID = -1;
	GLuint vaoID = -1;

	PolyRep representation = TRIANGLEREP;
	GLfloat pointSize = 4.0f;

	ShaderProgram* shaderProgram = nullptr;
};