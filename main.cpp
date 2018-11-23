#include "Engine/Material.h"
#include "Engine/PolyDataMapper.h"
#include "Engine/Primitives.h"
#include "Engine/Renderer.h"
#include "Engine/RenderWindow.h"
#include "Engine/Shaders.h"
#include "Engine/TrackballCameraInteractor.h"
#include "Engine/PNGReader.h"
#include "Engine/ImageMapper.h"

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
	mapper.setModelMatrix(MathHelp::matrixTranslate(0.0f, -1.0f, 0.0f) * MathHelp::matrixScale(20.0f));
	mapper.update();
	ren.addRenderItem(&mapper);

	PNGReader reader;
	reader.setFileName("C:/Users/Andx_/Desktop/test.png");
	reader.update();

	ImageMapper imageMapper;
	imageMapper.setInput(reader.getOutput());
	imageMapper.setModelMatrix(MathHelp::matrixScale(10.0f));
	imageMapper.update();
	ren.addRenderItem(&imageMapper);

	renWindow.start();

	return 1;
}