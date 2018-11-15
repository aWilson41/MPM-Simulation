#pragma once
#include "Camera.h"

// A camera on a sphere with focal point in middle
// Supports zoom and pan as well
class TrackballCamera : public Camera
{
public:
	TrackballCamera()
	{
		prevPos = glm::vec2(0.0f);
		reset();
	}

	void init(glm::vec2 pos)
	{
		setPos(pos);
		rotate(pos);
		updateCam();
	}

	void setPos(glm::vec2 pos) { prevPos = pos; }

	// Maps 2d mouse coordinates to spherical coordinates
	void updateCam()
	{
		// Exponentially scale the scroll out
		GLfloat r = std::pow(1.2, rho) * 0.01f;
		// Convert spherical coords
		glm::vec3 eyePos = glm::vec3(
			r * sin(phi) * cos(theta),
			r * cos(phi),
			r * sin(phi) * sin(theta));

		// Should prob use a slerp here but ohwell
		setEyePos(eyePos + focalPt);
		setFocalPt(focalPt);
		updateLookAt();
	}

	void rotate(glm::vec2 pos)
	{
		glm::vec2 diff = (pos - prevPos) * 0.5f;
		theta += diff.x * 0.008f;
		phi -= diff.y * 0.008f;

		// Clamp
		phi = MathHelp::clamp(phi, 0.01f, 3.14f);
		updateCam();
	}

	// Maps 2d mouse coordinates to shifts
	void pan(glm::vec2 pos)
	{
		glm::vec2 diff = (pos - prevPos) * 0.5f;

		// Up and right are the directions to pan when camera is facing forward
		glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
		// So we acquire rotated versions of them
		glm::mat3 invView = glm::inverse(view);
		glm::vec3 dx = invView * right;
		glm::vec3 dy = invView * up;

		// Scale shift with scale so shift is relative to how far you are zoomed out
		GLfloat r = std::pow(1.2, rho) * 0.000025f;
		focalPt += (dy * diff.y - dx * diff.x) * r;
		updateCam();
	}

	void setPhi(GLfloat phi) { TrackballCamera::phi = phi; }
	void setTheta(GLfloat theta) { TrackballCamera::theta = theta; }
	void setRho(GLfloat rho) { TrackballCamera::rho = rho; }

	glm::vec2 getPrevPos() { return prevPos; }
	GLfloat getPhi() { return phi; }
	GLfloat getTheta() { return theta; }
	GLfloat getRho() { return rho; }

protected:
	glm::vec2 prevPos;

	GLfloat phi = 1.4f;
	GLfloat theta = 1.57f;
	GLfloat rho = 60.0f;
};