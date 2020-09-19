#include <iostream>
#include <fstream>
#include <complex>
#include <vector>
#include "msu_reader.h"
#include "simpledeframer.h"

// Return filesize
size_t getFilesize(std::string filepath)
{
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    std::size_t fileSize = file.tellg();
    file.close();
    return fileSize;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage : " << argv[0] << " inputFrames.bin" << std::endl;
        return 0;
    }

    // Complete filesize
    size_t filesize = getFilesize(argv[1]);

    // Output and Input file
    std::ifstream data_in(argv[1], std::ios::binary);

    // Read buffer
    uint8_t buffer[1024];

    // Deframers and decoders
    MSUReader msuReader1, msuReader2, msuReader3;
    SimpleDeframer<uint64_t, 64, 121680, 0x0218a7a392dd9abf> msuDefra1, msuDefra2, msuDefra3;

    // Counters
    int channe1_frames = 0, channe2_frames = 0, channe3_frames = 0;

    // Graphics
    std::cout << "---------------------------" << std::endl;
    std::cout << "ELEKTRO-L Decoder by Aang23" << std::endl;
    std::cout << "---------------------------" << std::endl;
    std::cout << std::endl;

    std::cout << "Demultiplexing and deframing..." << std::endl;

    // Read until EOF
    while (!data_in.eof())
    {
        // Read buffer
        data_in.read((char *)buffer, 1024);

        // Get channel
        int vcid = buffer[5];

        // Check channel
        if (vcid == 85)
        {
            // Extract content, deframe, and push into decoder
            std::vector<uint8_t> defraVec;
            defraVec.insert(defraVec.end(), &buffer[24], &buffer[24] + (1024 - 24));
            std::vector<std::vector<uint8_t>> msu = msuDefra1.work(defraVec);
            channe1_frames += msu.size();
            for (std::vector<uint8_t> &frame : msu)
                msuReader1.pushFrame(&frame[0]);
        }
        else if (vcid == 90)
        {
            // Extract content, deframe, and push into decoder
            std::vector<uint8_t> defraVec;
            defraVec.insert(defraVec.end(), &buffer[24], &buffer[24] + (1024 - 24));
            std::vector<std::vector<uint8_t>> msu = msuDefra2.work(defraVec);
            channe2_frames += msu.size();
            for (std::vector<uint8_t> &frame : msu)
                msuReader2.pushFrame(&frame[0]);
        }
        else if (vcid == 102)
        {
            // Extract content, deframe, and push into decoder
            std::vector<uint8_t> defraVec;
            defraVec.insert(defraVec.end(), &buffer[24], &buffer[24] + (1024 - 24));
            std::vector<std::vector<uint8_t>> msu = msuDefra3.work(defraVec);
            channe3_frames += msu.size();
            for (std::vector<uint8_t> &frame : msu)
                msuReader3.pushFrame(&frame[0]);
        }

        // Show our progress
        std::cout << "\rProgress : " << round(((float)data_in.tellg() / (float)filesize) * 1000.0f) / 10.0f << "%     " << std::flush;
    }
    std::cout << std::endl;
    std::cout << std::endl;

    // Say what we found
    std::cout << "Channel 1 lines : " << channe1_frames << std::endl;
    std::cout << "Channel 2 lines : " << channe2_frames << std::endl;
    std::cout << "Channel 3 lines : " << channe3_frames << std::endl;

    std::cout << std::endl;

    // Write images out
    std::cout << "Writing images..." << std::endl;

    cimg_library::CImg<unsigned short> image1 = msuReader1.getImage();
    cimg_library::CImg<unsigned short> image2 = msuReader2.getImage();
    cimg_library::CImg<unsigned short> image3 = msuReader3.getImage();

    // Takes a while so we say how we're doing
    std::cout << "Channel 1..." << std::endl;
    image1.save_png("Elektro-1.png");

    std::cout << "Channel 2..." << std::endl;
    image2.save_png("Elektro-2.png");

    std::cout << "Channel 3..." << std::endl;
    image3.save_png("Elektro-3.png");

    data_in.close();
}