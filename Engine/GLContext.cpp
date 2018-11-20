#include "GLContext.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

GLContext::~GLContext() { glfwTerminate(); }

void GLContext::initGLFW()
{
	glfwSetErrorCallback(errorCallback);

	if (!glfwInit())
		throw std::runtime_error("Failed to initialise GLFW");

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
}

void GLContext::createWindow(std::string windowName, int windowWidth, int windowHeight)
{
	window = glfwCreateWindow(windowWidth, windowHeight, windowName.c_str(), nullptr, nullptr);

	if (!window)
	{
		glfwTerminate();
		throw std::runtime_error("Failed to create window with GLFW");
	}

	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetWindowSizeCallback(window, windowResize);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		glfwTerminate();
		throw std::runtime_error("Failed to initialize GLEW");
	}
}

bool GLContext::isActive() { return !glfwWindowShouldClose(window); }

void GLContext::loop()
{
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void GLContext::windowResize(GLFWwindow* window, int width, int height)
{
	printf("test\n");
}