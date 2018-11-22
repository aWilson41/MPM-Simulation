#pragma once
#include "Camera.h"

// A camera on a sphere with focal point in middle
// Supports zoom and pan as well
class TrackballCamera : public Camera
{
public:
	TrackballCamera() { reset(); }

	void initTrackballCamera(glm::vec2 pos, GLfloat phi, GLfloat theta, GLfloat rho, 
		GLfloat fov, GLfloat aspectRatio, GLfloat nearZ, GLfloat farZ)
	{
		TrackballCamera::phi = phi;
		TrackballCamera::theta = theta;
		TrackballCamera::rho = rho;
		setPerspective(fov, aspectRatio, nearZ, farZ);
		updateCam();
	}

	// Resets to defaults
	void reset() override
	{
		initTrackballCamera(glm::vec2(0.0f), 1.4f, 1.57f, 60.0f, 
			45.0f, 16.0f / 9.0f, 0.001f, 10000.0f);
	}

	// Maps 2d mouse coordinates to spherical coordinates
	void updateCam()
	{
		// Exponentially scale rho
		GLfloat r = std::pow(1.2f, rho) * 0.01f;
		// Calc eye position on sphere
		glm::vec3 eyePos = glm::vec3(
			r * sin(phi) * cos(theta),
			r * cos(phi),
			r * sin(phi) * sin(theta));

		setEyePos(eyePos + focalPt);
		setFocalPt(focalPt);
		updateLookAt();
	}

	// Input is a change in position like (prevMousePos - currentPos)
	void rotate(glm::vec2 diff)
	{
		theta += diff.x * rotateSpeed;
		phi -= diff.y * rotateSpeed;

		// Clamp
		phi = MathHelp::clamp(phi, 0.01f, 3.14f);
		updateCam();
	}

	// Maps 2d mouse coordinates to shifts
	void pan(glm::vec2 diff)
	{
		// Up and right are the directions to pan when camera is facing forward
		glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
		glm::mat3 invView = glm::inverse(view);
		glm::vec3 dx = invView * right;
		glm::vec3 dy = invView * up;

		// Scale shift with scale so shift is relative to how far you are zoomed out
		GLfloat r = std::pow(1.2f, rho);
		focalPt += (dy * diff.y - dx * diff.x) * r * shiftSpeed;
		updateCam();
	}

	void zoom(GLfloat diff)
	{
		rho += diff * zoomSpeed;
		updateCam();
	}

	void setPhi(GLfloat phi) { TrackballCamera::phi = phi; }
	void setTheta(GLfloat theta) { TrackballCamera::theta = theta; }
	void setRho(GLfloat rho) { TrackballCamera::rho = rho; }
	void setRotateSpeed(GLfloat speed) { rotateSpeed = speed; }
	void setShiftSpeed(GLfloat speed) { shiftSpeed = speed; }

	GLfloat getPhi() { return phi; }
	GLfloat getTheta() { return theta; }
	GLfloat getRho() { return rho; }
	GLfloat getRotateSpeed() { rotateSpeed; }
	GLfloat getShiftSpeed() { shiftSpeed; }

protected:
	GLfloat phi = 1.4f;
	GLfloat theta = 1.57f;
	GLfloat rho = 60.0f;

	GLfloat rotateSpeed = 0.008f;
	GLfloat shiftSpeed = 0.000025f;
	GLfloat zoomSpeed = 1.0f;
};