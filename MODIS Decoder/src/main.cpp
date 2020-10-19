#include <iostream>
#include <fstream>
#include <cstdint>
#include <math.h>
#include <cstring>
#include <ccsds/demuxer.h>
#include <ccsds/vcdu.h>
#include "modis_reader.h"

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
std::ofstream data_out;

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

    // Read buffer
    libccsds::CADU cadu;

    // Counters
    uint64_t modis_cadu = 0, ccsds = 0, modis_ccsds = 0;

    // Graphics
    std::cout << "---------------------------" << std::endl;
    std::cout << " MODIS Decoder by Aang23" << std::endl;
    std::cout << "      Aqua / Terra" << std::endl;
    std::cout << "---------------------------" << std::endl;
    std::cout << std::endl;

    std::cout << "Demultiplexing and deframing..." << std::endl;

    libccsds::Demuxer ccsdsDemuxer;

    MODISReader reader;

    // Read until EOF
    while (!data_in.eof())
    {
        // Read buffer
        data_in.read((char *)&cadu, 1024);

        // Parse this transport frame
        libccsds::VCDU vcdu = libccsds::parseVCDU(cadu);

        // Right channel? (VCID 30/42 is MODIS)
        if (vcdu.vcid == (terra ? 42 : 30))
        {
            modis_cadu++;

            // Demux
            std::vector<libccsds::CCSDSPacket> ccsdsFrames = ccsdsDemuxer.work(cadu);

            // Count frames
            ccsds += ccsdsFrames.size();

            // Push into processor (filtering APID 64)
            for (libccsds::CCSDSPacket &pkt : ccsdsFrames)
            {
                if (pkt.header.apid == 64)
                {
                    modis_ccsds++;
                    reader.work(pkt);
                }
            }
        }

        // Show our progress
        std::cout << "\rProgress : " << round(((float)data_in.tellg() / (float)filesize) * 1000.0f) / 10.0f << "%     " << std::flush;
    }

    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "VCID 30 (MODIS) Frames : " << modis_cadu << std::endl;
    std::cout << "CCSDS Frames           : " << ccsds << std::endl;
    std::cout << "MODIS CCSDS Frames     : " << modis_ccsds << std::endl;
    std::cout << "MODIS Day frames       : " << reader.day_count << std::endl;
    std::cout << "MODIS Night frames     : " << reader.night_count << std::endl;
    std::cout << "MODIS 1km lines        : " << reader.lines << std::endl;
    std::cout << "MODIS 500m lines       : " << reader.lines * 2 << std::endl;
    std::cout << "MODIS 250m lines       : " << reader.lines * 4 << std::endl;

    std::cout << std::endl;

    // Write images out
    std::cout << "Writing images... (Can take a while)" << std::endl;

    for (int i = 0; i < 2; i++)
    {
        cimg_library::CImg<unsigned short> image = reader.getImage250m(i);
        std::cout << "Channel " << i + 1 << "..." << std::endl;
        image.save_png(std::string("MODIS-" + std::to_string(i + 1) + ".png").c_str());
    }

    for (int i = 0; i < 5; i++)
    {
        cimg_library::CImg<unsigned short> image = reader.getImage500m(i);
        std::cout << "Channel " << i + 3 << "..." << std::endl;
        image.save_png(std::string("MODIS-" + std::to_string(i + 3) + ".png").c_str());
    }

    for (int i = 0; i < 31; i++)
    {
        cimg_library::CImg<unsigned short> image = reader.getImage1000m(i);
        if (i < 5)
        {
            std::cout << "Channel " << i + 8 << "..." << std::endl;
            image.save_png(std::string("MODIS-" + std::to_string(i + 8) + ".png").c_str());
        }
        else if (i == 5)
        {
            std::cout << "Channel 13L..." << std::endl;
            image.save_png(std::string("MODIS-13L.png").c_str());
        }
        else if (i == 6)
        {
            std::cout << "Channel 13H..." << std::endl;
            image.save_png(std::string("MODIS-13H.png").c_str());
        }
        else if (i == 7)
        {
            std::cout << "Channel 14L..." << std::endl;
            image.save_png(std::string("MODIS-14L.png").c_str());
        }
        else if (i == 8)
        {
            std::cout << "Channel 14H..." << std::endl;
            image.save_png(std::string("MODIS-14H.png").c_str());
        }
        else
        {
            std::cout << "Channel " << i + 6 << "..." << std::endl;
            image.save_png(std::string("MODIS-" + std::to_string(i + 6) + ".png").c_str());
        }
    }

    // Output a few nice composites as well
    std::cout << "221 Composite..." << std::endl;
    cimg_library::CImg<unsigned short> image221(1354 * 4, reader.lines * 4, 1, 3);
    {
        cimg_library::CImg<unsigned short> tempImage2 = reader.getImage250m(1), tempImage1 = reader.getImage250m(0);
        image221.draw_image(0, 0, 0, 0, tempImage2);
        image221.draw_image(0, 0, 0, 1, tempImage2);
        image221.draw_image(0, 0, 0, 2, tempImage1);
        image221.normalize(0, std::numeric_limits<unsigned short>::max());
    }
    image221.save_png("MODIS-RGB-221.png");

    std::cout << "121 Composite..." << std::endl;
    cimg_library::CImg<unsigned short> image121(1354 * 4, reader.lines * 4, 1, 3);
    {
        cimg_library::CImg<unsigned short> tempImage2 = reader.getImage250m(1), tempImage1 = reader.getImage250m(0);
        tempImage2.equalize(1000);
        tempImage1.equalize(1000);
        image121.draw_image(0, 0, 0, 0, tempImage1);
        image121.draw_image(0, 0, 0, 1, tempImage2);
        image121.draw_image(0, 0, 0, 2, tempImage1);
        image121.normalize(0, std::numeric_limits<unsigned short>::max());
    }
    image121.save_png("MODIS-RGB-121.png");

    std::cout << "143 Composite..." << std::endl;
    cimg_library::CImg<unsigned short> image143(1354 * 4, reader.lines * 4, 1, 3);
    {
        cimg_library::CImg<unsigned short> tempImage4 = reader.getImage500m(1), tempImage3 = reader.getImage500m(0), tempImage1 = reader.getImage250m(0);
        tempImage4.equalize(1000);
        tempImage3.equalize(1000);
        tempImage1.equalize(1000);
        image143.draw_image(0, 0, 0, 0, tempImage1);
        tempImage3.resize(tempImage3.width() * 2, tempImage3.height() * 2);
        tempImage4.resize(tempImage4.width() * 2, tempImage4.height() * 2);
        image143.draw_image(0, 0, 0, 1, tempImage4);
        image143.draw_image(0, 0, 0, 2, tempImage3);
        image143.normalize(0, std::numeric_limits<unsigned short>::max());
        image143.equalize(1000);
    }
    image143.save_png("MODIS-RGB-143.png");

    data_in.close();
    data_out.close();
}
