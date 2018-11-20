//#include "ImageData.h"
//
//void ImageData::allocate2DImage(UINT* dim, double* spacing, double* origin, UINT numComps, ScalarType type)
//{
//	ImageData::dim[0] = dim[0];
//	ImageData::dim[1] = dim[1];
//	ImageData::dim[2] = 1;
//	ImageData::spacing[0] = spacing[0];
//	ImageData::spacing[1] = spacing[1];
//	ImageData::dim[2] = 0.0;
//	ImageData::origin[0] = origin[0];
//	ImageData::origin[1] = origin[1];
//	ImageData::origin[2] = 0.0;
//	ImageData::numComps = numComps;
//	updateBounds();
//
//	switch (type)
//	{
//		TemplateMacro(allocateData(static_cast<TT>(0)));
//	default:
//		break;
//	}
//}
//
//void ImageData::updateBounds()
//{
//	bounds[0] = origin[0];
//	bounds[1] = origin[0] + dim[0] * spacing[0];
//	bounds[2] = origin[1];
//	bounds[3] = origin[1] + dim[1] * spacing[1];
//	bounds[4] = origin[2];
//	bounds[5] = origin[2] + dim[2] * spacing[2];
//}