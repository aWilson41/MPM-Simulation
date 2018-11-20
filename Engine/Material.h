//#pragma once
//
//#include "MathHelper.h"
//
//class Material
//{
//public:
//	Material()
//	{
//		diffuseColor = glm::vec3(0.0f);
//		ambientColor = glm::vec3(0.0f);
//	}
//
//	Material(glm::vec3 diffuseColor, glm::vec3 ambientColor)
//	{
//		Material::diffuseColor = diffuseColor;
//		Material::ambientColor = ambientColor;
//	}
//
//	void setAmbientToDiffuse(float fraction = 1.0f)
//	{
//		ambientColor = diffuseColor * fraction;
//	}
//
//	void setDiffuse(glm::vec3 diffuseColor) { Material::diffuseColor = diffuseColor; }
//	void setDiffuse(GLfloat r, GLfloat g, GLfloat b) { setDiffuse(glm::vec3(r, g, b)); }
//
//	void setAmbient(glm::vec3 ambientColor) { Material::ambientColor = ambientColor; }
//	void setAmbient(GLfloat r, GLfloat g, GLfloat b) { setAmbient(glm::vec3(r, g, b)); }
//
//	glm::vec3 getDiffuse() { return diffuseColor; }
//	glm::vec3 getAmbient() { return ambientColor; }
//
//protected:
//	glm::vec3 diffuseColor;
//	glm::vec3 ambientColor;
//};