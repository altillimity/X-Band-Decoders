#include <iostream>
#include <fstream>
#include <cstdint>
#include <math.h>
#include <cstring>
#include <ccsds/demuxer.h>
#include <ccsds/vcdu.h>
#include "ceres_reader.h"

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
    if (argc != 2 && argc != 3)
    {
        std::cout << "Usage (default Aqua, -t for Terra): " << argv[0] << " inputFrames.bin [-t]" << std::endl;
        return 0;
    }

    bool terra = false;

    // Terra mode?
    if (argc == 3)
    {
        if (std::string(argv[2]) == "-t")
            terra = true;
    }

    // Complete filesize
    size_t filesize = getFilesize(argv[1]);

    // Output and Input file
    data_in = std::ifstream(argv[1], std::ios::binary);
    // data_out = std::ofstream("test.bin", std::ios::binary);

    // Read buffer
    libccsds::CADU cadu;

    // Counters
    uint64_t ceres_cadu = 0, ccsds = 0, modis_ccsds = 0;

    // Graphics
    std::cout << "---------------------------" << std::endl;
    std::cout << "  CERES Decoder by Aang23" << std::endl;
    std::cout << "        Aqua / Terra      " << std::endl;
    std::cout << "---------------------------" << std::endl;
    std::cout << std::endl;

    std::cout << "Demultiplexing and deframing..." << std::endl;

    libccsds::Demuxer ccsdsDemuxer1, ccsdsDemuxer2;

    CERESReader reader1, reader2;

    // Read until EOF
    while (!data_in.eof())
    {
        // Read buffer
        data_in.read((char *)&cadu, 1024);

        // Parse this transport frame
        libccsds::VCDU vcdu = libccsds::parseVCDU(cadu);

        // Right channel? (VCID 10-15 is CERES)
        if (vcdu.vcid == (terra ? 11 : 10))
        {
            ceres_cadu++;

            // Demux
            std::vector<libccsds::CCSDSPacket> ccsdsFrames = ccsdsDemuxer1.work(cadu);

            // Count frames
            ccsds += ccsdsFrames.size();

            // Push into processor
            for (libccsds::CCSDSPacket &pkt : ccsdsFrames)
            {
                if (pkt.header.apid == (terra ? 131 : 141))
                {
                    modis_ccsds++;
                    reader1.work(pkt);
                }
            }
        }
        else if (vcdu.vcid == (terra ? 11 : 15))
        {
            ceres_cadu++;

            // Demux
            std::vector<libccsds::CCSDSPacket> ccsdsFrames = ccsdsDemuxer2.work(cadu);

            // Count frames
            ccsds += ccsdsFrames.size();

            // Push into processor
            for (libccsds::CCSDSPacket &pkt : ccsdsFrames)
            {
                if (pkt.header.apid == (terra ? 167 : 157))
                {
                    modis_ccsds++;
                    reader2.work(pkt);
                }
            }
        }

        // Show our progress
        std::cout << "\rProgress : " << round(((float)data_in.tellg() / (float)filesize) * 1000.0f) / 10.0f << "%     " << std::flush;
    }

    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "VCID (CERES) Frames    : " << ceres_cadu << std::endl;
    std::cout << "CCSDS Frames           : " << ccsds << std::endl;
    std::cout << "CERES CCSDS Frames     : " << modis_ccsds << std::endl;
    std::cout << "CERES 1 Lines          : " << reader1.lines << std::endl;
    std::cout << "CERES 2 Lines          : " << reader2.lines << std::endl;

    std::cout << std::endl;

    // Write images out
    std::cout << "Writing images... (Can take a while)" << std::endl;

    cimg_library::CImg<unsigned short> image_shortwave1 = reader1.getImage(0);
    cimg_library::CImg<unsigned short> image_longwave1 = reader1.getImage(1);
    cimg_library::CImg<unsigned short> image_total1 = reader1.getImage(2);
    cimg_library::CImg<unsigned short> image_shortwave2 = reader2.getImage(0);
    cimg_library::CImg<unsigned short> image_longwave2 = reader2.getImage(1);
    cimg_library::CImg<unsigned short> image_total2 = reader2.getImage(2);

    // Resize to be... Readable?
    image_shortwave1.resize(image_shortwave1.width(), image_shortwave1.height() * 7);
    image_longwave1.resize(image_longwave1.width(), image_longwave1.height() * 7);
    image_total1.resize(image_total1.width(), image_total1.height() * 7);
    image_shortwave2.resize(image_shortwave2.width(), image_shortwave2.height() * 7);
    image_longwave2.resize(image_longwave2.width(), image_longwave2.height() * 7);
    image_total2.resize(image_total2.width(), image_total2.height() * 7);

    // Equalize
    image_shortwave1.equalize(1000);
    image_longwave1.equalize(1000);
    image_total1.equalize(1000);
    image_shortwave2.equalize(1000);
    image_longwave2.equalize(1000);
    image_total2.equalize(1000);

    std::cout << "Shortwave Channel 1..." << std::endl;
    image_shortwave1.save_png("CERES1-SHORTWAVE.png");

    std::cout << "Longwave Channel 1..." << std::endl;
    image_longwave1.save_png("CERES1-LONGWAVE.png");

    std::cout << "Total Channel 1..." << std::endl;
    image_total1.save_png("CERES1-TOTAL.png");

    // Output a few nice composites as well
    std::cout << "Global Composite 1..." << std::endl;
    cimg_library::CImg<unsigned short> imageAll1(image_shortwave1.width() + image_longwave1.width() + image_total1.width(), image_shortwave1.height(), 1, 1);
    {
        imageAll1.draw_image(0, 0, 0, 0, image_shortwave1);
        imageAll1.draw_image(image_shortwave1.width(), 0, 0, 0, image_longwave1);
        imageAll1.draw_image(image_shortwave1.width() + image_longwave1.width(), 0, 0, 0, image_total1);
    }
    imageAll1.save_png("CERES1-ALL.png");

    std::cout << "Shortwave Channel 2..." << std::endl;
    image_shortwave2.save_png("CERES2-SHORTWAVE.png");

    std::cout << "Longwave Channel 2..." << std::endl;
    image_longwave2.save_png("CERES2-LONGWAVE.png");

    std::cout << "Total Channel 2..." << std::endl;
    image_total2.save_png("CERES2-TOTAL.png");

    // Output a few nice composites as well
    std::cout << "Global Composite 2..." << std::endl;
    cimg_library::CImg<unsigned short> imageAll2(image_shortwave2.width() + image_longwave2.width() + image_total2.width(), image_shortwave2.height(), 1, 1);
    {
        imageAll2.draw_image(0, 0, 0, 0, image_shortwave2);
        imageAll2.draw_image(image_shortwave2.width(), 0, 0, 0, image_longwave2);
        imageAll2.draw_image(image_shortwave2.width() + image_longwave2.width(), 0, 0, 0, image_total2);
    }
    imageAll2.save_png("CERES2-ALL.png");

    data_in.close();
}
