#include "RenderWindow.h"
#include "MouseInteractor.h"
#include "Renderer.h"
#include <GLFW/glfw3.h>
#include <stdexcept>

RenderWindow::RenderWindow()
{
	glfwSetErrorCallback(glfwErrorCallback);

	if (!glfwInit())
		throw std::runtime_error("Failed to initialise GLFW");

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_DEPTH_BITS, 24);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	//glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

	const GLFWvidmode* vidMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	createWindow(windowName, static_cast<int>(vidMode->width * 0.85), static_cast<int>(vidMode->height * 0.85));

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_CLAMP);
	glEnable(GL_MULTISAMPLE);
	glDisable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
}

void RenderWindow::start()
{
	// Update loop
	while (isActive())
	{
		render();
	}
}

void RenderWindow::stop() { glfwTerminate(); }

void RenderWindow::render()
{
	if (window == nullptr || ren == nullptr)
		return;

	glClearColor(0.5f, 0.3f, 0.25f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ren->render();

	glfwSwapBuffers(window);
	glfwPollEvents();
}

bool RenderWindow::isActive() { return !glfwWindowShouldClose(window); }

void RenderWindow::setWindowName(std::string name)
{
	windowName = name;
	glfwSetWindowTitle(window, windowName.c_str());
}

void RenderWindow::setInteractor(MouseInteractor* interactor)
{
	RenderWindow::interactor = interactor;
	// Initialize interactor with mouse position
	double posX, posY;
	glfwGetCursorPos(window, &posX, &posY);
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	interactor->init(glm::vec2(posX, posY), width, height);
}

void RenderWindow::createWindow(std::string windowName, int windowWidth, int windowHeight)
{
	window = glfwCreateWindow(windowWidth, windowHeight, windowName.c_str(), nullptr, nullptr);

	if (!window)
	{
		glfwTerminate();
		throw std::runtime_error("Failed to create window with GLFW");
	}

	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetWindowSizeCallback(window, glfwWindowResize);
	glfwSetCursorPosCallback(window, glfwMouseMove);
	glfwSetMouseButtonCallback(window, glfwMouseButton);
	glfwSetScrollCallback(window, glfwScroll);
	glfwSetWindowUserPointer(window, this);
	glfwSwapInterval(0); // Turn off vsync

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		glfwTerminate();
		throw std::runtime_error("Failed to initialize GLEW");
	}
}

void RenderWindow::glfwWindowResize(GLFWwindow* window, int width, int height)
{
	RenderWindow* renWin = static_cast<RenderWindow*>(glfwGetWindowUserPointer(window));
	renWin->getInteractor()->windowResize(width, height);
}

void RenderWindow::glfwMouseMove(GLFWwindow* window, double posX, double posY)
{
	RenderWindow* renWin = static_cast<RenderWindow*>(glfwGetWindowUserPointer(window));
	renWin->getInteractor()->mouseMove(glm::vec2(posX, posY));
}

void RenderWindow::glfwMouseButton(GLFWwindow* window, int button, int action, int mods)
{
	RenderWindow* renWin = static_cast<RenderWindow*>(glfwGetWindowUserPointer(window));
	if (action == GLFW_PRESS)
		renWin->getInteractor()->mouseDown(button);
	else if (action == GLFW_RELEASE)
		renWin->getInteractor()->mouseUp(button);
}

void RenderWindow::glfwScroll(GLFWwindow* window, double xoffset, double yoffset)
{
	RenderWindow* renWin = static_cast<RenderWindow*>(glfwGetWindowUserPointer(window));
	renWin->getInteractor()->mouseScroll(static_cast<GLfloat>(yoffset));
}