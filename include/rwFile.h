#ifndef __RWFILE_H__
#define __RWFILE_H__

#pragma once
#include <fstream>
#include <vector>
#include <stdexcept>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

class BMP {
public: 

    BMP();
    ~BMP();
    
    void read(const char* fname);

    void newBMP(uint32_t width, uint32_t height, uint16_t bitSize);

    void write(const char* fname);

    // view file on disk
    void showImage(const char* fname, int readFlag);
    // view file in memory
    void showImage(char const* fname, cv::Mat image);

    void colour2grey();

    void add_alpha(uint8_t alpha);

    void overlay(const char* mask);

    void alphaOverlay(std::vector<uint8_t>& background,std::vector<uint8_t>& mask, uint32_t i, uint32_t pos);
    // set alpha chanel on mask for red threshold
    void setAlpha();

    void filter_channel(bool b, bool g, bool r);

    void fill_region(uint32_t x0, uint32_t y0, uint32_t w, uint32_t h, std::vector<uint8_t>& BGR, uint8_t alpha, bool convert24to32);

    void set_pixel(uint32_t x0, uint32_t y0, uint8_t B, uint8_t G, uint8_t R, uint8_t A);

    //void draw_rectangle(uint32_t x0, uint32_t y0, uint32_t w, uint32_t h,
    //    uint8_t B, uint8_t G, uint8_t R, uint8_t A, uint8_t line_w);

private:
    int8_t channels;
    int8_t redThreshold;
    
#pragma pack(push, 1)
    typedef struct BMPFileHeader {
        uint16_t file_type{ 0x4D42 };          // File type always BM which is 0x4D42 (stored as hex uint16_t in little endian)
        uint32_t file_size{ 0 };               // Size of the file (in bytes)
        uint16_t reserved1{ 0 };               // Reserved, always 0
        uint16_t reserved2{ 0 };               // Reserved, always 0
        uint32_t offset_data{ 0 };             // Start position of pixel data (bytes from the beginning of the file)
    };

    typedef struct BMPInfoHeader {
        uint32_t size{ 0 };                      // Size of this header (in bytes)
        int32_t width{ 0 };                      // width of bitmap in pixels
        int32_t height{ 0 };                     // width of bitmap in pixels
                                                 //       (if positive, bottom-up, with origin in lower left corner)
                                                 //       (if negative, top-down, with origin in upper left corner)
        uint16_t planes{ 1 };                    // No. of planes for the target device, this is always 1
        uint16_t bit_count{ 0 };                 // No. of bits per pixel
        uint32_t compression{ 0 };               // 0 or 3 - uncompressed. THIS PROGRAM CONSIDERS ONLY UNCOMPRESSED BMP images
        uint32_t size_image{ 0 };                // 0 - for uncompressed images
        int32_t x_pixels_per_meter{ 0 };
        int32_t y_pixels_per_meter{ 0 };
        uint32_t colors_used{ 0 };               // No. color indexes in the color table. Use 0 for the max number of colors allowed by bit_count
        uint32_t colors_important{ 0 };          // No. of colors used for displaying the bitmap. If 0 all colors are required
    };

    typedef struct BMPColorHeader {
        uint32_t red_mask{ 0x00ff0000 };         // Bit mask for the red channel
        uint32_t green_mask{ 0x0000ff00 };       // Bit mask for the green channel
        uint32_t blue_mask{ 0x000000ff };        // Bit mask for the blue channel
        uint32_t alpha_mask{ 0xff000000 };       // Bit mask for the alpha channel
        uint32_t color_space_type{ 0x73524742 }; // Default "sRGB" (0x73524742)
        uint32_t unused[16]{ 0 };                // Unused data for sRGB color space
    };

    //typedef struct RGBQUAD {
    //    uint8_t rgbBlue{ 0 };
    //    uint8_t rgbGreen{ 0 };
    //    uint8_t rgbRed{ 0 };
    //    uint8_t rgbReserved{ 0 };
    //};

    //typedef struct BMP8bitColourTable {
    //    RGBQUAD bmiColors[1];
    //};

#pragma pack(pop)

    BMPFileHeader file_header;
    BMPInfoHeader bmp_info_header;
    BMPColorHeader bmp_color_header;
    //BMP8bitColourTable bmp_8bit_palette[256];
    
    std::vector<uint8_t> data;
    uint8_t colourTable[256 * 4] = { 0 };

    uint32_t row_stride{ 0 };

    void BMPFileParaReset();

    void write_headers(std::ofstream& of);

    void write_headers_and_data(std::ofstream& of);

    // Add 1 to the row_stride until it is divisible with align_stride
    uint32_t make_stride_aligned(uint32_t align_stride);

    // Check if the pixel data is stored as BGRA and if the color space type is sRGB
    void check_color_header(BMPColorHeader& bmp_color_header);
};

#endif // !__RWFILE_H__
