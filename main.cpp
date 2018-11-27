#include "Constants.h"
#include "Engine/Geometry2D.h"
#include "Engine/ImageData.h"
#include "Engine/ImageMapper.h"
#include "Engine/Material.h"
#include "Engine/PlaneSource.h"
#include "Engine/PNGReader.h"
#include "Engine/PNGWriter.h"
#include "Engine/PolyData.h"
#include "Engine/PolyDataMapper.h"
#include "Engine/Renderer.h"
#include "Engine/RenderWindow.h"
#include "Engine/TrackballCameraInteractor.h"
#include "MPMGrid.h"
#include "Particle.h"
#include <chrono>

void printIterationStats(MPMGrid* mpmGrid);
// Updates image with mass values from grid
void updateMassImage(MPMGrid* mpmGrid, ImageMapper* mapper);
// Updates particles with color attributes
void updateParticleColors(Particle* particles, PolyDataMapper* mapper);

int main(int argc, char *argv[])
{
	// Create the window and give it the renderer
	// This has to happen before any gl calls in other objects because 
	// glfw can only make the opengl context when creating the window.
	RenderWindow renWindow;
	renWindow.setWindowName("MPM Simulation");

	// Create the camera for the renderer to use
	TrackballCamera cam;
	cam.initTrackballCamera(1.4f, 1.57f, 30.0f);

	// Create the renderer
	Renderer ren;
	ren.setCamera(&cam);
	ren.addMaterial(new Material(glm::vec3(0.2f, 0.4f, 0.2f), 0.5f));
	ren.addMaterial(new Material(glm::vec3(0.8f, 0.2f, 0.2f), 1.0f));
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
	planeMapper.setModelMatrix(MathHelp::matrixTranslate(0.0f, -2.0f, 0.0f) * MathHelp::matrixScale(10.0f));
	planeMapper.update();
	ren.addRenderItem(&planeMapper);

#pragma region Generate Particles and Grid
	// Generate a 2d poly from a circle
	geom2d::Poly circlePoly;
	circlePoly.FromCircle(geom2d::Circle(0.0f, 0.0f, 0.25f), 25);
	GLfloat circlePolyArea = circlePoly.area();
	GLfloat particleArea = PARTICLE_DIAMETER * PARTICLE_DIAMETER;
	UINT particleCount = static_cast<UINT>(circlePolyArea / particleArea);
	printf("Polygon Area:   %f\n", circlePolyArea);
	printf("Particle Area:  %f\n", particleArea);
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
		particles[i].mass = PARTICLE_MASS;
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
	GLfloat padScale = 2.0f;
	glm::vec2 padSize = bounds.size() * padScale;
	glm::vec2 origin = bounds.pos - padSize * 0.5f;
	mpmGrid.initGrid(origin, padSize, 8, 8);
	mpmGrid.initParticles(particles, particleCount);

	// Setup a plane to draw the bounds of the simulation
	/*PlaneSource boundsPlane;
	boundsPlane.update();

	PolyDataMapper boundsMapper;
	boundsMapper.setModelMatrix(
		MathHelp::matrixTranslate(bounds.pos.x, bounds.pos.y, 0.0f) * 
		MathHelp::matrixScale(padSize.x, padSize.y, 1.0f) *
		MathHelp::matrixRotateX(-HALFPI));
	boundsMapper.setPolyRepresentation(PolyRep::LINEREP);
	boundsMapper.setInput(boundsPlane.getOutput());
	boundsMapper.setMaterial(ren.getMaterial(1));
	boundsMapper.update();
	ren.addRenderItem(&boundsMapper);*/

	// Setup a background image for visualizing the node values
	ImageMapper imageMapper;
	imageMapper.setModelMatrix(MathHelp::matrixTranslate(0.0f, bounds.pos.y, -0.1f));
	ren.addRenderItem(&imageMapper);
#pragma endregion

	// Update loop
	UINT iter = 0;
	const UINT subSteps = 1;
	while (renWindow.isActive())
	{
		auto start = std::chrono::steady_clock::now();
		for (UINT i = 0; i < subSteps; i++)
		{
			mpmGrid.projectToGrid();
			mpmGrid.update(TIMESTEP);
			
		}
		auto end = std::chrono::steady_clock::now();
		printf("Sim Time: %fs\n", std::chrono::duration<double, std::milli>(end - start).count() / 1000.0);

		iter++;
		printf("ITERATION: %d\n", iter * subSteps);
		printf("Frame: %d\n", iter);

		printIterationStats(&mpmGrid);

		ptCloudMapper.update(); // Update buffers
		updateMassImage(&mpmGrid, &imageMapper);
		renWindow.render();

		//auto end = std::chrono::steady_clock::now();
		//double frameTime = std::chrono::duration<double, std::milli>(end - start).count() / 1000.0; // In seconds
		//printf("Frame Time: %fs\n", frameTime);
		//printf("FPS: %f\n", 1.0 / frameTime);

		// Grab and write the frame
		/*GLint vp[4];
		glGetIntegerv(GL_VIEWPORT, vp);
		ImageData image;
		UINT dim[3] = { vp[2], vp[3], 1 };
		double spacing[3] = { 1.0, 1.0, 1.0 };
		double origin[3] = { 0.0, 0.0, 0.0 };
		image.allocate2DImage(dim, spacing, origin, 3, ScalarType::UCHAR_T);
		glReadPixels(0, 0, dim[0], dim[1], GL_RGB, GL_UNSIGNED_BYTE, image.getData());

		PNGWriter writer;
		writer.setFileName("output/frame_" + std::to_string(iter) + ".png");
		writer.setInput(&image);
		writer.update();*/
	}

	delete[] particles;

	return 1;
}

void printIterationStats(MPMGrid* mpmGrid)
{
	printf("Max Particle Deformation Gradient Det: %.*f\n", 10, mpmGrid->maxParticleDefDet);
	printf("Max Particle Deformation Gradient: \n");
	MathHelp::printMat(mpmGrid->maxParticleDef);
	printf("Max Particle Velocity Gradient Det:    %.*f\n", 10, mpmGrid->maxParticleVGDet);
	printf("Max Particle Velocity Gradient: \n");
	MathHelp::printMat(mpmGrid->maxParticleVG);

	printf("Max Deformation Det:                   %.*f\n", 10, mpmGrid->maxParticleDefDet);
	printf("Max Particle Velocity:                 %.*f\n", 10, mpmGrid->maxParticleVelocityMag);
	MathHelp::printVec(mpmGrid->maxParticleVelocity);

	printf("Max Node Velocity:                     %.*f\n", 10, mpmGrid->maxNodeVelocity);
	printf("Max Node Force                         %.*f\n", 10, mpmGrid->maxNodeForceMag);
	MathHelp::printVec(mpmGrid->maxNodeForce);
}

void updateMassImage(MPMGrid* mpmGrid, ImageMapper* mapper)
{
	// Extract the mass into an image
	ImageData* imageData = new ImageData();
	UINT dim[3] = { static_cast<UINT>(mpmGrid->gridWidth), static_cast<UINT>(mpmGrid->gridHeight), 1 };
	double spacing[3] = { mpmGrid->cellSize.x, mpmGrid->cellSize.y, 0.0 };
	double origin[3] = { 0.0, 0.0, 0.0 };
	imageData->allocate2DImage(dim, spacing, origin, 1, ScalarType::UCHAR_T);
	unsigned char* data = static_cast<unsigned char*>(imageData->getData());

	// Get the max mass
	GLfloat max = 0.0f;
	for (int i = 0; i < mpmGrid->gridHeight * mpmGrid->gridWidth; i++)
	{
		if (mpmGrid->nodes[i].mass > max)
			max = mpmGrid->nodes[i].mass;
	}
	// Set the image values
	GLfloat ratio = 255.0f / max;
	for (int i = 0; i < mpmGrid->gridHeight * mpmGrid->gridWidth; i++)
	{
		data[i] = static_cast<unsigned char>(mpmGrid->nodes[i].mass * ratio);
	}

	mapper->setInput(imageData);
	mapper->update();
}

void updateParticleColors(Particle* particles, PolyDataMapper* mapper)
{
	PolyData* polyData = mapper->getInput();
}