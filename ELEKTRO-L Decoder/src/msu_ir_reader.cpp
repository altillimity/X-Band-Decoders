#include "msu_ir_reader.h"
#include <algorithm>

MSUReaderIR::MSUReaderIR()
{
    //imageBufferRaw = new unsigned short[500000 * 1108];
    imageBuffer1 = new unsigned short[500000 * 184];
    finalImageBuffer1 = new unsigned short[3388 * 3388];
    imageBuffer2 = new unsigned short[500000 * 184];
    finalImageBuffer2 = new unsigned short[3388 * 3388];
    imageBuffer3 = new unsigned short[500000 * 184];
    finalImageBuffer3 = new unsigned short[3388 * 3388];
    imageBuffer4 = new unsigned short[500000 * 184];
    finalImageBuffer4 = new unsigned short[3388 * 3388];
    imageBuffer5 = new unsigned short[500000 * 184];
    finalImageBuffer5 = new unsigned short[3388 * 3388];
    imageBuffer6 = new unsigned short[500000 * 184];
    finalImageBuffer6 = new unsigned short[3388 * 3388];
    frames = 0;
}

MSUReaderIR::~MSUReaderIR()
{
    //delete[] imageBufferRaw;
    delete[] imageBuffer1;
    delete[] finalImageBuffer1;
    delete[] imageBuffer2;
    delete[] finalImageBuffer2;
    delete[] imageBuffer3;
    delete[] finalImageBuffer3;
    delete[] imageBuffer4;
    delete[] finalImageBuffer4;
    delete[] imageBuffer5;
    delete[] finalImageBuffer5;
    delete[] imageBuffer6;
    delete[] finalImageBuffer6;
}

void MSUReaderIR::pushFrame(uint8_t *data)
{
    // Offset to start reading from
    int pos = 5 * 20;

    // Convert to 10 bits values
    for (int i = 0; i < 1108; i += 4)
    {
        msuLineBuffer[i] = (data[pos + 0] << 2) | (data[pos + 1] >> 6);
        msuLineBuffer[i + 1] = ((data[pos + 1] % 64) << 4) | (data[pos + 2] >> 4);
        msuLineBuffer[i + 2] = ((data[pos + 2] % 16) << 6) | (data[pos + 3] >> 2);
        msuLineBuffer[i + 3] = ((data[pos + 3] % 4) << 8) | data[pos + 4];
        pos += 5;
    }

    /*
    // Deinterleave and load into our image buffer (raw)
    for (int i = 0; i < 1108; i++)
    {
        uint16_t pixel = msuLineBuffer[0 + i];
        imageBufferRaw[frames * 1108 + i] = pixel * 60;
    }
    */

    // Deinterleave and load into our image buffer
    for (int i = 0; i < 184; i++)
    {
        uint16_t pixel = msuLineBuffer[37 + i];
        imageBuffer1[frames * 184 + i] = pixel * 60;
    }
    for (int i = 0; i < 184; i++)
    {
        uint16_t pixel = msuLineBuffer[229 + i];
        imageBuffer2[frames * 184 + i] = pixel * 60;
    }
    for (int i = 0; i < 184; i++)
    {
        uint16_t pixel = msuLineBuffer[420 + i];
        imageBuffer3[frames * 184 + i] = pixel * 60;
    }
    for (int i = 0; i < 184; i++)
    {
        uint16_t pixel = msuLineBuffer[612 + i];
        imageBuffer4[frames * 184 + i] = pixel * 60;
    }
    for (int i = 0; i < 184; i++)
    {
        uint16_t pixel = msuLineBuffer[804 + i];
        imageBuffer5[frames * 184 + i] = pixel * 60;
    }
    for (int i = 0; i < 184; i++)
    {
        uint16_t pixel = msuLineBuffer[997 + i];
        imageBuffer6[frames * 184 + i] = pixel * 60;
    }

    frames++;
}

/*
cimg_library::CImg<unsigned short> MSUReaderIR::getImageRaw()
{
    return cimg_library::CImg<unsigned short>(&imageBufferRaw[0], 1108, frames);
}
*/

cimg_library::CImg<unsigned short> MSUReaderIR::getImage1()
{
    // Transform this mess into a full-disk!
    for (int i = 0; i < 3388; i++)
    {
        for (int ii = 0; ii < 16; ii++)
        {
            std::memcpy(&finalImageBuffer1[3388 * i + (ii)*184], &imageBuffer1[(10100 + (ii * 2) * 3401 + i) * 184], 184 * 2);
            //std::memcpy(&finalImageBuffer1[((11008 * 3388) - (11008 * i)) + (ii * 2 + 1) * 184], &imageBuffer1[(10219 + (ii * 2 + 1)* 3388 + i) * 184], 184 * 2);
        }
    }

    //return cimg_library::CImg<unsigned short>(&imageBuffer1[0], 184, frames);
    return cimg_library::CImg<unsigned short>(&finalImageBuffer1[0], 3388, 3388);
}

cimg_library::CImg<unsigned short> MSUReaderIR::getImage2()
{
    // Transform this mess into a full-disk!
    for (int i = 0; i < 3388; i++)
    {
        for (int ii = 0; ii < 16; ii++)
        {
            std::memcpy(&finalImageBuffer2[3388 * i + (ii)*184], &imageBuffer2[(10100 + (ii * 2) * 3401 + i) * 184], 184 * 2);
            //std::memcpy(&finalImageBuffer1[((11008 * 3388) - (11008 * i)) + (ii * 2 + 1) * 184], &imageBuffer1[(10219 + (ii * 2 + 1)* 3388 + i) * 184], 184 * 2);
        }
    }

    //return cimg_library::CImg<unsigned short>(&imageBuffer2[0], 184, frames);
    return cimg_library::CImg<unsigned short>(&finalImageBuffer1[0], 3388, 3388);
}

cimg_library::CImg<unsigned short> MSUReaderIR::getImage3()
{
    // Transform this mess into a full-disk!
    for (int i = 0; i < 3388; i++)
    {
        for (int ii = 0; ii < 16; ii++)
        {
            std::memcpy(&finalImageBuffer3[3388 * i + (ii)*184], &imageBuffer3[(10100 + (ii * 2) * 3401 + i) * 184], 184 * 2);
            //std::memcpy(&finalImageBuffer1[((11008 * 3388) - (11008 * i)) + (ii * 2 + 1) * 184], &imageBuffer1[(10219 + (ii * 2 + 1)* 3388 + i) * 184], 184 * 2);
        }
    }

    //return cimg_library::CImg<unsigned short>(&imageBuffer3[0], 184, frames);
    return cimg_library::CImg<unsigned short>(&finalImageBuffer3[0], 3388, 3388);
}

cimg_library::CImg<unsigned short> MSUReaderIR::getImage4()
{
    // Transform this mess into a full-disk!
    for (int i = 0; i < 3388; i++)
    {
        for (int ii = 0; ii < 16; ii++)
        {
            std::memcpy(&finalImageBuffer4[3388 * i + (ii)*184], &imageBuffer4[(10100 + (ii * 2) * 3401 + i) * 184], 184 * 2);
            //std::memcpy(&finalImageBuffer1[((11008 * 3388) - (11008 * i)) + (ii * 2 + 1) * 184], &imageBuffer1[(10219 + (ii * 2 + 1)* 3388 + i) * 184], 184 * 2);
        }
    }

    //return cimg_library::CImg<unsigned short>(&imageBuffer4[0], 184, frames);
    return cimg_library::CImg<unsigned short>(&finalImageBuffer4[0], 3388, 3388);
}

cimg_library::CImg<unsigned short> MSUReaderIR::getImage5()
{
    // Transform this mess into a full-disk!
    for (int i = 0; i < 3388; i++)
    {
        for (int ii = 0; ii < 16; ii++)
        {
            std::memcpy(&finalImageBuffer5[3388 * i + (ii)*184], &imageBuffer5[(10100 + (ii * 2) * 3401 + i) * 184], 184 * 2);
            //std::memcpy(&finalImageBuffer1[((11008 * 3388) - (11008 * i)) + (ii * 2 + 1) * 184], &imageBuffer1[(10219 + (ii * 2 + 1)* 3388 + i) * 184], 184 * 2);
        }
    }

    //return cimg_library::CImg<unsigned short>(&imageBuffer5[0], 184, frames);
    return cimg_library::CImg<unsigned short>(&finalImageBuffer5[0], 3388, 3388);
}

cimg_library::CImg<unsigned short> MSUReaderIR::getImage6()
{
    // Transform this mess into a full-disk!
    for (int i = 0; i < 3388; i++)
    {
        for (int ii = 0; ii < 16; ii++)
        {
            std::memcpy(&finalImageBuffer6[3388 * i + (ii)*184], &imageBuffer6[(10100 + (ii * 2) * 3401 + i) * 184], 184 * 2);
            //std::memcpy(&finalImageBuffer1[((11008 * 3388) - (11008 * i)) + (ii * 2 + 1) * 184], &imageBuffer1[(10219 + (ii * 2 + 1)* 3388 + i) * 184], 184 * 2);
        }
    }

    //return cimg_library::CImg<unsigned short>(&imageBuffer6[0], 184, frames);
    return cimg_library::CImg<unsigned short>(&finalImageBuffer6[0], 3388, 3388);
}
