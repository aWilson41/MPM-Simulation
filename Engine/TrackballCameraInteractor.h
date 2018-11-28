#pragma once
#include "MouseInteractor.h"

class TrackballCamera;

// Maps mouse values to camera
class TrackballCameraInteractor : public MouseInteractor
{
public:
	void setCamera(TrackballCamera* cam) { TrackballCameraInteractor::cam = cam; }

	void init(glm::vec2 pos, int windowWidth, int windowHeight) override;

	void mouseMove(glm::vec2 pos) override;
	void mouseScroll(GLfloat ds) override;
	void windowResize(int width, int height) override;

protected:
	TrackballCamera* cam = nullptr;
	glm::vec2 screenSize = glm::vec2(1.0f);
};