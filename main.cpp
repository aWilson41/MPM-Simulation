#include "Constants.h"
#include "MPMGrid.h"
#include "Particle.h"
#include <CircleSource.h>
#include <GlyphPolyDataMapper.h>
#include <ImageData.h>
#include <ImageMapper.h>
#include <PhongMaterial.h>
#include <PlaneSource.h>
#include <PolyData.h>
#include <PolyDataMapper.h>
#include <PolyDataPointCloud.h>
#include <Renderer.h>
#include <RenderWindow.h>
#include <SphereSource.h>
#include <TrackballCamera.h>
#include <TrackballCameraInteractor.h>

// Updates image with mass values from mpm grid
void updateGridImage(MPMGrid* mpmGrid, ImageMapper* mapper);
// Updates particle poly data with scalar values from particles
void updateParticlePoly(MPMGrid* mpmGrid, GlyphPolyDataMapper* mapper);

int main(int argc, char *argv[])
{
	// Create the window
	// This has to happen before any gl calls in other objects because 
	// glfw can only make the opengl context when creating the window.
	RenderWindow renWindow("MPM Simulation");

	// Create the camera for the renderer to use
	TrackballCamera cam;
	cam.initTrackballCamera(1.4f, 1.57f, 30.0f);

	// Create the renderer
	Renderer ren;
	ren.setCamera(&cam);
	ren.addMaterial(PhongMaterial(glm::vec3(0.2f, 0.4f, 0.2f), 0.5f));
	ren.addMaterial(PhongMaterial(glm::vec3(0.8f, 0.2f, 0.2f), 1.0f));
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

	// Generate point cloud within circle
	CircleSource particleShape;
	particleShape.setRadius(0.5f);
	particleShape.setDivisions(25);
	particleShape.update();
	const UINT particleCount = static_cast<UINT>(particleShape.getOutput()->getArea() / (PARTICLE_DIAMETER * PARTICLE_DIAMETER));
	PolyDataPointCloud polyDataPtCloud;
	polyDataPtCloud.setUse2d(true);
	polyDataPtCloud.setOptimizeByRadius(true);
	polyDataPtCloud.setRadius(0.05f);
	polyDataPtCloud.setNumberOfIterations(30);
	polyDataPtCloud.setNumberOfPoints(particleCount);
	polyDataPtCloud.setInput(particleShape.getOutput());
	polyDataPtCloud.update();
	PolyData* ptCloudPolyData = polyDataPtCloud.getOutput();

	// Create a uv sphere source for instancing
	SphereSource particleSphereSource;
	particleSphereSource.setRadius(0.02f);
	particleSphereSource.update();
	// Create the particle mapper
	GlyphPolyDataMapper particleMapper;
	particleMapper.setInput(particleSphereSource.getOutput());
	particleMapper.allocateOffsets(particleCount);
	particleMapper.allocateColorData(particleCount);
	glm::vec3* offsetData = reinterpret_cast<glm::vec3*>(particleMapper.getOffsetData());
	// Setup the polydata and particles
	glm::vec3* posData = reinterpret_cast<glm::vec3*>(ptCloudPolyData->getVertexData());
	Particle* particles = new Particle[particleCount];
	for (UINT i = 0; i < particleCount; i++)
	{
		offsetData[i] = posData[i];

		particles[i].pos = &offsetData[i];
		particles[i].mass = PARTICLE_MASS;
		particles[i].bulk = BULK_MODULUS;
		particles[i].shear = SHEAR_MODULUS;
	}

	ren.addRenderItem(&particleMapper);

	// Setup the MPMGrid for simulation
	MPMGrid mpmGrid;
	GLfloat padScale = 2.0f;
	GLfloat* bounds = polyDataPtCloud.getBounds();
	glm::vec2 padSize = glm::vec2(bounds[1] - bounds[0], bounds[3] - bounds[2]) * padScale;
	glm::vec2 boundsCenter = glm::vec2(bounds[1] + bounds[0], bounds[3] + bounds[2]) * 0.5f;
	glm::vec2 origin = boundsCenter - padSize * 0.5f;
	mpmGrid.initGrid(origin, padSize, GRID_DIM, GRID_DIM);
	mpmGrid.initParticles(particles, particleCount);

	updateParticlePoly(&mpmGrid, &particleMapper);

	// Setup a background image for visualizing the node values
	ImageMapper imageMapper;
	imageMapper.setModelMatrix(MathHelp::matrixTranslate(0.0f, boundsCenter.y, -0.1f));
	ren.addRenderItem(&imageMapper);

	// Update loop
	UINT frameCount = 0;
	float frameTime = 0.0f;
	while (renWindow.isActive())
	{
#ifdef STATS
		printf("Frame: %d\n", frameCount);
		auto start = std::chrono::steady_clock::now();
#endif
		// Do the actual simulation
		for (UINT i = 0; i < SUBSTEPS; i++)
		{
			mpmGrid.projectToGrid();
			// Split so user can apply their own forces to the velocities
			mpmGrid.update(TIMESTEP);
		}
		
#ifdef STATS
		frameTime += TIMESTEP * SUBSTEPS;
		printf("PostFrameTime: %f\n", frameTime);
		auto end = std::chrono::steady_clock::now();
		printf("Sim Time: %fs\n", std::chrono::duration<double, std::milli>(end - start).count() / 1000.0);
#endif

		updateGridImage(&mpmGrid, &imageMapper);
		updateParticlePoly(&mpmGrid, &particleMapper);
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
void updateParticlePoly(MPMGrid* mpmGrid, GlyphPolyDataMapper* mapper)
{
	glm::vec3* colors = reinterpret_cast<glm::vec3*>(mapper->getColorData());
	GLfloat max = 0.0f;
	for (UINT i = 0; i < mapper->getInstanceCount(); i++)
	{
		colors[i].y = glm::determinant(mpmGrid->particles[i].defGp);
		if (colors[i].y > max)
			max = colors[i].y;
		colors[i].x = 0.0f;
		colors[i].z = 0.0f;
	}
	for (UINT i = 0; i < mapper->getInstanceCount(); i++)
	{
		colors[i].y *= 1.0f / max;
	}
	mapper->update();
}