#pragma once
#include <string>

class ImageData;

class PNGReader
{
public:
	~PNGReader();

public:
	ImageData* getOutput() { return imageData; }
	std::string getFileName() { return fileName; }

	void setFileName(std::string fileName) { PNGReader::fileName = fileName; }

	void update();

protected:
	ImageData* imageData = nullptr;
	std::string fileName = "";
};