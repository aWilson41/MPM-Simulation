#include "mainwidget.h"
#include "Engine/Geometry.h"
#include "Engine/Material.h"
#include "Engine/PointCloud.h"
#include "Engine/Primitives.h"
#include "Engine/ResourceLoader.h"
#include "MPMGrid.h"
#include "Particle.h"

#include <gl/GL.h>
#include <math.h>
#include <QApplication>
#include <qdesktopwidget.h>
#include <QMouseEvent>

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


	/*glm::mat2 source;
	source[0][0] = 42;
	source[0][1] = 2;
	source[1][0] = 104;
	source[1][1] = 23;
	MathHelp::printMat(source);
	printf("\n");

	glm::mat2 u, v;
	glm::vec2 s;
	MathHelp::svd(glm::transpose(source), &u, &s, &v);

	MathHelp::printMat(u);
	printf("\n");
	MathHelp::printVec(s);
	printf("\n");
	MathHelp::printMat(v);
	printf("\n");
	glm::mat2 st = I;
	st[0][0] = s[0];
	st[1][1] = s[1];
	MathHelp::printMat(u * st * v);*/
}

void MainWidget::initializeGL()
{
	initializeOpenGLFunctions();
	glEnable(GL_PROGRAM_POINT_SIZE);
	glClearColor(0.4f, 0.58f, 0.9f, 1.0f);
	initShaders();

#pragma region Create Snow Shape and Point Cloud
	// Create a 2d polygon to randomly distribute points/particles in
	geom::Poly circlePoly;
	circlePoly.FromCircle(geom::Circle(0.0f, 50.0f, 100.0f), 25);
	GLfloat circlePolyArea = circlePoly.area();
	printf("Polygon Area:   %f\n", circlePolyArea);
	GLfloat particleArea = PARTICLE_DIAMETER * PARTICLE_DIAMETER;
	printf("Particle Area:  %f\n", particleArea);
	UINT particleCount = circlePolyArea / particleArea;
	printf("Particle Count: %d\n", particleCount);
	// Create a random distribute of points in this shape
	std::vector<glm::vec2> results = MathHelp::generatePointCloud(&circlePoly, particleCount);
	std::vector<glm::vec3> pts = std::vector<glm::vec3>(results.size());
	// Convert to vec3
	for (UINT i = 0; i < results.size(); i++)
	{
		pts[i] = glm::vec3(results[i], 0.0f);
	}

	// Create the point cloud object for rendering
	ptCloud = new PointCloud();
	ptCloud->setPoints(pts.data(), static_cast<UINT>(pts.size()));
	ptCloud->world = MathHelp::matrixScale(1.0f);
	ptCloud->setShaderProgram(&ptShader);
#pragma endregion

#pragma region Init Particles
	// Create particles from the points, particles hold position pointers to avoid
	// copying them during rendering
	for (UINT i = 0; i < ptCloud->pts.size(); i++)
	{
		Particle particle(&ptCloud->pts[i], PARTICLE_MASS);
		particles.push_back(particle);
	}

	// Create the MPM grid
	geom::Rect bounds = MathHelp::getBounds(results.data(), static_cast<UINT>(results.size()));
	mpmGrid = new MPMGrid();
	glm::vec2 paddedBounds = bounds.size() * 3.0f; // 2x the size of the init bounds
	mpmGrid->initGrid(bounds.pos - (paddedBounds - bounds.size()) / 2.0f, paddedBounds, 64, 64);
	mpmGrid->initParticles(particles.data(), static_cast<UINT>(particles.size()));
#pragma endregion

	plane = new Plane();
	plane->setShaderProgram(&normShader);
	plane->world = MathHelp::matrixTranslate(0.0f, -85.0f, 0.0f) * MathHelp::matrixScale(1000.0f);
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
	glm::vec2 newMousePos = MathHelp::qPointToVec2(e->pos());
	if (e->buttons() == Qt::LeftButton)
		updateCamera(newMousePos);
	mousePos = newMousePos;
}

// Camera is updated on a sphere (currently just lerped)
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
	// constant timestep of 12ms
	//float dt = 0.001f;
	mpmGrid->projectToGrid();
	mpmGrid->update(1.0f);
	printf("Iteration Complete\n");

	ptCloud->updateBuffer();
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
	ptCloud->draw(viewProj);
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