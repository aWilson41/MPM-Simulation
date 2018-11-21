#pragma once

class PolyData;

class PlaneSource
{
public:
	PlaneSource();
	~PlaneSource();

public:
	// Updates the output poly data to the plane specified by the parameters
	void update();

	PolyData* getOutput() { return outputData; }

protected:
	PolyData* outputData = nullptr;
};