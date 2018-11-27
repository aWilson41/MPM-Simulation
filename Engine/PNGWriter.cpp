#include "PNGWriter.h"
#include "ImageData.h"
#include "LodePNG/lodepng.h"

void PNGWriter::update()
{
	if (fileName == "")
	{
		printf("PNGWriter: No file name specified.\n");
		return;
	}

	if (imageData->getScalarType() != UCHAR_T)
	{
		printf("PNGWriter: Can only write unsigned char images.\n");
		return;
	}

	// Copy the image to std vector because that's how lodepng wants it (add component)
	UINT* dim = imageData->getDimensions();
	std::vector<unsigned char> image(dim[0] * dim[1] * 4);
	unsigned char* data = static_cast<unsigned char*>(imageData->getData());

	UINT numComps = imageData->getNumComps();
	if (numComps == 1)
	{
		for (UINT i = 0; i < dim[0] * dim[1]; i++)
		{
			UINT i2 = i * 4;
			image[i2] = data[i];
			image[i2 + 1] = data[i];
			image[i2 + 2] = data[i];
			image[i2 + 3] = 255;
		}
	}
	else if (numComps == 3)
	{
		// Image must be flipped vertically
		for (UINT x = 0; x < dim[0]; x++)
		{
			for (UINT y = 0; y < dim[1]; y++)
			{
				int index = 4 * (dim[0] * y + x);
				int index1 = 3 * (dim[0] * (dim[1] - y - 1) + x);
				image[index] = data[index1];
				image[index + 1] = data[index1 + 1];
				image[index + 2] = data[index1 + 2];
				image[index + 3] = 255;
			}
		}
	}
	else if (numComps == 4)
	{
		for (UINT i = 0; i < dim[0] * dim[1] * 4; i++)
		{
			image[i] = data[i];
		}
	}
	else
	{
		printf("PNGWriter: Can only write 1, 3, or 4 component images.\n");
		return;
	}

	// Write the image with lodepng
	if (lodepng::encode(fileName, image, dim[0], dim[1]))
		printf("PNGWriter: LodePNG Error\n");
}