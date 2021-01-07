#include <iostream>
#include <fstream>
#include <cstdint>
#include <math.h>
#include <cstring>
#include <ccsds/demuxer.h>
#include <ccsds/vcdu.h>
#include "airs_reader.h"

// Return filesize
size_t getFilesize(std::string filepath)
{
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    std::size_t fileSize = file.tellg();
    file.close();
    return fileSize;
}

// IO files
std::ifstream data_in;

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
    data_in = std::ifstream(argv[1], std::ios::binary);

    // Read buffer
    libccsds::CADU cadu;

    // Counters
    uint64_t airs_cadu = 0, ccsds = 0, airs_ccsds = 0;

    // Graphics
    std::cout << "---------------------------" << std::endl;
    std::cout << " AIRS Decoder by Aang23" << std::endl;
    std::cout << "          Aqua" << std::endl;
    std::cout << "---------------------------" << std::endl;
    std::cout << std::endl;

    std::cout << "Demultiplexing and deframing..." << std::endl;

    libccsds::Demuxer ccsdsDemuxer = libccsds::Demuxer();

    // Readers
    AIRSReader airs_reader;

    // Read until EOF
    while (!data_in.eof())
    {
        // Read buffer
        data_in.read((char *)&cadu, 1024);

        // Parse this transport frame
        libccsds::VCDU vcdu = libccsds::parseVCDU(cadu);

        // Right channel? (VCID 35 is AIRS)
        if (vcdu.vcid == 35)
        {
            airs_cadu++;

            // Demux
            std::vector<libccsds::CCSDSPacket> ccsdsFrames = ccsdsDemuxer.work(cadu);

            // Count frames
            ccsds += ccsdsFrames.size();

            // Push into processor (filtering APID 404)
            for (libccsds::CCSDSPacket &pkt : ccsdsFrames)
            {
                if (pkt.header.apid == 404)
                {
                    airs_reader.work(pkt);
                    airs_ccsds++;
                }
            }
        }

        // Show our progress
        std::cout << "\rProgress : " << round(((float)data_in.tellg() / (float)filesize) * 1000.0f) / 10.0f << "%     " << std::flush;
    }

    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "VCID 35 (AIRS) Frames : " << airs_cadu << std::endl;
    std::cout << "CCSDS Frames          : " << ccsds << std::endl;
    std::cout << "AIRS Frames           : " << airs_ccsds << std::endl;

    std::cout << std::endl;

    // Write images out
    std::cout << "Writing images... (Can take a while)" << std::endl;

    //for (int i = 0; i < 2600; i++)
    // {
    //    std::cout << "Channel " << (i + 1) << "..." << std::endl;
    //    airs_reader.getChannel(i).save_png(std::string("AIRS-" + std::to_string(i + 1) + ".png").c_str());
    //}

    for (int i = 0; i < 4; i++)
    {
        std::cout << "HD Channel " << (i + 1) << "..." << std::endl;
        airs_reader.getHDChannel(i).save_png(std::string("AIRS-HD-" + std::to_string(i + 1) + ".png").c_str());
    }

    // Output a few nice composites as well
    std::cout << "Global Composite..." << std::endl;
    int all_width_count = 100;
    int all_height_count = 27;
    cimg_library::CImg<unsigned short> imageAll(90 * all_width_count, airs_reader.getChannel(0).height() * all_height_count, 1, 1);
    {
        int height = airs_reader.getChannel(0).height();

        for (int row = 0; row < all_height_count; row++)
        {
            for (int column = 0; column < all_width_count; column++)
            {
                if (row * all_width_count + column >= 2666)
                    break;

                imageAll.draw_image(90 * column, height * row, 0, 0, airs_reader.getChannel(row * all_width_count + column));
            }
        }
    }
    imageAll.save_png("AIRS-ALL.png");

    std::cout << "HD 221 Composite..." << std::endl;
    cimg_library::CImg<unsigned short> image221(90 * 8, airs_reader.getHDChannel(0).height(), 1, 3);
    {
        image221.draw_image(0, 0, 0, 0, airs_reader.getHDChannel(1));
        image221.draw_image(0, 0, 0, 1, airs_reader.getHDChannel(1));
        image221.draw_image(0, 0, 0, 2, airs_reader.getHDChannel(0));
    }
    image221.save_png("AIRS-HD-RGB-221.png");

    std::cout << "HD 332 Composite..." << std::endl;
    cimg_library::CImg<unsigned short> image332(90 * 8, airs_reader.getHDChannel(0).height(), 1, 3);
    {
        image332.draw_image(0, 0, 0, 0, airs_reader.getHDChannel(2));
        image332.draw_image(0, 0, 0, 1, airs_reader.getHDChannel(2));
        image332.draw_image(0, 0, 0, 2, airs_reader.getHDChannel(1));
    }
    image332.save_png("AIRS-HD-RGB-332.png");

    std::cout << "HD 321 Composite..." << std::endl;
    cimg_library::CImg<unsigned short> image321(90 * 8, airs_reader.getHDChannel(0).height(), 1, 3);
    {
        image321.draw_image(0, 0, 0, 0, airs_reader.getHDChannel(2));
        image321.draw_image(0, 0, 0, 1, airs_reader.getHDChannel(1));
        image321.draw_image(0, 0, 0, 2, airs_reader.getHDChannel(0));
    }
    image321.save_png("AIRS-HD-RGB-321.png");

    data_in.close();
}
