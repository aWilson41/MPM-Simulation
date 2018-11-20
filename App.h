#pragma once
//#include "Engine/TrackballCamera.h"
#include "Engine/GLContext.h"
#include <vector>

//class MPMGrid;
//class Material;
//class Particle;
//class Plane;
//class PointCloud;

class App
{
public:
	App();
	~App();

	void init();
	void loop();
	bool isActive() { return context.isActive(); }

//protected:
//	void simulate();
//	void printIterationStats();
//	
protected:
	GLContext context;

private:
	//TrackballCamera cam;

	//Plane* plane = nullptr;
	//Plane* boundsPlane = nullptr;
	//PointCloud* ptCloud = nullptr;
	//MPMGrid* mpmGrid = nullptr;
	//std::vector<Material*> materials;
	//std::vector<Particle> particles;

	//glm::vec3 lightDir = glm::normalize(glm::vec3(0.0f, 1.0f, 1.0f));

	//glm::vec2 mousePos = glm::vec2(0.0f);

	//bool running = false;
	//UINT iter = 0;
};