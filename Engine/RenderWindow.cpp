#include "RenderWindow.h"
#include "MouseInteractor.h"
#include "Renderer.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

RenderWindow::RenderWindow()
{
	glfwSetErrorCallback(glfwErrorCallback);

	if (!glfwInit())
		throw std::runtime_error("Failed to initialise GLFW");

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_DEPTH_BITS, 16);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	//glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

	const GLFWvidmode* vidMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	createWindow(windowName, static_cast<int>(vidMode->width * 0.85), static_cast<int>(vidMode->height * 0.85));

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glDisable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);
}

void RenderWindow::stop() { glfwTerminate(); }

void RenderWindow::render()
{
	if (window == nullptr || ren == nullptr)
		return;

	glClearColor(0.5f, 0.3f, 0.25f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ren->render();

	glfwSwapBuffers(window);
	glfwPollEvents();
}

bool RenderWindow::isActive() { return !glfwWindowShouldClose(window); }

void RenderWindow::setWindowName(std::string name)
{
	windowName = name;
	glfwSetWindowTitle(window, windowName.c_str());
}

void RenderWindow::setInteractor(MouseInteractor* interactor)
{
	RenderWindow::interactor = interactor;
	// Initialize interactor with mouse position
	double posX, posY;
	glfwGetCursorPos(window, &posX, &posY);
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	interactor->init(glm::vec2(posX, posY), width, height);
}

void RenderWindow::createWindow(std::string windowName, int windowWidth, int windowHeight)
{
	window = glfwCreateWindow(windowWidth, windowHeight, windowName.c_str(), nullptr, nullptr);

	if (!window)
	{
		glfwTerminate();
		throw std::runtime_error("Failed to create window with GLFW");
	}

	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetWindowSizeCallback(window, glfwWindowResize);
	glfwSetCursorPosCallback(window, glfwMouseMove);
	glfwSetMouseButtonCallback(window, glfwMouseButton);
	glfwSetScrollCallback(window, glfwScroll);
	glfwSetWindowUserPointer(window, this);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		glfwTerminate();
		throw std::runtime_error("Failed to initialize GLEW");
	}
}

void RenderWindow::glfwWindowResize(GLFWwindow* window, int width, int height)
{
	RenderWindow* renWin = static_cast<RenderWindow*>(glfwGetWindowUserPointer(window));
	renWin->getInteractor()->windowResize(width, height);
}

void RenderWindow::glfwMouseMove(GLFWwindow* window, double posX, double posY)
{
	RenderWindow* renWin = static_cast<RenderWindow*>(glfwGetWindowUserPointer(window));
	renWin->getInteractor()->mouseMove(glm::vec2(posX, posY));
}

void RenderWindow::glfwMouseButton(GLFWwindow* window, int button, int action, int mods)
{
	RenderWindow* renWin = static_cast<RenderWindow*>(glfwGetWindowUserPointer(window));
	if (action == GLFW_PRESS)
		renWin->getInteractor()->mouseDown(button);
	else if (action == GLFW_RELEASE)
		renWin->getInteractor()->mouseUp(button);
}

void RenderWindow::glfwScroll(GLFWwindow* window, double xoffset, double yoffset)
{
	RenderWindow* renWin = static_cast<RenderWindow*>(glfwGetWindowUserPointer(window));
	renWin->getInteractor()->mouseScroll(yoffset);
}
//
//void App::initializeGL()
//{
//	glEnable(GL_PROGRAM_POINT_SIZE);
//	glClearColor(0.4f, 0.58f, 0.9f, 1.0f);
//	//Shaders::initShaders();
////
////#pragma region Create Snow Shape and Point Cloud
////	// Create a 2d polygon to randomly distribute points/particles in
////	geom::Poly circlePoly;
////	circlePoly.FromCircle(geom::Circle(0.0f, 115.0f, 100.0f), 25);
////	GLfloat circlePolyArea = circlePoly.area();
////	printf("Polygon Area:   %f\n", circlePolyArea);
////	GLfloat particleArea = PARTICLE_DIAMETER * PARTICLE_DIAMETER;
////	printf("Particle Area:  %f\n", particleArea);
////	UINT particleCount = circlePolyArea / particleArea;
////	printf("Particle Count: %d\n", particleCount);
////	// Create a random distribution of points in this shape
////	std::vector<glm::vec2> results = MathHelp::generatePointCloud(&circlePoly, particleCount);
////	std::vector<glm::vec3> pts = std::vector<glm::vec3>(results.size());
////	// Convert to vec3
////	for (UINT i = 0; i < results.size(); i++)
////	{
////		pts[i] = glm::vec3(results[i], 0.0f);
////	}
////
////	// Create the point cloud object for rendering
////	ptCloud = new PointCloud();
////	ptCloud->setPoints(pts.data(), static_cast<UINT>(pts.size()));
////	ptCloud->world = MathHelp::matrixScale(1.0f);
////	ptCloud->setShaderProgram(Shaders::shaders["ptShader"]);
////#pragma endregion
////
////#pragma region Init Particles
////	// Create particles from the points, particles hold position pointers to avoid
////	// copying them during rendering
////	for (UINT i = 0; i < ptCloud->pts.size(); i++)
////	{
////		Particle particle(&ptCloud->pts[i], PARTICLE_MASS);
////		particle.bulk = BULK_MODULUS;
////		particle.shear = SHEAR_MODULUS;
////		particles.push_back(particle);
////	}
////
////	// Create the MPM grid
////	geom::Rect bounds = MathHelp::getBounds(results.data(), static_cast<UINT>(results.size()));
////	mpmGrid = new MPMGrid();
////	GLfloat padScale = 4.0f;
////	glm::vec2 paddedSize = bounds.size() * padScale;
////	glm::vec2 origin = bounds.pos - paddedSize * 0.5f;
////	mpmGrid->initGrid(origin, paddedSize, 32, 32);
////	mpmGrid->initParticles(particles.data(), static_cast<UINT>(particles.size()));
////#pragma endregion
////
////	// Ground plane
////	plane = new Plane();
////	plane->setShaderProgram(Shaders::shaders["normShader"]);
////	plane->world = MathHelp::matrixTranslate(0.0f, -85.0f, 0.0f) * MathHelp::matrixScale(1000.0f);
////	Material* planeMat = new Material();
////	planeMat->setDiffuse(0.2f, 0.4f, 0.2f);
////	planeMat->setAmbientToDiffuse(0.8f);
////	materials.push_back(planeMat);
////	plane->setMaterial(planeMat);
////
////	// Bounds plane
////	boundsPlane = new Plane();
////	boundsPlane->setShaderProgram(Shaders::shaders["normShaders"]);
////	boundsPlane->world = MathHelp::matrixTranslate(bounds.pos.x, bounds.pos.y, 0.0f) * MathHelp::matrixScale(bounds.extent.x * 2.0f * padScale) * MathHelp::matrixRotateX(-PI * 0.5f);
////	Material* blackMat = new Material();
////	blackMat->setDiffuse(0.0f, 0.0f, 0.0f);
////	materials.push_back(blackMat);
////	boundsPlane->setMaterial(blackMat);
////
//	// Enable depth buffer
//	glEnable(GL_DEPTH_TEST);
//	// Enable Multisampling
//	glEnable(GL_MULTISAMPLE);
//	// Enable back face cull
//	glEnable(GL_CULL_FACE);
//
//	// Use QBasicTimer because its faster than QTimer
//	timer.start(1, this);
//}
//
//void App::mousePressEvent(QMouseEvent* e) { }
//
//void App::mouseReleaseEvent(QMouseEvent* e) { }
//
//void App::keyPressEvent(QKeyEvent* e)
//{
//	if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
//		running = !running;
//
//	// Simulate one step
//	if (e->key() == Qt::Key_Space)
//	{
//		simulate();
//		ptCloud->updateBuffer();
//		update();
//	}
//}
//
//void App::wheelEvent(QWheelEvent* e)
//{
//	cam.setRho(cam.getRho() - e->delta() * 0.004f);
//	cam.rotate(cam.getPrevPos());
//	update();
//}
//
//void App::mouseMoveEvent(QMouseEvent* e)
//{
//	// Get the new mouse position as vec2
//	glm::vec2 newMousePos = MathHelp::qPointToVec2(e->pos());
//	if (e->buttons() == Qt::LeftButton)
//	{
//		cam.rotate(newMousePos);
//		update();
//	}
//	else if (e->buttons() == Qt::MiddleButton)
//	{
//		cam.pan(newMousePos);
//		update();
//	}
//	cam.setPos(newMousePos);
//}
//
//void App::timerEvent(QTimerEvent* e)
//{
//	if (!running)
//		return;
//	simulate();
//	ptCloud->updateBuffer();
//	update();
//}
//
//void App::simulate()
//{
//	// constant timestep of 12ms
//	mpmGrid->projectToGrid();
//	mpmGrid->update(TIMESTEP);
//	iter++;
//	printIterationStats();
//}
//
//void App::printIterationStats()
//{
//	printf("\nITERATION: %d\n", iter);
//
//	printf("Max Particle Deformation Gradient Det: %.*f\n", 10, mpmGrid->maxParticleDefDet);
//	printf("Max Particle Deformation Gradient: \n");
//	MathHelp::printMat(mpmGrid->maxParticleDef);
//	printf("Max Particle Velocity Gradient Det:    %.*f\n", 10, mpmGrid->maxParticleVGDet);
//	printf("Max Particle Velocity Gradient: \n");
//	MathHelp::printMat(mpmGrid->maxParticleVG);
//	printf("Max Particle Energy Derivative Det:    %.*f\n", 10, mpmGrid->maxParticleEnergyDerivativeDet);
//	printf("Max Particle Energy Derivative: \n");
//	MathHelp::printMat(mpmGrid->maxParticleEnergyDerivative);
//
//	printf("Max Deformation Det:                   %.*f\n", 10, mpmGrid->maxParticleDefDet);
//	printf("Max Particle Velocity:                 %.*f\n", 10, mpmGrid->maxParticleVelocity);
//
//	printf("Max Node Velocity:                     %.*f\n", 10, mpmGrid->maxNodeVelocity);
//	printf("Max Node Force                         %.*f\n", 10, mpmGrid->maxNodeF);
//}
//
//
//void App::resizeGL(int w, int h)
//{
//	// Calculate aspect ratio
//	qreal aspect = qreal(w) / qreal(h ? h : 1);
//
//	// Set near plane to 0.001, far plane to 700.0, field of view 45 degrees
//	// Set the perspective projection matrix
//	cam.setPerspective(45.0, aspect, 0.001f, 10000.0f);
//	//cam.setOrtho(-1.0f, 1.0f, 1.0f, -1.0f, 0.001f, 10000.0f);
//}
//
//void App::paintGL()
//{
//	// Clear color and depth buffer
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glm::mat4 viewProj = cam.proj * cam.view;
//
//	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//	//Shaders::shaders["normShader"]->bind();
//	//glUniform3f(Shaders::shaders["normShader"]->uniformLocation("lightDir"), lightDir.x, lightDir.y, lightDir.z);
//	////plane->draw(viewProj);
//	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//	//boundsPlane->draw(viewProj);
//	//Shaders::shaders["normShader"]->release();
//
//	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//	//Shaders::shaders["ptShader"]->bind();
//	//glUniform3f(Shaders::shaders["ptShader"]->uniformLocation("lightDir"), lightDir.x, lightDir.y, lightDir.z);
//	//ptCloud->draw(viewProj);
//	//Shaders::shaders["ptShader"]->release();
//}
//
//App::~App()
//{
//	// Delete the materials
//	for (UINT i = 0; i < materials.size(); i++)
//	{
//		if (materials[i] != nullptr)
//			delete materials[i];
//	}
//
//	if (ptCloud != nullptr)
//		delete ptCloud;
//	if (plane != nullptr)
//		delete plane;
//	if (boundsPlane != nullptr)
//		delete boundsPlane;
//
//	// Make sure the context is current when deleting the texture and the buffers.
//	makeCurrent();
//	doneCurrent();
//}