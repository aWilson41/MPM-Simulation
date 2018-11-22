#pragma once
#include "Engine/MouseInteractor.h"
#include "Engine/TrackballCamera.h"

// Maps mouse values to camera
class TrackballCameraInteractor : public MouseInteractor
{
public:
	void setCamera(TrackballCamera* cam) { TrackballCameraInteractor::cam = cam; }

	void init(glm::vec2 mousePos, int windowWidth, int windowHeight) override
	{
		// If the camera doesn't get initialized with the current mouse position 
		// there will be a big jump when first mouse move is called
		MouseInteractor::init(mousePos, windowWidth, windowHeight);
		cam->initTrackballCamera(mousePos, 1.4f, 1.57f, 60.0f, 
			45.0f, static_cast<GLfloat>(windowWidth) / windowHeight, 0.001f, 10000.0f);
	}

	void mouseMove(glm::vec2 mousePos) override
	{
		glm::vec2 diff = prevMousePos - mousePos;
		if (leftButtonDown)
			cam->rotate(diff);
		if (rightButtonDown)
			cam->pan(diff);

		MouseInteractor::mouseMove(mousePos);
	}

	void mouseScroll(GLfloat ds) override { cam->zoom(ds); }

	void windowResize(int width, int height) override
	{
		cam->setPerspective(45.0f, static_cast<GLfloat>(width) / height, 0.001f, 10000.0f);
	}

protected:
	TrackballCamera* cam = nullptr;
};