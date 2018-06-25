#pragma once
#include "MathHelper.h"
#include <QtOpenGL\qgl.h>

class Camera
{
public:
	Camera()
	{
		reset();
	}

	void reset();

	void setPerspective(GLfloat fov, GLfloat aspectRatio, GLfloat nearZ, GLfloat farZ)
	{
		Camera::fov = fov;
		Camera::aspectRatio = aspectRatio;
		Camera::nearZ = nearZ;
		Camera::farZ = farZ;
		proj = glm::perspective(fov, aspectRatio, nearZ, farZ);
	}
	void setOrtho(GLfloat left, GLfloat right, GLfloat top, GLfloat bottom, GLfloat nearZ, GLfloat farZ)
	{
		Camera::nearZ = nearZ;
		Camera::farZ = farZ;
		proj = glm::ortho(left, right, top, bottom, nearZ, farZ);
	}
	void setEyePos(glm::vec3 pos) { eyePos = pos; }
	void setEyePos(GLfloat x, GLfloat y, GLfloat z) { setEyePos(glm::vec3(x, y, z)); }

	void setFocalPt(glm::vec3 pt) { focalPt = pt; }
	void setFocalPt(GLfloat x, GLfloat y, GLfloat z) { setFocalPt(glm::vec3(x, y, z)); }
	void updateLookAt() { view = glm::lookAt(eyePos, focalPt, up); }

public:
	// Camera view matrix
	glm::mat4 view;
	// Camera projection matrix
	glm::mat4 proj;
	// Eye position
	glm::vec3 eyePos;
	// Focal point
	glm::vec3 focalPt;
	// Up
	glm::vec3 up;
	// Camera specs
	GLfloat fov;
	GLfloat aspectRatio;
	GLfloat nearZ;
	GLfloat farZ;
};