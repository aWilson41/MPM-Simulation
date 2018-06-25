#pragma once

class Particle;

class Spring
{
public:
	Particle* p1 = nullptr;
	Particle* p2 = nullptr;
	float ks = 40.0f;
	float kd = 0.5f;
	float restingLength = 1.0f;

public:
	Spring() { }

	Spring(Particle* p1, Particle* p2, float restDist)
	{
		Spring::p1 = p1;
		Spring::p2 = p2;
		restingLength = restDist;
	}

	void applySpringForce();
};