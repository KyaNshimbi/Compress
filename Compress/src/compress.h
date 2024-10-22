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
	PNGCompress(const char* fileName = "")
		: rowPointers{ nullptr }, width{ 0 }, height{ 0 }, colorType{ 0 }, bitDepth{ 0 }, readingFPtr{ nullptr }, readingStruct{ nullptr }, readingInfoPtr{ nullptr }, m_fileName{ fileName }
	{
		if (!open(fileName))
		{
			abort();
		}
	}

	PNGCompress(const PNGCompress& other) = delete;

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

	bool open(const char* fileName)
	{
		m_fileName = fileName;
		fopen_s(&readingFPtr, m_fileName.c_str(), "rb");
		if (!readingFPtr)
		{
			fileName = "";
			return false;
		}
		return true;
	}

	void initialise()
	{
		if (!readingFPtr)
		{
			abort();
		}

		//Checking if the first 8 bytes have been read successfully
		size_t number{ 8 };
		png_byte header[8];
		size_t read = fread(header, sizeof(header[0]), number, readingFPtr);
		if (read != number)
		{
			abort();
		}
		//Checking if the bytes are corresponding to the png header signature
		if (png_sig_cmp(header, 0, number))
		{
			abort();
		}

		//Initialising the PNG Structure
		readingStruct =
			png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
		if (!readingStruct)
		{
			abort();
		}
		//Initialising PNG Info
		readingInfoPtr = png_create_info_struct(readingStruct);
		if (!readingInfoPtr)
		{
			png_destroy_read_struct(&readingStruct, nullptr, nullptr);
			abort();
		}
		//If libpng encounteres an error a long jump
		if (setjmp(png_jmpbuf(readingStruct)))
		{
			png_destroy_read_struct(&readingStruct, &readingInfoPtr, nullptr);
			abort();
		}

		//Initialising the pngIO to defaults
		png_init_io(readingStruct, readingFPtr);

		//Notifying libpng that we checked the signature of the png file and some bytes are missing from the start of the file
		png_set_sig_bytes(readingStruct, number);

		//High level read interface
		png_read_png(readingStruct, readingInfoPtr, PNG_TRANSFORM_IDENTITY, nullptr);

		rowPointers = png_get_rows(readingStruct, readingInfoPtr);
		width = png_get_image_width(readingStruct, readingInfoPtr);
		height = png_get_image_height(readingStruct, readingInfoPtr);
		colorType = png_get_color_type(readingStruct, readingInfoPtr);
		bitDepth = png_get_bit_depth(readingStruct, readingInfoPtr);
	}

	void compress() const
	{
		if (!rowPointers)
		{
			abort();
		}

		std::string writeName{ m_fileName };
		writeName.erase(writeName.size() - 4, 4);
		writeName.append("Compressed.png");
		FILE* fPtr{ nullptr };
		fopen_s(&fPtr, writeName.c_str(), "wb");

		if (!fPtr)
		{
			abort();
		}
		png_structp pngPtr{ png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr) };
		if (!pngPtr)
		{
			abort();
		}
		png_infop infoPtr{ png_create_info_struct(pngPtr) };
		if (!infoPtr)
		{
			png_destroy_write_struct(&pngPtr, nullptr);
			abort();
		}
		if (setjmp(png_jmpbuf(pngPtr)))
		{
			png_destroy_write_struct(&pngPtr, &infoPtr);
			fclose(fPtr);
			abort();
		}
		png_init_io(pngPtr, fPtr);

		// Set the zlib compression level
		png_set_compression_level(pngPtr, Z_BEST_COMPRESSION);

		// Filling the info structure with all the data needed to write before the actual image
		png_set_IHDR(pngPtr, infoPtr, width, height, bitDepth, colorType, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

		// Setting image data into the info structure
		png_set_rows(pngPtr, infoPtr, rowPointers);

		// High lever write interface
		png_write_png(pngPtr, infoPtr, PNG_TRANSFORM_IDENTITY, nullptr);

		// Free memory allocated by libpng
		png_destroy_write_struct(&pngPtr, &infoPtr);
	}
};