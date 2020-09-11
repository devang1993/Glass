#include"../include/readFile.h"

readFile::readFile()
{
}

readFile::~readFile()
{
}

void readFile::readImage(const char* fileName, byte** pixels, int32* width, int32* height, int32* bytesPerPixel)
{
    FILE* imageFile;
    fopen_s(&imageFile, fileName, "rb");
    if (imageFile)
    {
        std::cout << "read image successful" << std::endl;
        int32 dataOffset;
        fseek(imageFile, DATA_OFFSET_OFFSET, SEEK_SET);
        fread(&dataOffset, 4, 1, imageFile);
        fseek(imageFile, WIDTH_OFFSET, SEEK_SET);
        fread(width, 4, 1, imageFile);
        fseek(imageFile, HEIGHT_OFFSET, SEEK_SET);
        fread(height, 4, 1, imageFile);
        int16 bitsPerPixel;
        fseek(imageFile, BITS_PER_PIXEL_OFFSET, SEEK_SET);
        fread(&bitsPerPixel, 2, 1, imageFile);
        *bytesPerPixel = ((int32)bitsPerPixel) / 8;

        int32 paddedRowSize = (int32)(4 * ceil((float)(*width) / 4.0f)) * (*bytesPerPixel);
        int32 unpaddedRowSize = (*width) * (*bytesPerPixel);
        int32 totalSize = unpaddedRowSize * (*height);
        *pixels = (byte*)malloc(totalSize);
        byte* currentRowPointer = *pixels + ((*height - 1) * unpaddedRowSize);
        for (int32 i = 0; i < *height; i++)
        {
            fseek(imageFile, dataOffset + (i * paddedRowSize), SEEK_SET);
            fread(currentRowPointer, 1, unpaddedRowSize, imageFile);
            currentRowPointer -= unpaddedRowSize;
        }

        fclose(imageFile);
        std::cout << *pixels << std::endl;
        std::cout << *width << std::endl;
        std::cout << *height << std::endl;
        std::cout << *bytesPerPixel << std::endl;

    }
    else
        std::cout << "read image failed" << std::endl;
}