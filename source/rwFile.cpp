#include "rwFile.h"

BMP::BMP() {
    channels = 0;
    inpChannels = 0;
    redThreshold = 40;
    bwThreshold = 25;
}

BMP::~BMP() {
}

void BMP::read(const char* fname) {
    std::ifstream inp{ fname, std::ios_base::binary };
    if (inp) {
        inp.read((char*)&file_header, sizeof(file_header));
        
        if (file_header.file_type != 0x4D42)
            throw std::runtime_error("Error! Unrecognized file format.");
        
        inp.read((char*)&bmp_info_header, sizeof(bmp_info_header));

        channels = bmp_info_header.bit_count / 8;

        // The BMPColorHeader is used only for transparent images
        switch (bmp_info_header.bit_count) {
        
        case 32: {
            // Check if the file has bit mask color information
            if (bmp_info_header.size >= (sizeof(BMPInfoHeader) + sizeof(BMPColorHeader))) {
                inp.read((char*)&bmp_color_header, sizeof(bmp_color_header));
                // Check if the pixel data is stored as BGRA and if the color space type is sRGB
                check_color_header(bmp_color_header);
            }
            else {
                std::cerr << "Error! The file \"" << fname << "\" does not seem to contain bit mask information\n";
                throw std::runtime_error("Error! Unrecognized file format.");
            }
            break; 
        }
        
        case 24: {
            break;
        }
        
        case 8: {
            inp.read((char*)bmp_8bit_table, sizeof(bmp_8bit_table));
            break;
        }
        
        case 4: {
            inp.read((char*)&bmp_4bit_table, sizeof(bmp_4bit_table));
            break;
        }
        
        case 1: {
            inp.read((char*)&bmp_1bit_table, sizeof(bmp_1bit_table));
            break;
        }
        
        }

        // Jump to the pixel data location
        inp.seekg(file_header.offset_data, inp.beg);

        // Adjust the header fields for output.
        // Some editors will put extra info in the image file, we only save the headers and the data.
        bmp_info_header.size = sizeof(BMPInfoHeader);
        file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);

        switch (bmp_info_header.bit_count) {
        case 32:
            bmp_info_header.size += sizeof(BMPColorHeader);
            file_header.offset_data += sizeof(BMPColorHeader);
            break;
        case 24:
            break;
        case 8:
            file_header.offset_data += sizeof(bmp_8bit_table);
            break;
        case 4:
            file_header.offset_data += sizeof(bmp_4bit_table);
            break;
        case 1:
            file_header.offset_data += sizeof(bmp_1bit_table);
            break;
        default:
            throw std::runtime_error("Unsupported image bit size");
            break;
        }

        file_header.file_size = file_header.offset_data;

        if (bmp_info_header.height < 0)
            throw std::runtime_error("The program can treat only BMP images with the origin in the bottom left corner!");

        // Here we check if we need to take into account row padding
        switch (bmp_info_header.bit_count) {
        
        case 32: {
            data.resize((size_t)bmp_info_header.width * bmp_info_header.height * channels);
            inp.read((char*)data.data(), data.size());
            file_header.file_size += static_cast<uint32_t>(data.size());
            break;
        }
        
        case 24:
        case 8: {
            data.resize((size_t)bmp_info_header.width * bmp_info_header.height * channels);
            if (bmp_info_header.width % 4 == 0) {
                inp.read((char*)data.data(), data.size());
                file_header.file_size += static_cast<uint32_t>(data.size());
            }
            else {
                row_stride = bmp_info_header.width * channels;
                make_stride_aligned();
                std::vector<uint8_t> padding_row(new_stride - row_stride);

                for (int y = 0; y < bmp_info_header.height; ++y) {
                    inp.read((char*)(data.data() + (size_t)row_stride * y), row_stride);
                    inp.read((char*)padding_row.data(), padding_row.size());
                }
                file_header.file_size += static_cast<uint32_t>(data.size()) + bmp_info_header.height * static_cast<uint32_t>(padding_row.size());
            }
            break;
        }
        
        case 4: {
            data.resize(bmp_info_header.width * bmp_info_header.height);
            if (bmp_info_header.width % 16 == 0) {
                inp.read((char*)data.data(), data.size());
                file_header.file_size += static_cast<uint32_t>(data.size());
            }
            else {
                row_stride = bmp_info_header.width;
                make_stride_aligned();
                std::vector<uint8_t> padding_row(new_stride - row_stride);

                for (int y = 0; y < bmp_info_header.height; ++y) {
                    inp.read((char*)(data.data() + (size_t)row_stride * y), row_stride);
                    inp.read((char*)padding_row.data(), padding_row.size());
                }
                file_header.file_size += static_cast<uint32_t>(data.size()) + bmp_info_header.height * static_cast<uint32_t>(padding_row.size());
            }
            break;
        }
        
        case 1: {
            //bit1.resize(bmp_info_header.width * bmp_info_header.height / 8);
            if (bmp_info_header.width % 4 == 0) {
                inp.read((char*)data.data(), data.size());
                file_header.file_size += static_cast<uint32_t>(data.size());
            }
            else {
                row_stride = bmp_info_header.width / 8;
                new_stride = 4 * ((row_stride * bmp_info_header.bit_count + 31) / 32);
                std::vector<uint8_t>copyData(new_stride* bmp_info_header.height);
                inp.read((char*)(copyData.data()), (new_stride * bmp_info_header.height));
                data.resize(bmp_info_header.width * bmp_info_header.height / 8, 0);
                //for (int y = 0; y < bmp_info_header.height; ++y) {
                //    for (int x = 0; x < row_stride; ++x) {
                //        data[(y * row_stride) + x] = copyData[(y * new_stride) + x];
                //    }
                //    switch (row_stride % 8) {

                //    case 1: {
                //        data[((y + 1) * row_stride)] |= (copyData[((y + 1) * row_stride)] & 0x80);
                //        break;
                //    }
                //    case 2:
                //        data[((y + 1) * row_stride)] |= (copyData[((y + 1) * row_stride)] & 0xC0);
                //        break;
                //case 3:
                //    data[((y + 1) * row_stride)] |= (copyData[((y + 1) * row_stride)] & 0xE0);
                //    break;
                //case 4:
                //    data[((y + 1) * row_stride)] |= (copyData[((y + 1) * row_stride)] & 0xF0);
                //    break;
                //    }

                //}

                //file_header.file_size += static_cast<uint32_t>(data.size()) + bmp_info_header.height * static_cast<uint32_t>(padding_row.size());

                //row_stride = bmp_info_header.width;
                //make_stride_aligned();
                //std::vector<uint8_t>padding_row(new_stride - row_stride);
                //data.resize(row_stride * bmp_info_header.height);
                //for (int y = 0; y < bmp_info_header.height; ++y) {
                //    inp.read((char*)(data.data() + (size_t)row_stride * y), row_stride);
                //    inp.read((char*)padding_row.data(), padding_row.size());
                //}
                //file_header.file_size += static_cast<uint32_t>(data.size()) + bmp_info_header.height * static_cast<uint32_t>(padding_row.size());
            }
            break;
        }
        
        }

        inp.close();
    }
    else
        throw std::runtime_error("Unable to open the input image file.");
    
    //std::cout << file_header.file_type << std::endl;          // File type always BM which is 0x4D42 (stored as hex uint16_t in little endian)
    //std::cout << file_header.file_size << std::endl;              // Size of the file (in bytes)
    //std::cout << file_header.reserved1 << std::endl;    // Reserved, always 0
    //std::cout << file_header.reserved2 << std::endl;    // Reserved, always 0
    //std::cout << file_header.offset_data << std::endl;    // Start position of pixel data (bytes from the beginning of the file)
    //
    //std::cout << bmp_info_header.size << std::endl;     // Size of this header (in bytes)
    //std::cout << bmp_info_header.width << std::endl;        // width of bitmap in pixels
    //std::cout << bmp_info_header.height << std::endl;       // width of bitmap in pixels
    //                                            //       (if positive, bottom-up, with origin in lower left corner)
    //                                            //       (if negative, top-down, with origin in upper left corner)
    //std::cout << bmp_info_header.planes << std::endl;      // No. of planes for the target device, this is always 1
    //std::cout << bmp_info_header.bit_count<< std::endl;     // No. of bits per pixel
    //std::cout << bmp_info_header.compression<< std::endl;   // 0 or 3 - uncompressed. THIS PROGRAM CONSIDERS ONLY UNCOMPRESSED BMP images
    //std::cout << bmp_info_header.size_image << std::endl;  // 0 - for uncompressed images
    //std::cout << bmp_info_header.x_pixels_per_meter << std::endl;
    //std::cout << bmp_info_header.y_pixels_per_meter << std::endl;
    //std::cout << bmp_info_header.colors_used << std::endl;// No. color indexes in the color table. Use 0 for the max number of colors allowed by bit_count
    //std::cout << bmp_info_header.colors_important << std::endl;       // No. of colors used for displaying the bitmap. If 0 all colors are required

}

void BMP::newBMP(uint32_t width, uint32_t height, uint16_t bitSize) {
    if (width <= 0 || height <= 0)
        throw std::runtime_error("The image width and height must be positive numbers.");
    BMPFileParaReset();
    bmp_info_header.size = sizeof(BMPInfoHeader);
    file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);
    bmp_info_header.width = width;
    bmp_info_header.height = height;
    switch (bitSize) {
    
    case 32: {
        bmp_info_header.bit_count = 32;
        channels = 4;
        row_stride = width * channels;
        data.resize((size_t)row_stride * height);
        bmp_info_header.size += sizeof(BMPColorHeader);
        file_header.offset_data += sizeof(BMPColorHeader);
        bmp_info_header.compression = 3;
        file_header.file_size = file_header.offset_data + (uint32_t)data.size();
        break;
    }

    case 24: {
        bmp_info_header.bit_count = 24;
        channels = 3;
        row_stride = width * channels;
        data.resize((size_t)row_stride * height);
        file_header.file_size = file_header.offset_data + (uint32_t)data.size();
        make_stride_aligned();
        if (new_stride != row_stride)
            file_header.file_size += bmp_info_header.height * (new_stride - row_stride);
        break;
    }
    
    case  8: {
        bmp_info_header.bit_count = 8;
        channels = 1;
        row_stride = width * channels;
        colourTable();
        file_header.offset_data += sizeof(bmp_8bit_table);
        file_header.file_size = file_header.offset_data + (row_stride* height);
        make_stride_aligned();
        if (new_stride != row_stride)
            file_header.file_size += bmp_info_header.height * (new_stride - row_stride);
        break;
    }
    
    case  4: {
        bmp_info_header.width = width;
        bmp_info_header.bit_count = 4;
        row_stride =  4 * ((width * bmp_info_header.bit_count + 31) / 32);
        colourTable();
        file_header.offset_data += sizeof(bmp_4bit_table);
        file_header.file_size = file_header.offset_data + (uint32_t)(row_stride * height);
        if (row_stride % 4 != 0) {
            make_stride_aligned();
            file_header.file_size += bmp_info_header.height * (new_stride - row_stride);
        }
        else
            new_stride = row_stride;
        break;
    }
    
    case  1: {
        bmp_info_header.bit_count = 1;
        row_stride = 4 * ((width * bmp_info_header.bit_count + 31) / 32);
        colourTable();
        file_header.offset_data += sizeof(bmp_1bit_table);
        file_header.file_size = file_header.offset_data + (row_stride * height);
        if (row_stride % 4 != 0) {
            make_stride_aligned();
            file_header.file_size += bmp_info_header.height * (new_stride - row_stride);
        }
        else
            new_stride = row_stride;
        break;
    }
    
    default:
        throw std::runtime_error("Bit size not supported.");
    }
}

void BMP::colour8bit() {
    if (bmp_info_header.bit_count > 8) {
        inpChannels = channels;
        //std::vector<uint8_t> copyData(data);
        newBMP(bmp_info_header.width, bmp_info_header.height, 8);
        colourAvg();
    }
    else
        std::cout << "Select 24 bit or 32 bit image" << std::endl;
}

void BMP::colour4bit() {
    if (bmp_info_header.bit_count > 8) {
        inpChannels = channels;
        std::vector<uint8_t> copyData(data);
        newBMP(bmp_info_header.width, bmp_info_header.height, 4);
        data.clear();
        data.resize(copyData.size() / inpChannels);
        // B*0.27, G*0.72, R*0.07
        for (int i = 0; i < copyData.size(); i += inpChannels)
            data[i / inpChannels] = ((copyData[i] * 21) + (copyData[i + 1] * 72) + (copyData[i + 2] * 7)) / 1500;
        copyData.resize(data.size());
        copyData = data;
        data.clear();
        data.resize(new_stride * bmp_info_header.height);
        int newPos = 0, oldPos = 0;
        for (int y = 0; y < bmp_info_header.height; y++) {
            oldPos = y * bmp_info_header.width;
            newPos = y * new_stride;
            //switch (bmp_info_header.width % 8) {
            //case 0: {
            //    for (int x = 0; x < row_stride; x++)
            //        data[newPos + x] = (copyData[oldPos + x] & 0x0F << 4) | copyData[oldPos + x + 1] & 0x0F;
            //    break;
            //}
            //case 1:{
            //    for (int x = 0; x < row_stride - 1; x++)
            //        data[newPos + x] = (copyData[oldPos + x] & 0x0F << 4) | copyData[oldPos + x + 1] & 0x0F;
            //    data[newPos + row_stride] = (copyData[oldPos + row_stride] & 0x0F) << 4;
            //    data[newPos + row_stride + 1] = data[newPos + row_stride + 2] = 0;
            //    break;
            //}
            //case 2: {
            //    for (int x = 0; x < row_stride; x++)
            //        data[newPos + x] = (copyData[oldPos + x] & 0x0F << 4) | copyData[oldPos + x + 1] & 0x0F;
            //    data[newPos + row_stride + 1] = data[newPos + row_stride + 2] = 0;
            //    break;
            //}
            //case 3: {
            //    for (int x = 0; x < row_stride-1; x++)
            //        data[newPos + x] = (copyData[oldPos + x] & 0x0F << 4) | copyData[oldPos + x + 1] & 0x0F;
            //    data[newPos + row_stride] = (copyData[oldPos + row_stride] & 0x0F) << 4;
            //    break;
            //}
            //
            //}
            
            for (int x = 0; x < bmp_info_header.width / 2; x++)
                data[newPos + x] = ((copyData[oldPos + x] & 0x0F) << 4) | (copyData[oldPos + x + 1] & 0x0F);
            switch (bmp_info_header.width % 2) {
            case 0: {
                for (int i = 0; i < row_stride - (bmp_info_header.width / 2); i++)
                    data[newPos + bmp_info_header.width / 2 + i] = 0;
                break;
            }

            case 1: {
                data[newPos + bmp_info_header.width / 2] = (copyData[oldPos + row_stride] & 0x0F) << 4;
                for (int i = 1; i < new_stride - (bmp_info_header.width / 2); i++)
                    data[newPos + bmp_info_header.width / 2 + i] = 0;
                break;
            }
            }

            //if (row_stride % 2 == 0) {
            //    for (int x = 0; x < row_stride; x++)
            //        data[newPos + x] = (copyData[oldPos + x] & 0x0F << 4) | copyData[oldPos + x + 1] & 0x0F;
            //    if (row_stride % 4 != 0)
            //        for (int i = 0; i < (new_stride - row_stride); i++)
            //            data[newPos + row_stride + i] = 0;
            //}
            //else {
            //    for (int x = 0; x < row_stride - 1; x++)
            //        data[newPos + x] = (copyData[oldPos + x] & 0x0F << 4) | copyData[oldPos + x + 1] & 0x0F;
            //    data[newPos + row_stride] = (copyData[oldPos + row_stride] & 0x0F) << 4;
            //    for (int i = 0; i < (new_stride - row_stride); i++)
            //        data[newPos + row_stride + 1] = 0;
            //}
        }
    }
    else
        std::cout << "Select 24 or 32 bit image" << std::endl;
}

void BMP::colour1bit() {
    if (bmp_info_header.bit_count > 7) {
        inpChannels = channels;
        if (inpChannels != 1)
            colourAvg();
        newBMP(bmp_info_header.width, bmp_info_header.height, 1);
        std::vector<uint8_t> copyData(data);
        data.clear();   
        data.resize(new_stride * bmp_info_header.height);
       
        //**//**//**
        for (int y = 0; y < bmp_info_header.height; y++) {
            for (int x = 0; x < bmp_info_header.width; x += 8) {
                for (int i = 0; (i < 8) && ((x + i) < bmp_info_header.width); i++) {
                    data[(y * row_stride) + (x / 8)] |=
                        (((copyData[(y * bmp_info_header.width) + x + i] < bwThreshold) ? 0 : 1) << (7 - i));
                }
            }
            if (row_stride - (bmp_info_header.width / 8) != 0) 
                    for (int i = 0; i < row_stride - (bmp_info_header.width / 8); i++)
                        data[(y * row_stride) + (bmp_info_header.width / 8) + i] = 0;
            else
                if (row_stride - (bmp_info_header.width / 8) != 0)
                    for (int i = 0; i < row_stride - (bmp_info_header.width / 8); i++)
                        data[(y * row_stride) + (bmp_info_header.width / 8 + 1) + i] = 0;
        }
        //**//**//**
    }
    else
        std::cout << "Select 8, 24 or 32 bit image" << std::endl;
}

void BMP::colourAvg() {
    if (bmp_info_header.bit_count > 7) {
        // B*0.27, G*0.72, R*0.07
        for (int i = 0; i < data.size(); i += inpChannels)
            data[i / inpChannels] = ((data[i] * 21) + (data[i + 1] * 72) + (data[i + 2] * 7)) / 100;
        data.resize(data.size() / inpChannels);
    }
}

void BMP::write(const char* fname) {
    std::ofstream of{ fname, std::ios::binary };
    if (of) {
        switch (bmp_info_header.bit_count) {

        case 32: {
            write_headers_and_data(of);
            break;
        }

        case 24:

        case 8: {
            if (bmp_info_header.width % 4 == 0)
                write_headers_and_data(of);
            else {
                make_stride_aligned();
                std::vector<uint8_t> padding_row(new_stride - row_stride);
                write_headers(of);
                for (int y = 0; y < bmp_info_header.height; ++y) {
                    of.write((const char*)(data.data() + (size_t)row_stride * y), row_stride);
                    of.write((const char*)padding_row.data(), padding_row.size());
                }
            }
            break;
        }

        case 4: {
            write_headers_and_data(of);
            break;
        }

        case 1: {
            if (bmp_info_header.width % 4 == 0)
                write_headers_and_data(of);
            else {
                make_stride_aligned();
                std::vector<uint8_t> padding_row(new_stride - row_stride);
                write_headers(of);
                for (int y = 0; y < bmp_info_header.height; ++y) {
                    of.write((const char*)(data.data() + (size_t)row_stride * y), row_stride);
                    of.write((const char*)padding_row.data(), padding_row.size());
                }
            }
            break;
        }

        default: {
            throw std::runtime_error("The program can treat only 4, 8, 24 or 32 bits per pixel");
            break;
        }
        }
        of.flush();
    }
    else
        throw std::runtime_error("Unable to open the output image file.");
}

void BMP::add_alpha(uint8_t alpha) {
    if (channels != 3)
        throw std::runtime_error("24 bit image required to give alpha");
    else {
        std::vector<uint8_t> copyData(data);
        newBMP(bmp_info_header.width, bmp_info_header.height, 32);
        fill_region(0, 0, bmp_info_header.width, bmp_info_header.height, copyData, alpha, true);
    }
}

void BMP::overlay(const char* mask) {
    BMP readMask;
    readMask.read(mask);
    uint32_t i = 0;
    uint32_t dataPos = 0;
    uint32_t x_offset = rand() % (bmp_info_header.width - readMask.bmp_info_header.width - 1);
    uint32_t y_offset = rand() % (bmp_info_header.height - readMask.bmp_info_header.height - 1);
    uint8_t alpha = 255;
    if (channels == 3)
        alpha = 0;
    for (uint32_t y = y_offset; y < y_offset + readMask.bmp_info_header.height; y++) {
        for (uint32_t x = x_offset; x < x_offset + readMask.bmp_info_header.width; x++) {
            dataPos = channels * (y * bmp_info_header.width + x);
            i = channels * ((y - y_offset) * readMask.bmp_info_header.width + (x - x_offset));;
            if (channels == 1)
                if(readMask.data[i] > redThreshold)
                    data[dataPos] = readMask.data[i];
            if(channels != 1) {
                if (readMask.data[i] + readMask.data[(size_t)i + 1] + readMask.data[(size_t)i + 2] != 255 * 3) {
                    if (channels == 4) {
                        alphaOverlay(data, readMask.data, i, dataPos);
                        alpha = readMask.data[(size_t)i + 3];
                    }
                    set_pixel(x, y, readMask.data[i + 0], readMask.data[(size_t)i + 1], readMask.data[(size_t)i + 2], alpha);
                }
            }
        }
    }
}

void BMP::alphaOverlay(std::vector<uint8_t>& background, std::vector<uint8_t>& mask, uint32_t i, uint32_t pos) {
    uint8_t alpha = 255;
    // alpha blending
    alpha = mask[(size_t)i + 3] + background[(size_t)pos + 3] * (255 - mask[(size_t)i + 3])/255;

    // alpha blending colour
    for (int j = 0; j < 3; j++)
        mask[(size_t)i + j] = ((mask[(size_t)i + j] * mask[(size_t)i + 3]) + background[(size_t)pos+ j] * background[(size_t)pos + 3] * (255 - mask[(size_t)i + 3])/255) / alpha;

    mask[(size_t)i + 3] = alpha;
}

void BMP::setAlpha() {
    if (channels != 4)
        throw std::runtime_error("Image does not have alpha");
    for (uint32_t i = 2; i < data.size(); i += 4) {
        if (data[i] < redThreshold)
            data[(size_t)i + 1] = 0;
        else
            data[(size_t)i + 1] = 255;
    }
}

void BMP::filter_channel(bool b, bool g, bool r) {
    if (b == false)
        for (int32_t i = 0; i < data.size(); i += channels)
            data[i] = 0;
    if (g == false)
        for (int32_t i = 1; i < data.size(); i += channels)
            data[i] = 0;
    if (r == false)
        for (int32_t i = 2; i < data.size(); i += channels)
            data[i] = 0;
}

//void BMP::draw_rectangle(uint32_t x0, uint32_t y0, uint32_t w, uint32_t h,
//    uint8_t B, uint8_t G, uint8_t R, uint8_t A, uint8_t line_w) {
//    if (x0 + w > (uint32_t)bmp_info_header.width || y0 + h > (uint32_t)bmp_info_header.height) {
//        throw std::runtime_error("The rectangle does not fit in the image!");
//    }
//
//    fill_region(x0, y0, w, line_w, B, G, R, A);                                             // top line
//    fill_region(x0, (y0 + h - line_w), w, line_w, B, G, R, A);                              // bottom line
//    fill_region((x0 + w - line_w), (y0 + line_w), line_w, (h - (2 * line_w)), B, G, R, A);  // right line
//    fill_region(x0, (y0 + line_w), line_w, (h - (2 * line_w)), B, G, R, A);                 // left line
//}

void BMP::BMPFileParaReset() {
    file_header.file_type = 0x4D42;          // File type always BM which is 0x4D42 (stored as hex uint16_t in little endian)
    file_header.file_size = 0;               // Size of the file (in bytes)
    file_header.reserved1 = 0;               // Reserved, always 0
    file_header.reserved2 = 0;               // Reserved, always 0
    file_header.offset_data = 0;             // Start position of pixel data (bytes from the beginning of the file)

    bmp_info_header.size = 0;                   // Size of this header (in bytes)
    bmp_info_header.width = 0;                  // width of bitmap in pixels
    bmp_info_header.height = 0;                 // width of bitmap in pixels
                                                //       (if positive, bottom-up, with origin in lower left corner)
                                                //       (if negative, top-down, with origin in upper left corner)
    bmp_info_header.planes = 1;                 // No. of planes for the target device, this is always 1
    bmp_info_header.bit_count = 0;              // No. of bits per pixel
    bmp_info_header.compression = 0;            // 0 or 3 - uncompressed. THIS PROGRAM CONSIDERS ONLY UNCOMPRESSED BMP images
    bmp_info_header.size_image = 0;             // 0 - for uncompressed images
    bmp_info_header.x_pixels_per_meter = 0;
    bmp_info_header.y_pixels_per_meter = 0;
    bmp_info_header.colors_used = 0;            // No. color indexes in the color table. Use 0 for the max number of colors allowed by bit_count
    bmp_info_header.colors_important = 0;       // No. of colors used for displaying the bitmap. If 0 all colors are required
}

void BMP::fill_region(uint32_t x0, uint32_t y0, uint32_t w, uint32_t h, std::vector<uint8_t>& BGR, uint8_t alpha, bool convert24to32) {
    uint32_t pos = 0, increment = 0;
    if (convert24to32)
        increment = channels - 1;
    for (uint32_t y = y0; y < y0 + h; ++y) {
        for (uint32_t x = x0; x < x0 + w; x++) {
            pos = increment * (y * bmp_info_header.width + x);
            set_pixel(x, y, BGR[pos], BGR[(size_t)pos + 1], BGR[(size_t)pos + 2], alpha);
        }
    }
}

void BMP::set_pixel(uint32_t x0, uint32_t y0, uint8_t B, uint8_t G, uint8_t R, uint8_t A) {
    if (x0 > (uint32_t)bmp_info_header.width || y0 > (uint32_t)bmp_info_header.height) {
        throw std::runtime_error("The point is outside the image boundaries!");
    }
    uint32_t pos = channels * (y0 * bmp_info_header.width + x0);
    data[pos + 0] = B;
    data[(size_t)pos + 1] = G;
    data[(size_t)pos + 2] = R;
    if (channels == 4) {
        data[(size_t)pos + 3] = A;
    }
}

void BMP::colourTable() {
    switch (bmp_info_header.bit_count) {

    case 8: {
        for (int i = 0; i < 256; i++)
            bmp_8bit_table[i].rgbBlue = bmp_8bit_table[i].rgbGreen = bmp_8bit_table[i].rgbRed = i;
        break;
    }

    case 4: {
        for (int i = 0; i < 16; i++)
            bmp_4bit_table[i].rgbBlue =
            bmp_4bit_table[i].rgbGreen =
            bmp_4bit_table[i].rgbRed = i * 255 / 15;
        break;
    }

    case 1: {
        // colour table
        bmp_1bit_table[0] = { 0,0,0,0 };
        bmp_1bit_table[1] = { 255,255,255,0 };
        break;
    }
    }
}

void BMP::write_headers(std::ofstream& of) {
    of.write((const char*)&file_header, sizeof(file_header));
    of.write((const char*)&bmp_info_header, sizeof(bmp_info_header));
    switch (bmp_info_header.bit_count) {
    case 32:
        of.write((const char*)&bmp_color_header, sizeof(bmp_color_header));
        break;
    case 24:
        break;
    case 8:
        of.write((const char*)&bmp_8bit_table, sizeof(bmp_8bit_table));
        break;
    case 4:
        of.write((const char*)&bmp_4bit_table, sizeof(bmp_4bit_table));
        break;
    case 1:
        of.write((const char*)&bmp_1bit_table, sizeof(bmp_1bit_table));
    }
}

void BMP::write_headers_and_data(std::ofstream& of) {
    write_headers(of);
    of.write((const char*)data.data(), data.size());
}

void BMP::make_stride_aligned() {
    switch (bmp_info_header.bit_count) {
    case 24:
    case 16:
    case 8: 
    case 4: {
        new_stride = row_stride;
        while (new_stride % 4 != 0)
            new_stride++;
        break;
    }

    case 1: {
        int Bytes = 0, padding = 0;
        if ((bmp_info_header.width % 8) != 0)
            Bytes = bmp_info_header.width / 8;
        else
            Bytes = bmp_info_header.width / 8 + 1;
        if ((Bytes % 4) != 0)
            padding = 4 - (Bytes % 4);
        else
            padding = 0;
        new_stride = Bytes + padding;
        break;
    }

    }
}

void BMP::check_color_header(BMPColorHeader& bmp_color_header) {
    BMPColorHeader expected_color_header;
    if (expected_color_header.red_mask != bmp_color_header.red_mask ||
        expected_color_header.blue_mask != bmp_color_header.blue_mask ||
        expected_color_header.green_mask != bmp_color_header.green_mask ||
        expected_color_header.alpha_mask != bmp_color_header.alpha_mask) {
        throw std::runtime_error("Unexpected color mask format! The program expects the pixel data to be in the BGRA format");
    }
    if (expected_color_header.color_space_type != bmp_color_header.color_space_type) {
        throw std::runtime_error("Unexpected color space type! The program expects sRGB values");
    }
}

void BMP::showImage(const char* fname) {
    cv::Mat image;
    image = cv::imread(fname, cv::IMREAD_ANYCOLOR);
    cv::namedWindow(fname, cv::WINDOW_AUTOSIZE);
    cv::imshow(fname, image);
    cv::waitKey();
}

void BMP::showImage(char const* fname, cv::Mat image) {
    cv::namedWindow(fname, cv::WINDOW_AUTOSIZE);
    cv::imshow(fname, image);
    cv::waitKey();
}
