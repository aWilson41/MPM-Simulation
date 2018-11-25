#include "Constants.h"
#include "Engine/Geometry2D.h"
#include "Engine/ImageMapper.h"
#include "Engine/Material.h"
#include "Engine/PlaneSource.h"
#include "Engine/PNGReader.h"
#include "Engine/PolyData.h"
#include "Engine/PolyDataMapper.h"
#include "Engine/Renderer.h"
#include "Engine/RenderWindow.h"
#include "Engine/TrackballCameraInteractor.h"
#include "MPMGrid.h"
#include "Particle.h"
//#include <chrono>

void printIterationStats(MPMGrid* mpmGrid, UINT iter);

int main(int argc, char *argv[])
{
	// Create the window and give it the renderer
	// This has to happen before any gl calls in other objects because 
	// glfw can only make the opengl context when creating the window.
	RenderWindow renWindow;
	renWindow.setWindowName("MPM Simulation");

	// Create the camera for the renderer to use
	TrackballCamera cam;

	// Create the renderer
	Renderer ren;
	ren.setCamera(&cam);
	ren.addMaterial(new Material(glm::vec3(0.2f, 0.4f, 0.2f), 0.5f));
	ren.addMaterial(new Material(glm::vec3(0.8f, 0.8f, 0.8f), 1.0f));
	renWindow.setRenderer(&ren);

	// Setup the camera and camera interactor
	TrackballCameraInteractor iren;
	iren.setCamera(&cam);
	renWindow.setInteractor(&iren);

	// Setup a ground plane
	PlaneSource plane;
	plane.update();

	PolyDataMapper planeMapper;
	planeMapper.setInput(plane.getOutput());
	planeMapper.setMaterial(ren.getMaterial(0));
	planeMapper.setModelMatrix(MathHelp::matrixScale(500.0f));
	planeMapper.update();
	ren.addRenderItem(&planeMapper);

#pragma region Generate Particles and Grid
	// Generate a 2d poly from a circle
	geom2d::Poly circlePoly;
	circlePoly.FromCircle(geom2d::Circle(0.0f, 115.0f, 100.0f), 25);
	GLfloat circlePolyArea = circlePoly.area();
	printf("Polygon Area:   %f\n", circlePolyArea);
	GLfloat particleArea = PARTICLE_DIAMETER * PARTICLE_DIAMETER;
	printf("Particle Area:  %f\n", particleArea);
	UINT particleCount = static_cast<UINT>(circlePolyArea / particleArea);
	printf("Particle Count: %d\n", particleCount);
	// Create a random distribution of points in this shape
	std::vector<glm::vec3> results = MathHelp::generatePointCloud(&circlePoly, particleCount);

	// Setup some polyData to render as a point cloud and setup a collection of particles referencing the positions
	PolyData ptCloudPolyData;
	ptCloudPolyData.allocate(particleCount, CellType::POINT);
	Cell* data = ptCloudPolyData.getPointData();
	Particle* particles = new Particle[particleCount];
	for (UINT i = 0; i < results.size(); i++)
	{
		data[i].v1.pos = results[i];

		particles[i].pos = &data[i].v1.pos; // Ref to avoid storing two copies
		particles[i].bulk = BULK_MODULUS;
		particles[i].shear = SHEAR_MODULUS;
	}

	// Setup the mapper to draw the point cloud
	PolyDataMapper ptCloudMapper;
	ptCloudMapper.setInput(&ptCloudPolyData);
	ptCloudMapper.setMaterial(ren.getMaterial(1));
	ptCloudMapper.update();
	ren.addRenderItem(&ptCloudMapper);

	// Setup the MPMGrid for simulation
	geom2d::Rect bounds = MathHelp::get2dBounds(results.data(), particleCount);
	MPMGrid mpmGrid;
	GLfloat padScale = 4.0f;
	glm::vec2 padSize = bounds.size() * padScale;
	glm::vec2 origin = bounds.pos - padSize * 0.5f;
	mpmGrid.initGrid(origin, padSize, 32, 32);
	mpmGrid.initParticles(particles, particleCount);
#pragma endregion

	// Update loop
	UINT iter = 0;
	while (renWindow.isActive())
	{
		//auto start = std::chrono::steady_clock::now();

		mpmGrid.projectToGrid();
		mpmGrid.update(TIMESTEP);
		ptCloudMapper.update(); // Update buffers
		printIterationStats(&mpmGrid, iter++);

		renWindow.render();

		//auto end = std::chrono::steady_clock::now();
		//double frameTime = std::chrono::duration<double, std::milli>(end - start).count() / 1000.0; // In seconds
		//printf("Frame Time: %fs\n", frameTime);
		//printf("FPS: %f\n", 1.0 / frameTime);
	}

	delete[] particles;

	return 1;
}

void printIterationStats(MPMGrid* mpmGrid, UINT iter)
{
	printf("\nITERATION: %d\n", iter);

	printf("Max Particle Deformation Gradient Det: %.*f\n", 10, mpmGrid->maxParticleDefDet);
	printf("Max Particle Deformation Gradient: \n");
	MathHelp::printMat(mpmGrid->maxParticleDef);
	printf("Max Particle Velocity Gradient Det:    %.*f\n", 10, mpmGrid->maxParticleVGDet);
	printf("Max Particle Velocity Gradient: \n");
	MathHelp::printMat(mpmGrid->maxParticleVG);
	printf("Max Particle Energy Derivative Det:    %.*f\n", 10, mpmGrid->maxParticleEnergyDerivativeDet);
	printf("Max Particle Energy Derivative: \n");
	MathHelp::printMat(mpmGrid->maxParticleEnergyDerivative);

	printf("Max Deformation Det:                   %.*f\n", 10, mpmGrid->maxParticleDefDet);
	printf("Max Particle Velocity:                 %.*f\n", 10, mpmGrid->maxParticleVelocity);

	printf("Max Node Velocity:                     %.*f\n", 10, mpmGrid->maxNodeVelocity);
	printf("Max Node Force                         %.*f\n", 10, mpmGrid->maxNodeF);
}