#include <iostream>
#include <fstream>
#include <complex>
#include <vector>
#include "msu_reader.h"
#include "msu_ir_reader.h"
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
    MSUReaderIR msuReaderIR;
    SimpleDeframer<uint64_t, 64, 121680, 0x0218a7a392dd9abf> msuDefra1, msuDefra2, msuDefra3; // High-res Channels
    SimpleDeframer<uint64_t, 64, 14560, 0x0218a7a392dd9abf> msuDefra4;                        // Low-res channels

    // Counters
    int channe1_frames = 0, channe2_frames = 0, channe3_frames = 0, channe4_frames = 0;

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
        else if (vcid == 105)
        {
            // Extract content, deframe, and push into decoder
            std::vector<uint8_t> defraVec;
            defraVec.insert(defraVec.end(), &buffer[24], &buffer[24] + (1024 - 24));
            std::vector<std::vector<uint8_t>> msu = msuDefra4.work(defraVec);
            channe4_frames += msu.size();
            for (std::vector<uint8_t> &frame : msu)
                msuReaderIR.pushFrame(&frame[0]);
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
    std::cout << "Low-res channels frames : " << channe4_frames << std::endl;

    std::cout << std::endl;

    // Write images out
    std::cout << "Writing images... (Can take a while)" << std::endl;

    cimg_library::CImg<unsigned short> image1 = msuReader1.getImage();
    cimg_library::CImg<unsigned short> image2 = msuReader2.getImage();
    cimg_library::CImg<unsigned short> image3 = msuReader3.getImage();
    cimg_library::CImg<unsigned short> image4 = msuReaderIR.getImage1();
    cimg_library::CImg<unsigned short> image5 = msuReaderIR.getImage2();
    cimg_library::CImg<unsigned short> image6 = msuReaderIR.getImage3();
    cimg_library::CImg<unsigned short> image7 = msuReaderIR.getImage4();
    cimg_library::CImg<unsigned short> image8 = msuReaderIR.getImage5();
    cimg_library::CImg<unsigned short> image9 = msuReaderIR.getImage6();
    //cimg_library::CImg<unsigned short> imageIRRaw = msuReaderIR.getImageRaw();

    //std::cout << "Channel RAW IR..." << std::endl;
    //imageIRRaw.save_png("Elektro-RAW-IR.png");

    std::cout << "321 Composite..." << std::endl;
    cimg_library::CImg<unsigned short> image321(12008, 12008, 1, 3);
    image321.draw_image(0, 1774 - 4400, 0, 0, image3);
    image321.draw_image(-31, -23 - 4400, 0, 1, image2);
    image321.draw_image(22, 3574 - 4400, 0, 2, image1);
    image321.mirror('X');
    image321.mirror('Y');
    image321.save_png("Elektro-RGB-321.png");

    std::cout << "221 Composite..." << std::endl;
    cimg_library::CImg<unsigned short> image221(12008, 12008, 1, 3);
    image221.draw_image(-31, -23 - 4400, 0, 0, image2);
    image221.draw_image(-31, -23 - 4400, 0, 1, image2);
    image221.draw_image(22, 3574 - 4400, 0, 2, image1);
    image221.mirror('X');
    image221.mirror('Y');
    image221.save_png("Elektro-RGB-221.png");

    std::cout << "332 Composite..." << std::endl;
    cimg_library::CImg<unsigned short> image332(12008, 12008, 1, 3);
    image332.draw_image(0, 1774 - 4400, 0, 0, image3);
    image332.draw_image(0, 1774 - 4400, 0, 1, image3);
    image332.draw_image(-31, -23 - 4400, 0, 2, image2);
    image332.mirror('X');
    image332.mirror('Y');
    image332.save_png("Elektro-RGB-332.png");

    // Flip
    image1.mirror('X');
    image2.mirror('X');
    image3.mirror('X');
    image4.mirror('X');
    image5.mirror('X');
    image6.mirror('X');
    image7.mirror('X');
    image8.mirror('X');
    image9.mirror('X');
    image1.mirror('Y');
    image2.mirror('Y');
    image3.mirror('Y');
    image4.mirror('Y');
    image5.mirror('Y');
    image6.mirror('Y');
    image7.mirror('Y');
    image8.mirror('Y');
    image9.mirror('Y');

    // Takes a while so we say how we're doing
    std::cout << "Channel 1..." << std::endl;
    image1.save_png("Elektro-1.png");

    std::cout << "Channel 2..." << std::endl;
    image2.save_png("Elektro-2.png");

    std::cout << "Channel 3..." << std::endl;
    image3.save_png("Elektro-3.png");

    std::cout << "Channel 4..." << std::endl;
    image4.save_png("Elektro-4.png");

    std::cout << "Channel 5..." << std::endl;
    image5.save_png("Elektro-5.png");

    std::cout << "Channel 6..." << std::endl;
    image6.save_png("Elektro-6.png");

    std::cout << "Channel 7..." << std::endl;
    image7.save_png("Elektro-7.png");

    std::cout << "Channel 8..." << std::endl;
    image8.save_png("Elektro-8.png");

    std::cout << "Channel 9..." << std::endl;
    image9.save_png("Elektro-9.png");

    data_in.close();
}