#pragma once
#include "MathHelper.h"
#include "Types.h"

// Stores image data of uchar, int, float, or double
class ImageData
{
public:
	ImageData() { }

	~ImageData()
	{
		switch (type) 
		{ 
			TemplateMacro(deleteData(static_cast<TT>(0)));
		default:
			break;
		}
	}

	void allocate2DImage(UINT* dim, double* spacing, double* origin, UINT numComps, ScalarType type);

	UINT* getDimensions() { return dim; }
	void* getData() { return data; }
	
	// Updates bounds given dimensions, spacing, & origin
	void updateBounds();

protected:
	template<class T>
	void allocateData(T, int count) { data = new T[count]; }

	template<class T>
	void deleteData(T) { delete[] static_cast<T*>(data); }

protected:
	void* data = nullptr;
	UINT dim[3] = { 0, 0, 0 };
	UINT numComps = 1;
	ScalarType type = ScalarType::UCHAR_T;

	double spacing[3] = { 1.0, 1.0, 1.0 };
	double origin[3] = { 0.0, 0.0, 0.0 };
	double bounds[6] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
};