#include <png.h>
#include <zlib.h>
#include <stdlib.h>
#include <stdio.h>

class PNGCompress
{
private:
	png_bytepp rowPointers;
	png_uint_32 width;

};
void readPNGFile(const char* fileName, png_bytepp& rowPointers, png_uint_32& width, png_uint_32& height, png_byte& colorType, png_byte& bitDepth)
{
	FILE* fPtr{ nullptr };
	fopen_s(&fPtr, fileName, "rb");

	if (!fPtr)
	{
		abort();
	}

	//Checking if the first 8 bytes have been read successfully
	size_t number{ 8 };
	png_byte header[8];
	size_t read = fread(header, sizeof(header[0]), number, fPtr);
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
	png_structp pngPtr{
		png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr,nullptr, nullptr) };
	if (!pngPtr)
	{
		abort();
	}
	//Initialising PNG Info
	png_infop infoPtr{ png_create_info_struct(pngPtr) };
	if (!infoPtr)
	{
		png_destroy_read_struct(&pngPtr, nullptr, nullptr);
		abort();
	}
	//If libpng encounteres an error a long jump
	if (setjmp(png_jmpbuf(pngPtr)))
	{
		png_destroy_read_struct(&pngPtr, &infoPtr, nullptr);
		abort();
	}
	
	//Initialising the pngIO to defaults
	png_init_io(pngPtr, fPtr);

	//Notifying libpng that we checked the signature of the png file and some bytes are missing from the start of the file
	png_set_sig_bytes(pngPtr, number);

	//High level read interface
	png_read_png(pngPtr, infoPtr, PNG_TRANSFORM_IDENTITY, nullptr);

	rowPointers = png_get_rows(pngPtr, infoPtr);
	width = png_get_image_width(pngPtr, infoPtr);
	height = png_get_image_height(pngPtr, infoPtr);
	colorType = png_get_color_type(pngPtr, infoPtr);
	bitDepth = png_get_bit_depth(pngPtr, infoPtr);

	fclose(fPtr);
	//png_destroy_read_struct(&pngPtr, &infoPtr, nullptr);

}

void writePNGFile(const char* fileName, png_bytepp& rowPointers, png_uint_32& width, png_uint_32& height, png_byte& colorType, png_byte& bitDepth)
{
	FILE* fPtr{ nullptr };
	fopen_s(&fPtr, fileName, "wb");

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

int main(int argc, char* argv[])
{
	png_bytepp rowPointers{ nullptr };
	png_uint_32 width{ 0 };
	png_uint_32 height{ 0 };
	png_byte colorType{};
	png_byte bitDepth{};
	if (argc > 1)
	{
		readPNGFile(argv[1], rowPointers, width, height, colorType, bitDepth);
		writePNGFile(argv[2], rowPointers, width, height, colorType, bitDepth);
	}

	

	return 0;
}