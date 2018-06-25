#include "MainWidget.h"
#include "Engine/ResourceLoader.h"

#include <QMouseEvent>
#include <QApplication>
#include <qdesktopwidget.h>
#include <math.h>

MainWidget::MainWidget(QWidget* parent) : 
	QOpenGLWidget(parent)
{
	QRect rect = QApplication::desktop()->availableGeometry();
	setGeometry(150, 100, rect.width() * 1440.0 / 1920.0, rect.height() * 810.0 / 1080.0);

	// Update the mouse position to the current mouse position
	QPoint mousePt = mapFromGlobal(QCursor::pos());
	mousePos = glm::vec2(static_cast<GLfloat>(mousePt.x()), static_cast<GLfloat>(mousePt.y()));
	setMouseTracking(true);
	updateCamera(mousePos);
}

void MainWidget::initializeGL()
{
	initializeOpenGLFunctions();
	glEnable(GL_PROGRAM_POINT_SIZE);

	glClearColor(0.4f, 0.58f, 0.9f, 1.0f);
	initShaders();

	srand(time(NULL));

	// Create some random points in a box
	std::vector<glm::vec3> pts = std::vector<glm::vec3>(1000);
	int range = 200;
	glm::vec3 translate = glm::vec3(0.0f, 50.0f, 0.0f);
	for (UINT i = 0; i < 1000; i++)
	{
		pts[i] = glm::vec3(rand() % range, rand() % range, rand() % range) - glm::vec3(range) * 0.5f + translate;
	}

	ptCloud = new PointCloud();
	ptCloud->setPoints(pts.data(), static_cast<UINT>(pts.size()));
	ptCloud->world = mathHelper::matrixScale(1.0f);
	ptCloud->setShaderProgram(&ptShader);

	plane = new Plane();
	plane->setShaderProgram(&normShader);
	plane->world = mathHelper::matrixTranslate(0.0f, -85.0f, 0.0f) * mathHelper::matrixScale(1000.0f);
	Material* planeMat = new Material();
	planeMat->setDiffuse(0.2f, 0.4f, 0.2f);
	planeMat->setAmbientToDiffuse(0.8f);
	materials.push_back(planeMat);
	plane->setMaterial(planeMat);

	// Enable depth buffer
	glEnable(GL_DEPTH_TEST);
	// Enable Multisampling
	glEnable(GL_MULTISAMPLE);
	// Enable back face cull
	glEnable(GL_CULL_FACE);

	// Use QBasicTimer because its faster than QTimer
	timer.start(12, this);
}

void MainWidget::initShaders()
{
	// Compile vertex shader
	if (!normShader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.glsl"))
		close();
	// Compile fragment shader
	if (!normShader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.glsl"))
		close();
	// Link shader pipeline
	if (!normShader.link())
		close();
	// Bind shader pipeline for use
	if (!normShader.bind())
		close();

	if (!ptShader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/ptvShader.glsl"))
		close();
	if (!ptShader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/ptfShader.glsl"))
		close();
	if (!ptShader.link())
		close();
	if (!ptShader.bind())
		close();
}

void MainWidget::mousePressEvent(QMouseEvent* e)
{

}

void MainWidget::mouseReleaseEvent(QMouseEvent* e)
{

}

void MainWidget::wheelEvent(QWheelEvent* e)
{
	rho1 -= e->delta() * 0.004f;
	updateCamera(mousePos);
}

void MainWidget::mouseMoveEvent(QMouseEvent* e)
{
	// Get the new mouse position as vec2
	glm::vec2 newMousePos = mathHelper::qPointToVec2(e->pos());
	if (e->buttons() == Qt::LeftButton)
		updateCamera(newMousePos);
	mousePos = newMousePos;
}

void MainWidget::updateCamera(glm::vec2 pos)
{
	glm::vec2 diff = (mousePos - pos) * 0.5f;
	theta -= diff.x * 0.008f;
	phi += diff.y * 0.008f;
	
	// Clamp
	if (phi > 3.14f)
		phi = 3.14f;
	else if (phi < 0.01f)
		phi = 0.01f;

	// Exponentially scale the scroll out
	GLfloat rho2 = std::pow(1.2, rho1) * 0.01f;
	// Convert spherical coords
	glm::vec3 eyePos = glm::vec3(
		rho2 * sin(phi) * cos(theta),
		rho2 * cos(phi),
		rho2 * sin(phi) * sin(theta));

	// Should prob use a slerp here but ohwell
	cam.setEyePos(eyePos);
	cam.setFocalPt(0.0f, 0.0f, 0.0f);
	cam.updateLookAt();
}

void MainWidget::timerEvent(QTimerEvent* e)
{
	update();
}


void MainWidget::resizeGL(int w, int h)
{
	// Calculate aspect ratio
	qreal aspect = qreal(w) / qreal(h ? h : 1);

	// Set near plane to 0.001, far plane to 700.0, field of view 45 degrees
	// Set the perspective projection matrix
	cam.setPerspective(45.0, aspect, 0.001f, 10000.0f);
	//cam.setOrtho(-1.0f, 1.0f, 1.0f, -1.0f, 0.001f, 10000.0f);
}

void MainWidget::paintGL()
{
	// Clear color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 viewProj = cam.proj * cam.view;

	normShader.bind();
	glUniform3f(normShader.uniformLocation("lightDir"), lightDir.x, lightDir.y, lightDir.z);
	plane->draw(viewProj);
	normShader.release();

	ptShader.bind();
	glUniform3f(ptShader.uniformLocation("lightDir"), lightDir.x, lightDir.y, lightDir.z);
	ptCloud->Draw(viewProj);
	ptShader.release();
}

MainWidget::~MainWidget()
{
	// Delete the materials
	for (UINT i = 0; i < materials.size(); i++)
	{
		if (materials[i] != nullptr)
			delete materials[i];
	}

	if (ptCloud != nullptr)
		delete ptCloud;
	if (plane != nullptr)
		delete plane;

	// Make sure the context is current when deleting the texture and the buffers.
	makeCurrent();
	doneCurrent();
}