#include "Shaders.h"

namespace Shaders
{
	std::vector<ShaderProgram*> shaders;

	ShaderProgram* getShader(std::string name)
	{
		for (unsigned int i = 0; i < shaders.size(); i++)
		{
			if (shaders[i]->getName() == name)
				return shaders[i];
		}
		return nullptr;
	}

	// Initialize some basic shaders
	void initShaders()
	{
		ShaderProgram* shader1 = new ShaderProgram("Norm Shader");
		shader1->loadVertexShader("Shaders/vertexShader.glsl");
		shader1->loadFragmentShader("Shaders/fragShader.glsl");
		shader1->compileProgram();
		shaders.push_back(shader1);

		ShaderProgram* shader2 = new ShaderProgram("Image Shader");
		shader2->loadVertexShader("Shaders/imageVertexShader.glsl");
		shader2->loadFragmentShader("Shaders/imageFragShader.glsl");
		shader2->compileProgram();
		shaders.push_back(shader2);

		//bool success = true;
		//shaders["NormShader"] = new QOpenGLShaderProgram();
		//// Compile vertex shader
		//if (!shaders["NormShader"]->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Engine/Shaders/vertexShader.glsl"))
		//	success = false;
		//// Compile fragment shader
		//if (!shaders["NormShader"]->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Engine/Shaders/fragShader.glsl"))
		//	success = false;
		//// Link shader pipeline
		//if (!shaders["NormShader"]->link())
		//	success = false;
		//// Bind shader pipeline for use
		//if (!shaders["NormShader"]->bind())
		//	success = false;

		//shaders["PtShader"] = new QOpenGLShaderProgram();
		//if (!shaders["PtShader"]->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Engine/Shaders/ptVertexShader.glsl"))
		//	success = false;
		//if (!shaders["PtShader"]->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Engine/Shaders/ptFragShader.glsl"))
		//	success = false;
		//if (!shaders["PtShader"]->link())
		//	success = false;
		//if (!shaders["PtShader"]->bind())
		//	success = false;

		//shaders["imageShader"] = new QOpenGLShaderProgram();
		//if (!shaders["imageShader"]->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Engine/Shaders/imageVertexShader.glsl"))
		//	success = false;
		//if (!shaders["imageShader"]->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Engine/Shaders/imageFragShader.glsl"))
		//	success = false;
		//if (!shaders["imageShader"]->link())
		//	success = false;
		//if (!shaders["imageShader"]->bind())
		//	success = false;
	}

	void deleteShaders()
	{
		for (unsigned int i = 0; i < shaders.size(); i++)
		{
			shaders[i]->release();
			delete shaders[i];
		}
	}
};