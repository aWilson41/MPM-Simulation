#include "PNGReader.h"
#include "ImageData.h"
#include "LodePNG/lodepng.h"

PNGReader::~PNGReader()
{
	if (imageData != nullptr)
		delete imageData;
}

void PNGReader::update()
{
	if (fileName == "")
		return;

	if (imageData != nullptr)
		delete imageData;

	std::vector<unsigned char> png;
	std::vector<unsigned char> image;
	if (lodepng::load_file(png, fileName) != 0)
	{
		printf(("Error reading: " + fileName + "\n").c_str());
		return;
	}

	UINT width, height;
	if (lodepng::decode(image, width, height, png) != 0)
	{
		printf(("Error reading: " + fileName + "\n").c_str());
		return;
	}

	imageData = new ImageData();
	static UINT dim[] = { width, height, 1 };
	static double spacing[] = { 0.0, 0.0, 0.0 };
	static double origin[] = { 0.0, 0.0, 0.0 };
	imageData->allocate2DImage(dim, spacing, origin, 3, ScalarType::UCHAR_T);
	unsigned char* data = static_cast<unsigned char*>(imageData->getData());

	// Copy the loaded image
	for (UINT i = 0; i < width * height; i++)
	{
		UINT index1 = i * 3;
		UINT index2 = i * 4;
		data[index1] = image[index2];         // r
		data[index1 + 1] = image[index2 + 1]; // g
		data[index1 + 2] = image[index2 + 2]; // b
	}
}