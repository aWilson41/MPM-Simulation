#pragma once
#include <GL/glew.h>
#include <fstream>
#include <sstream>

class ShaderProgram
{
public:
	ShaderProgram(std::string name)
	{
		shaderName = name;
	}

public:
	std::string getName() { return shaderName; }

	std::string readShaderFile(std::string filePath)
	{
		// Read the shader
		std::ifstream file(filePath.c_str());
		if (file.fail())
		{
			printf(("Failed to open: " + filePath + "\n").c_str());
			return "";
		}
		std::stringstream buffer;
		buffer << file.rdbuf();
		return buffer.str();
	}

	void loadVertexShader(std::string filePath)
	{
		vertexFileName = filePath;
		vertexShaderSrc = readShaderFile(filePath);
	}
	void loadFragmentShader(std::string filePath)
	{
		fragFileName = filePath;
		fragShaderSrc = readShaderFile(filePath);
	}
	void compileShader(const char* src, GLuint shaderID, std::string filePath)
	{
		glShaderSource(shaderID, 1, &src, NULL);
		glCompileShader(shaderID);
		GLint compile_ok = GL_FALSE;
		glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compile_ok);
		if (!compile_ok)
		{
			printf(("Shader error in: " + filePath + "\n").c_str());
			printShaderError(shaderID);
			glDeleteShader(shaderID);
		}
		/*else
			printf((filePath + " compiled OK.\n").c_str());*/
	}
	void compileProgram()
	{
		fragShaderID = glCreateShader(GL_FRAGMENT_SHADER);
		compileShader(fragShaderSrc.c_str(), fragShaderID, fragFileName);

		vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		compileShader(vertexShaderSrc.c_str(), vertexShaderID, vertexFileName);

		programID = glCreateProgram();
		glAttachShader(programID, vertexShaderID);
		glAttachShader(programID, fragShaderID);
		glLinkProgram(programID);

		glDeleteShader(vertexShaderID);
		glDeleteShader(fragShaderID);

		GLint link_ok = GL_FALSE;
		glGetProgramiv(programID, GL_LINK_STATUS, &link_ok);
		if (!link_ok)
		{
			printf("Shader link error: ");
			printLinkerError(programID);
			glDeleteProgram(programID);
		}
		//else
		//	printf("Shader linked OK.\n");
	}

	void printShaderError(GLuint shaderID)
	{
		GLint logSize = 0;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logSize);
		GLchar* data = new GLchar[logSize];
		glGetShaderInfoLog(shaderID, logSize, &logSize, data);
		printf(data);
		printf("\n");
		delete[] data;
		
	}
	void printLinkerError(GLuint programID)
	{
		GLint logSize = 0;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logSize);
		GLchar* data = new GLchar[logSize];
		glGetProgramInfoLog(programID, logSize, &logSize, data);
		printf(data);
		printf("\n");
		delete[] data;
	}

	void release()
	{
		glDetachShader(programID, vertexShaderID);
		glDetachShader(programID, fragShaderID);
		glDeleteShader(vertexShaderID);
		glDeleteShader(fragShaderID);
		glDeleteProgram(programID);
	}

	GLuint getProgramID() { return programID; }

protected:
	std::string shaderName = "none";
	GLuint programID = -1;
	GLuint vertexShaderID = -1;
	GLuint fragShaderID = -1;

	std::string vertexFileName = "";
	std::string fragFileName = "";
	std::string vertexShaderSrc = "";
	std::string fragShaderSrc = "";
};