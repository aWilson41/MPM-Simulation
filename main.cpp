#include "Engine/ImageMapper.h"
#include "Engine/Material.h"
#include "Engine/PNGReader.h"
#include "Engine/PolyDataMapper.h"
#include "Engine/Primitives.h"
#include "Engine/Renderer.h"
#include "Engine/RenderWindow.h"
#include "Engine/TrackballCameraInteractor.h"
//#include <chrono>

int main(int argc, char *argv[])
{
	// Create the window and give it the renderer
	// This has to happen before any gl calls in other objects because 
	// glfw can only make the opengl context when creating the window.
	RenderWindow renWindow;
	renWindow.setWindowName("MPM Simulation");

	// Create the camera for the renderer to use
	TrackballCamera cam;

	// Create the renderer
	Renderer ren;
	ren.setCamera(&cam);
	ren.addMaterial(new Material(glm::vec3(0.2f, 0.4f, 0.2f), 0.5f));
	renWindow.setRenderer(&ren);

	// Setup the camera and camera interactor
	TrackballCameraInteractor iren;
	iren.setCamera(&cam);
	renWindow.setInteractor(&iren);

	// Add stuff to the renderer
	// Setup a plane
	PlaneSource plane;
	plane.update();

	PolyDataMapper mapper;
	mapper.setInput(plane.getOutput());
	mapper.setMaterial(ren.getMaterial(0));
	mapper.setModelMatrix(MathHelp::matrixScale(500.0f));
	mapper.update();
	ren.addRenderItem(&mapper);

	PNGReader reader;
	reader.setFileName("C:/Users/Andx_/Desktop/test.png");
	reader.update();

	ImageMapper imageMapper;
	imageMapper.setInput(reader.getOutput());
	imageMapper.update();
	ren.addRenderItem(&imageMapper);
	 
	PNGReader reader1;
	reader1.setFileName("C:/Users/Andx_/Desktop/awesomeface.png");
	reader1.update();
	ImageMapper imageMapper1;
	imageMapper1.setInput(reader1.getOutput());
	imageMapper1.setModelMatrix(MathHelp::matrixTranslate(0.0f, 0.0f, 10.0f));
	imageMapper1.update();
	ren.addRenderItem(&imageMapper1);

	// Update loop
	while (renWindow.isActive())
	{
		//auto start = std::chrono::steady_clock::now();

		renWindow.render();

		//auto end = std::chrono::steady_clock::now();
		//// In seconds
		//double frameTime = std::chrono::duration<double, std::milli>(end - start).count() / 1000.0;
		//printf("Frame Time: %fs\n", frameTime);
		//printf("FPS: %f\n", 1.0 / frameTime);
	}

	return 1;
}