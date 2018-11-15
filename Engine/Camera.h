#pragma once
#include "MathHelper.h"
#include <QtOpenGL\qgl.h>

class Camera
{
public:
	Camera() { reset(); }

	void reset()
	{
		fov = 45.0f;
		eyePos = focalPt = glm::vec3(0.0f);
		up = glm::vec3(0.0f, 1.0f, 0.0f);
		view = glm::mat4(1.0f);

		// 1, 0, 0, 0
		// 0, 1, 0, 0
		// 0, 0, 1, 0
		// 0, 0, 1, 0 default proj matrix. Stuffs z in w to be used in z divide
		proj = glm::mat4(1.0f);
		proj[2][3] = 1.0f;
		proj[3][3] = 0.0f;
	}

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
	glm::mat4 view = I;
	// Camera projection matrix
	glm::mat4 proj = I;
	// Eye position
	glm::vec3 eyePos = glm::vec3(0.0f);;
	// Focal point
	glm::vec3 focalPt = glm::vec3(0.0f);;
	// Up
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	// Camera specs
	GLfloat fov = 45.0f;
	GLfloat aspectRatio;
	GLfloat nearZ;
	GLfloat farZ;
};