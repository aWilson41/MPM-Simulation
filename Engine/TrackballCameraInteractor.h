#pragma once
#include "Engine/MouseInteractor.h"
#include "Engine/TrackballCamera.h"

// Maps mouse values to camera
class TrackballCameraInteractor : public MouseInteractor
{
public:
	void setCamera(TrackballCamera* cam) { TrackballCameraInteractor::cam = cam; }

	void init(glm::vec2 pos, int windowWidth, int windowHeight) override
	{
		// If the camera doesn't get initialized with the current mouse position 
		// there will be a big jump when first mouse move is called
		screenSize = glm::vec2(windowWidth, windowHeight);
		cam->initTrackballCamera(1.4f, 1.57f, 35.0f, 
			45.0f, static_cast<GLfloat>(windowWidth) / windowHeight, 0.1f, 10000.0f);
		prevMousePos = mousePos = pos / screenSize;
	}

	void mouseMove(glm::vec2 pos) override
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

	void mouseScroll(GLfloat ds) override { cam->zoom(-ds); }

	void windowResize(int width, int height) override
	{
		screenSize = glm::vec2(width, height);
		cam->setPerspective(45.0f, static_cast<GLfloat>(width) / height, 0.1f, 10000.0f);
	}

protected:
	TrackballCamera* cam = nullptr;
	glm::vec2 screenSize = glm::vec2(1.0f);
};