#include "TrackballCameraInteractor.h"
#include "TrackballCamera.h"

void TrackballCameraInteractor::init(glm::vec2 pos, int windowWidth, int windowHeight)
{
	// If the camera doesn't get initialized with the current mouse position 
	// there will be a big jump when first mouse move is called
	screenSize = glm::vec2(windowWidth, windowHeight);
	cam->initTrackballCamera(45.0f, static_cast<GLfloat>(windowWidth) / windowHeight, 0.1f, 10000.0f);
	prevMousePos = mousePos = pos / screenSize;
}

void TrackballCameraInteractor::mouseMove(glm::vec2 pos)
{
	// Push back the last mouse position
	prevMousePos = mousePos;
	// Then update it and normalize the mouse coordinates
	mousePos = pos / screenSize;

	glm::vec2 diff = mousePos - prevMousePos;
	if (leftButtonDown)
		cam->rotate(diff);
	if (middleButtonDown)
		cam->pan(diff);
}

void TrackballCameraInteractor::mouseScroll(GLfloat ds) { cam->zoom(-ds); }

void TrackballCameraInteractor::windowResize(int width, int height)
{
	screenSize = glm::vec2(width, height);
	cam->setPerspective(45.0f, static_cast<GLfloat>(width) / height, 0.1f, 10000.0f);
}