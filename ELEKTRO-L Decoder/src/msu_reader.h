#pragma once

#include <cstdint>
#define cimg_use_png
#define cimg_display 0
#include "CImg.h"

class MSUReader
{
private:
    unsigned short *imageBuffer;
    unsigned short msuLineBuffer[12044];
    int frames;

public:
    MSUReader();
    ~MSUReader();
    void pushFrame(uint8_t *data);
    cimg_library::CImg<unsigned short> getImage();
};