#pragma once
#include <string>

class GLFWwindow;

class GLContext
{
public:
	~GLContext();
	
	void initGLFW();
	void createWindow(std::string windowName, int windowWidth, int windowHeight);
	bool isActive();
	void loop();

	static void errorCallback(int error, const char * description)
	{
		printf(("GLFW Error (" + std::to_string(error) + ") " + description + "\n").c_str());
	}

	static void windowResize(GLFWwindow* window, int width, int height);

protected:
	GLFWwindow* window = nullptr;
};