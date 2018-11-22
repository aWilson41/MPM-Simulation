#pragma once
#include <vector>
#include <string>

class MouseInteractor;
class GLFWwindow;
class Renderer;

class RenderWindow
{
public:
	RenderWindow();
	~RenderWindow() { stop(); }

public:
	void stop();
	void render();
	bool isActive();

	void setRenderer(Renderer* ren) { RenderWindow::ren = ren; }
	void setWindowName(std::string name);
	void setInteractor(MouseInteractor* interactor);
	MouseInteractor* getInteractor() { return interactor; }

protected:
	void createWindow(std::string windowName, int windowWidth, int windowHeight);

	// GLFW callback
	static void glfwErrorCallback(int error, const char * description)
	{
		printf(("GLFW Error (" + std::to_string(error) + ") " + description + "\n").c_str());
	}
	static void glfwWindowResize(GLFWwindow* window, int width, int height);
	static void glfwMouseMove(GLFWwindow* window, double posX, double posY);
	static void glfwMouseButton(GLFWwindow* window, int button, int action, int mods);
	static void glfwScroll(GLFWwindow* window, double xoffset, double yoffset);

protected:
	GLFWwindow* window = nullptr;
	Renderer* ren = nullptr;
	MouseInteractor* interactor = nullptr;
	std::string windowName = "GLFW Window";
};