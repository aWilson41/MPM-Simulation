#pragma once
#include <vector>
#include <string>

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

protected:
	void createWindow(std::string windowName, int windowWidth, int windowHeight);

	static void errorCallback(int error, const char * description)
	{
		printf(("GLFW Error (" + std::to_string(error) + ") " + description + "\n").c_str());
	}

	static void windowResize(GLFWwindow* window, int width, int height);

protected:
	GLFWwindow* window = nullptr;
	Renderer* ren = nullptr;
	std::string windowName = "GLFW Window";
};