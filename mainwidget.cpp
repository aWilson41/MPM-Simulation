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
	glClearColor(0.4f, 0.58f, 0.9f, 1.0f);
	initShaders();

	/*poly = resourceLoader::loadPolygon("C:/Users/Andx_/Desktop/Cube.obj");
	if (poly == nullptr)
	{
		QMessageBox::warning(this, tr("Error"), tr("Failed to load mesh."), QMessageBox::Ok);
		exit(1);
	}
	poly->setShaderProgram(&program);
	poly->world = mathHelper::matrixScale(0.2f);
	Material* polyMat = new Material();
	polyMat->setDiffuse(0.4f, 0.16f, 0.0f);
	polyMat->setAmbientToDiffuse(0.8f);
	materials.push_back(polyMat);
	poly->setMaterial(polyMat);*/

	softBody = resourceLoader::loadTetgenMesh("C:/Users/Andx_/Desktop/mesh/dragon.1", 8);
	if (softBody == nullptr)
	{
		QMessageBox::warning(this, tr("Error"), tr("Failed to load mesh."), QMessageBox::Ok);
		exit(1);
	}
	softBody->setShaderProgram(&program);
	Material* mat = new Material();
	mat->setDiffuse(0.4f, 0.16f, 0.0f);
	mat->setAmbientToDiffuse(0.8f);
	materials.push_back(mat);
	softBody->setMaterial(mat);
	softBody->calculateNormals();

	plane = new Plane();
	plane->setShaderProgram(&program);
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
	if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.glsl"))
		close();

	// Compile fragment shader
	if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.glsl"))
		close();

	// Link shader pipeline
	if (!program.link())
		close();

	// Bind shader pipeline for use
	if (!program.bind())
		close();
}

void MainWidget::mousePressEvent(QMouseEvent* e)
{
	//poly->data[0].pos.x += 100.0f;
	/*poly->vertexBuffer->bind();
	VertexData* vertices = static_cast<VertexData*>(poly->vertexBuffer->map(QOpenGLBuffer::Access::WriteOnly));
	vertices[0].pos.x += 100.0f;
	poly->vertexBuffer->unmap();
	poly->vertexBuffer->release();*/

	glm::vec2 pos = glm::vec2(e->localPos().x(), height() - e->localPos().y());
	glm::vec2 size = glm::vec2(width(), height()) / 2.0f;
	pos = (pos - size) / size;
	glm::vec4 pt = glm::vec4(pos.x, pos.y, 0.0f, 1.0f) * glm::inverse(cam.proj);
	glm::vec3 dir = glm::normalize(glm::vec3(pt) - cam.eyePos);
	//mathHelper::computeEyeRay(pos, width(), height(), cam.fov, cam.nearZ);

	for (int i = 0; i < softBody->faceCount; i++)
	{
		glm::vec3 diff1 = softBody->faceData[i].v3->pos - softBody->faceData[i].v2->pos;
		glm::vec3 diff2 = softBody->faceData[i].v3->pos - softBody->faceData[i].v1->pos;
		glm::vec3 n = glm::normalize(glm::cross(diff1, diff2));
		glm::vec4 results = mathHelper::triangleRayIntersection(mathHelper::Ray(glm::vec3(pt), dir), softBody->faceData[i].v1->pos, softBody->faceData[i].v2->pos, softBody->faceData[i].v3->pos, n);
		if (results.x == 0.0f && results.y == 0.0f && results.z == 0.0f)
			printf("Miss\n");
		else
			printf("Hit\n");
	}
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
	softBody->update(0.02f, -20.6f);
	update();
}


void MainWidget::resizeGL(int w, int h)
{
	// Calculate aspect ratio
	qreal aspect = qreal(w) / qreal(h ? h : 1);

	// Set near plane to 0.001, far plane to 700.0, field of view 45 degrees
	// Set the perspective projection matrix
	cam.setPerspective(45.0, aspect, 0.001f, 10000.0f);
}

void MainWidget::paintGL()
{
	// Clear color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	program.bind();

	// Set modelview-projection matrix
	glm::mat4 viewProj = cam.proj * cam.view;
	glUniform3f(program.uniformLocation("lightDir"), lightDir.x, lightDir.y, lightDir.z);
	//poly->draw(viewProj);
	softBody->draw(viewProj);
	plane->draw(viewProj);

	program.release();
}

MainWidget::~MainWidget()
{
	for (UINT i = 0; i < materials.size(); i++)
	{
		if (materials[i] != nullptr)
			delete materials[i];
	}

	if (poly != nullptr)
		delete poly;
	if (softBody != nullptr)
		delete softBody;
	if (plane != nullptr)
		delete plane;

	// Make sure the context is current when deleting the texture and the buffers.
	makeCurrent();
	doneCurrent();
}