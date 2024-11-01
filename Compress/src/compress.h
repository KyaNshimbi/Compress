#pragma once

#include <png.h>
#include <zlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

class PNGCompress
{
private:
	png_bytepp rowPointers;
	png_uint_32 width;
	png_uint_32 height;
	png_byte colorType;
	png_byte bitDepth;

	FILE* readingFPtr;

	png_structp readingStruct;
	png_infop readingInfoPtr;
	std::string m_fileName;
public:
	PNGCompress(const char* fileName = "");

	PNGCompress(const PNGCompress& other) = delete;

	bool open(const char* fileName);

	void initialise();

	void compress() const;

	~PNGCompress()
	{
		if (readingFPtr)
		{
			fclose(readingFPtr);
		}
		if (readingStruct)
		{
			png_destroy_read_struct(&readingStruct, &readingInfoPtr, nullptr);
		}

	}
	
};