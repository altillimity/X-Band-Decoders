#pragma once

#include <cstdint>
#define cimg_use_png
#define cimg_display 0
#include "CImg.h"

class MSUReaderIR
{
private:
    //unsigned short *imageBufferRaw;
    unsigned short *imageBuffer1;
    unsigned short *finalImageBuffer1;
    unsigned short *imageBuffer2;
    unsigned short *finalImageBuffer2;
    unsigned short *imageBuffer3;
    unsigned short *finalImageBuffer3;
    unsigned short *imageBuffer4;
    unsigned short *finalImageBuffer4;
    unsigned short *imageBuffer5;
    unsigned short *finalImageBuffer5;
    unsigned short *imageBuffer6;
    unsigned short *finalImageBuffer6;
    unsigned short msuLineBuffer[12044];
    int frames;

public:
    MSUReaderIR();
    ~MSUReaderIR();
    void pushFrame(uint8_t *data);
    //cimg_library::CImg<unsigned short> getImageRaw();
    cimg_library::CImg<unsigned short> getImage1();
    cimg_library::CImg<unsigned short> getImage2();
    cimg_library::CImg<unsigned short> getImage3();
    cimg_library::CImg<unsigned short> getImage4();
    cimg_library::CImg<unsigned short> getImage5();
    cimg_library::CImg<unsigned short> getImage6();
};