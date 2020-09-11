#ifndef __READFILE_H__
#define __READFILE_H__

#define DATA_OFFSET_OFFSET 0x000A
#define WIDTH_OFFSET 0x0012
#define HEIGHT_OFFSET 0x0016
#define BITS_PER_PIXEL_OFFSET 0x001C
#define HEADER_SIZE 14
#define INFO_HEADER_SIZE 40
#define NO_COMPRESION 0
#define MAX_NUMBER_OF_COLORS 0
#define ALL_COLORS_REQUIRED 0

#include<fstream>
#include<iostream>
#include<new>

typedef unsigned int int32;
typedef short int16;
typedef unsigned char byte;

class readFile
{
public:
	readFile();
	~readFile();
	void readImage(const char* fileName, byte** pixels, int32* width, int32* height, int32* bytesPerPixel);

private:

};

#endif // !
