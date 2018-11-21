#include "Engine/RenderWindow.h"
#include "Engine/Renderer.h"
#include "Engine/Primitives.h"
#include "Engine/PolyDataMapper.h"
#include "Engine/Shaders.h"

int main(int argc, char *argv[])
{
	// Create the window and give it the renderer
	// The glfw window has to be created to get the opengl context to make gl calls. This isn't something I can change using glfw.
	RenderWindow renWindow;
	renWindow.setWindowName("MPM Simulation");

	// Create the renderer
	Renderer ren;
	renWindow.setRenderer(&ren);

	// Add stuff to the renderer
	// Setup a plane
	PlaneSource plane;
	plane.update();

	PolyDataMapper mapper;
	mapper.setShaderProgram(Shaders::getShader("Norm Shader"));
	mapper.setInput(plane.getOutput());
	mapper.update();
	ren.addRenderItem(&mapper);

	// Update loop
	while (renWindow.isActive())
	{
		// Do stuff

		renWindow.render();
	}

	return 1;
}