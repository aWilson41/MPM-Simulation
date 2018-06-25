#pragma once
#include "Engine\Camera.h"
#include "Engine\Primitives.h"
#include "Engine\Material.h"
#include "Engine\PointCloud.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QBasicTimer>
#include <QOpenGLShaderProgram>

class MainWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT
public:
	explicit MainWidget(QWidget* parent = 0);
	~MainWidget();

protected:
	void mousePressEvent(QMouseEvent* e) override;
	void mouseReleaseEvent(QMouseEvent* e) override;
	void wheelEvent(QWheelEvent* e) override;
	void mouseMoveEvent(QMouseEvent* e) override;
	void timerEvent(QTimerEvent* e) override;

	void initializeGL() override;
	void resizeGL(int w, int h) override;
	void paintGL() override;

	void initShaders();

protected:
	void updateCamera(glm::vec2 pos);
	
private:
	Camera cam;
	QBasicTimer timer;
	QOpenGLShaderProgram normShader;
	QOpenGLShaderProgram ptShader;
	Plane* plane = nullptr;
	PointCloud* ptCloud = nullptr;
	std::vector<Material*> materials;

	glm::vec3 lightDir = glm::normalize(glm::vec3(0.0f, 1.0f, 1.0f));

	glm::vec2 mousePos = glm::vec2(0.0f);
	GLfloat phi = 1.4f;
	GLfloat theta = 1.57f;
	GLfloat rho1 = 60.0f;
};