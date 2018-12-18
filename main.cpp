#include "Constants.h"
#include "Engine/Geometry2D.h"
#include "Engine/ImageData.h"
#include "Engine/ImageMapper.h"
#include "Engine/Material.h"
#include "Engine/PlaneSource.h"
#include "Engine/PNGWriter.h"
#include "Engine/PolyData.h"
#include "Engine/PolyDataMapper.h"
#include "Engine/Renderer.h"
#include "Engine/RenderWindow.h"
#include "Engine/TrackballCamera.h"
#include "Engine/TrackballCameraInteractor.h"
#include "MPMGrid.h"
#include "Particle.h"

void printIterationStats(MPMGrid* mpmGrid);
// Updates image with mass values from mpm grid
void updateGridImage(MPMGrid* mpmGrid, ImageMapper* mapper);
// Updates particle poly data with scalar values from particles
void updateParticlePoly(MPMGrid* mpmGrid, PolyDataMapper* mapper);

int main(int argc, char *argv[])
{
	// Create the window
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
	ren.addMaterial(Material(glm::vec3(0.2f, 0.4f, 0.2f), 0.5f));
	ren.addMaterial(Material(glm::vec3(0.8f, 0.2f, 0.2f), 1.0f));
	renWindow.setRenderer(&ren);

	// Setup the camera interactor (maps user window input to camera)
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

	// Generate a 2d poly from a circle
	geom2d::Poly circlePoly;
	circlePoly.FromCircle(geom2d::Circle(0.0f, 0.0f, 0.5f), 25); // radius in meters

	// Fill the circle with randomely distributed positions
	GLfloat circlePolyArea = circlePoly.area();
	GLfloat particleArea = PARTICLE_DIAMETER * PARTICLE_DIAMETER;
	UINT particleCount = static_cast<UINT>(circlePolyArea / particleArea);
	printf("Polygon Area:   %f\n", circlePolyArea);
	printf("Particle Area:  %f\n", particleArea);
	printf("Particle Count: %d\n", particleCount);
	std::vector<glm::vec2> results = MathHelp::generatePointCloud(&circlePoly, particleCount);

	// Setup the polydata and particles
	PolyData ptCloudPolyData;
	ptCloudPolyData.allocateVertexData(particleCount, CellType::POINT);
	ptCloudPolyData.allocateScalarData(3);
	glm::vec3* posData = reinterpret_cast<glm::vec3*>(ptCloudPolyData.getVertexData());
	glm::vec3* colorData = reinterpret_cast<glm::vec3*>(ptCloudPolyData.getScalarData());
	Particle* particles = new Particle[particleCount];
	for (UINT i = 0; i < results.size(); i++)
	{
		posData[i] = glm::vec3(results[i], 0.0f);

		particles[i].pos = &posData[i];
		particles[i].mass = PARTICLE_MASS;
		particles[i].bulk = BULK_MODULUS;
		particles[i].shear = SHEAR_MODULUS;
	}

	// Setup the particles mapper
	PolyDataMapper ptCloudMapper;
	ptCloudMapper.setInput(&ptCloudPolyData);
	ptCloudMapper.setMaterial(ren.getMaterial(1));
	ptCloudMapper.setPolyRepresentation(CellType::POINT);
	ptCloudMapper.update();
	ren.addRenderItem(&ptCloudMapper);

	// Setup the MPMGrid for simulation
	geom2d::Rect bounds = MathHelp::get2dBounds(results.data(), results.size());
	MPMGrid mpmGrid;
	GLfloat padScale = 2.0f;
	glm::vec2 padSize = bounds.size() * padScale;
	glm::vec2 origin = bounds.pos - padSize * 0.5f;
	mpmGrid.initGrid(origin, padSize, 32, 32);
	mpmGrid.initParticles(particles, particleCount);

	// Setup a background image for visualizing the node values
	ImageMapper imageMapper;
	imageMapper.setModelMatrix(MathHelp::matrixTranslate(0.0f, bounds.pos.y, -0.1f));
	ren.addRenderItem(&imageMapper);

	// Update loop
	UINT frameCount = 0;
	float frameTime = 0.0f;
	while (renWindow.isActive())
	{
		printf("Frame: %d\n", frameCount);
		printf("FrameTime: %f\n", frameTime);
#ifdef STATS
		auto start = std::chrono::steady_clock::now();
#endif
		// Do the actual simulation
		for (UINT i = 0; i < SUBSTEPS; i++)
		{
			mpmGrid.projectToGrid();
			// Split so user can apply their own forces to the velocities
			mpmGrid.update(TIMESTEP);
		}
		frameTime += TIMESTEP * SUBSTEPS;
#ifdef STATS
		auto end = std::chrono::steady_clock::now();
		printf("Sim Time: %fs\n", std::chrono::duration<double, std::milli>(end - start).count() / 1000.0);
		printIterationStats(&mpmGrid);
#endif

		updateGridImage(&mpmGrid, &imageMapper);
		updateParticlePoly(&mpmGrid, &ptCloudMapper);
		renWindow.render();
#ifdef OUTPUTFRAMES
		// Get the frame
		GLint vp[4];
		glGetIntegerv(GL_VIEWPORT, vp);
		ImageData image;
		UINT dim[3] = { static_cast<UINT>(vp[2]), static_cast<UINT>(vp[3]), 1 };
		double spacing[3] = { 1.0, 1.0, 1.0 };
		double origin[3] = { 0.0, 0.0, 0.0 };
		image.allocate2DImage(dim, spacing, origin, 3, ScalarType::UCHAR_T);
		glReadPixels(0, 0, dim[0], dim[1], GL_RGB, GL_UNSIGNED_BYTE, image.getData());
		// Write the frame as png
		PNGWriter writer;
		if (frameCount < 10)
			writer.setFileName("output/frame_000" + std::to_string(frameCount) + ".png");
		else if (frameCount < 100)
			writer.setFileName("output/frame_00" + std::to_string(frameCount) + ".png");
		else if (frameCount < 1000)
			writer.setFileName("output/frame_0" + std::to_string(frameCount) + ".png");
		else if (frameCount < 10000)
			writer.setFileName("output/frame_" + std::to_string(frameCount) + ".png");
		writer.setInput(&image);
		writer.update();
#endif
		frameCount++;
	}

	delete[] particles;

	return 1;
}

void printIterationStats(MPMGrid* mpmGrid)
{
	printf("Max Particle defGe Det: %.*f\n", 10, mpmGrid->maxParticleDefDete);
	printf("Max Particle defGe: \n");
	MathHelp::printMat(mpmGrid->maxParticleDefe);
	printf("Max Particle defGp Det: %.*f\n", 10, mpmGrid->maxParticleDefDetp);
	printf("Max Particle defGp: \n");
	MathHelp::printMat(mpmGrid->maxParticleDefp);

	printf("Max Particle Velocity Gradient Det:    %.*f\n", 10, mpmGrid->maxParticleVGDet);
	printf("Max Particle Velocity Gradient: \n");
	MathHelp::printMat(mpmGrid->maxParticleVG);
	printf("Max Particle Velocity:                 %.*f\n", 10, mpmGrid->maxParticleVelocityMag);
	MathHelp::printVec(mpmGrid->maxParticleVelocity);

	printf("Max Node Velocity:                     %.*f\n", 10, mpmGrid->maxNodeVelocity);
	printf("Max Node Force                         %.*f\n", 10, mpmGrid->maxNodeForceMag);
	MathHelp::printVec(mpmGrid->maxNodeForce);
}

// Puts the mass values from the mpmgrid into an image
void updateGridImage(MPMGrid* mpmGrid, ImageMapper* mapper)
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

// Puts the defGp values into color values for the poly
void updateParticlePoly(MPMGrid* mpmGrid, PolyDataMapper* mapper)
{
	PolyData* polyData = mapper->getInput();
	glm::vec3* colors = reinterpret_cast<glm::vec3*>(polyData->getScalarData());
	GLfloat max = 0.0f;
	for (UINT i = 0; i < polyData->getNumOfPoints(); i++)
	{
		colors[i].x = glm::determinant(mpmGrid->particles[i].defGp);
		if (colors[i].x > max)
			max = colors[i].x;
		colors[i].y = 0.0f;
		colors[i].z = 0.0f;
	}
	for (UINT i = 0; i < polyData->getNumOfPoints(); i++)
	{
		colors[i].x *= 1.0f / max;
	}
	mapper->update();
}