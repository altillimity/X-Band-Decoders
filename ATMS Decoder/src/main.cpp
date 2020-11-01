#include <iostream>
#include <fstream>
#include <cstdint>
#include <math.h>
#include <cstring>
#include <ccsds/demuxer.h>
#include <ccsds/vcdu.h>
#include "atms_reader.h"

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
    std::cout << "  ATMS Decoder by Aang23" << std::endl;
    std::cout << "         NPP / JPSS        " << std::endl;
    std::cout << "---------------------------" << std::endl;
    std::cout << std::endl;

    std::cout << "Demultiplexing and deframing..." << std::endl;

    libccsds::Demuxer ccsdsDemuxer;

    ATMSReader reader;

    // Read until EOF
    while (!data_in.eof())
    {
        // Read buffer
        data_in.read((char *)&cadu, 1024);

        // Parse this transport frame
        libccsds::VCDU vcdu = libccsds::parseVCDU(cadu);

        // Right channel? (VCID 1 is ATMS)
        if (vcdu.vcid == 1)
        {
            modis_cadu++;

            // Demux
            std::vector<libccsds::CCSDSPacket> ccsdsFrames = ccsdsDemuxer.work(cadu);

            // Count frames
            ccsds += ccsdsFrames.size();

            // Push into processor (filtering APID 528)
            for (libccsds::CCSDSPacket &pkt : ccsdsFrames)
            {
                if (pkt.header.apid == 528)
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

    std::cout << "VCID 1 (ATMS) Frames   : " << modis_cadu << std::endl;
    std::cout << "CCSDS Frames           : " << ccsds << std::endl;
    std::cout << "ATMS CCSDS Frames      : " << modis_ccsds << std::endl;
    std::cout << "ATMS Lines             : " << reader.lines << std::endl;

    std::cout << std::endl;

    // Write images out
    std::cout << "Writing images... (Can take a while)" << std::endl;

    cimg_library::CImg<unsigned short> image1 = reader.getImage(0);
    cimg_library::CImg<unsigned short> image2 = reader.getImage(1);
    cimg_library::CImg<unsigned short> image3 = reader.getImage(2);
    cimg_library::CImg<unsigned short> image4 = reader.getImage(3);
    cimg_library::CImg<unsigned short> image5 = reader.getImage(4);
    cimg_library::CImg<unsigned short> image6 = reader.getImage(5);
    cimg_library::CImg<unsigned short> image7 = reader.getImage(6);
    cimg_library::CImg<unsigned short> image8 = reader.getImage(7);
    cimg_library::CImg<unsigned short> image9 = reader.getImage(8);
    cimg_library::CImg<unsigned short> image10 = reader.getImage(9);
    cimg_library::CImg<unsigned short> image11 = reader.getImage(10);
    cimg_library::CImg<unsigned short> image12 = reader.getImage(11);
    cimg_library::CImg<unsigned short> image13 = reader.getImage(12);
    cimg_library::CImg<unsigned short> image14 = reader.getImage(13);
    cimg_library::CImg<unsigned short> image15 = reader.getImage(14);
    cimg_library::CImg<unsigned short> image16 = reader.getImage(15);
    cimg_library::CImg<unsigned short> image17 = reader.getImage(16);
    cimg_library::CImg<unsigned short> image18 = reader.getImage(17);
    cimg_library::CImg<unsigned short> image19 = reader.getImage(18);
    cimg_library::CImg<unsigned short> image20 = reader.getImage(19);
    cimg_library::CImg<unsigned short> image21 = reader.getImage(20);
    cimg_library::CImg<unsigned short> image22 = reader.getImage(21);

    image1.equalize(1000);
    image2.equalize(1000);
    image3.equalize(1000);
    image4.equalize(1000);
    image5.equalize(1000);
    image6.equalize(1000);
    image7.equalize(1000);
    image8.equalize(1000);
    image9.equalize(1000);
    image10.equalize(1000);
    image11.equalize(1000);
    image12.equalize(1000);
    image13.equalize(1000);
    image14.equalize(1000);
    image15.equalize(1000);
    image16.equalize(1000);
    image17.equalize(1000);
    image18.equalize(1000);
    image19.equalize(1000);
    image20.equalize(1000);
    image21.equalize(1000);
    image22.equalize(1000);

    std::cout << "Channel 1..." << std::endl;
    image1.save_png("ATMS-1.png");

    std::cout << "Channel 2..." << std::endl;
    image2.save_png("ATMS-2.png");

    std::cout << "Channel 3..." << std::endl;
    image3.save_png("ATMS-3.png");

    std::cout << "Channel 4..." << std::endl;
    image4.save_png("ATMS-4.png");

    std::cout << "Channel 5..." << std::endl;
    image5.save_png("ATMS-5.png");

    std::cout << "Channel 6..." << std::endl;
    image6.save_png("ATMS-6.png");

    std::cout << "Channel 7..." << std::endl;
    image7.save_png("ATMS-7.png");

    std::cout << "Channel 8..." << std::endl;
    image8.save_png("ATMS-8.png");

    std::cout << "Channel 9..." << std::endl;
    image9.save_png("ATMS-9.png");

    std::cout << "Channel 10..." << std::endl;
    image10.save_png("ATMS-10.png");

    std::cout << "Channel 11..." << std::endl;
    image11.save_png("ATMS-11.png");

    std::cout << "Channel 12..." << std::endl;
    image12.save_png("ATMS-12.png");

    std::cout << "Channel 13..." << std::endl;
    image13.save_png("ATMS-13.png");

    std::cout << "Channel 14..." << std::endl;
    image14.save_png("ATMS-14.png");

    std::cout << "Channel 15..." << std::endl;
    image15.save_png("ATMS-15.png");

    std::cout << "Channel 16..." << std::endl;
    image16.save_png("ATMS-16.png");

    std::cout << "Channel 17..." << std::endl;
    image17.save_png("ATMS-17.png");

    std::cout << "Channel 18..." << std::endl;
    image18.save_png("ATMS-18.png");

    std::cout << "Channel 19..." << std::endl;
    image19.save_png("ATMS-19.png");

    std::cout << "Channel 20..." << std::endl;
    image20.save_png("ATMS-20.png");

    std::cout << "Channel 21..." << std::endl;
    image21.save_png("ATMS-21.png");

    std::cout << "Channel 22..." << std::endl;
    image22.save_png("ATMS-22.png");

    // Output a few nice composites as well
    std::cout << "Global Composite..." << std::endl;
    cimg_library::CImg<unsigned short> imageAll(96 * 11, image1.height() * 2, 1, 1);
    {
        // Row 1
        imageAll.draw_image(96 * 0, 0, 0, 0, image1);
        imageAll.draw_image(96 * 1, 0, 0, 0, image2);
        imageAll.draw_image(96 * 2, 0, 0, 0, image3);
        imageAll.draw_image(96 * 3, 0, 0, 0, image4);
        imageAll.draw_image(96 * 4, 0, 0, 0, image5);
        imageAll.draw_image(96 * 5, 0, 0, 0, image6);
        imageAll.draw_image(96 * 6, 0, 0, 0, image7);
        imageAll.draw_image(96 * 7, 0, 0, 0, image8);
        imageAll.draw_image(96 * 8, 0, 0, 0, image9);
        imageAll.draw_image(96 * 9, 0, 0, 0, image10);
        imageAll.draw_image(96 * 10, 0, 0, 0, image11);

        // Row 2
        imageAll.draw_image(96 * 0, image1.height(), 0, 0, image12);
        imageAll.draw_image(96 * 1, image1.height(), 0, 0, image13);
        imageAll.draw_image(96 * 2, image1.height(), 0, 0, image14);
        imageAll.draw_image(96 * 3, image1.height(), 0, 0, image15);
        imageAll.draw_image(96 * 4, image1.height(), 0, 0, image16);
        imageAll.draw_image(96 * 5, image1.height(), 0, 0, image17);
        imageAll.draw_image(96 * 6, image1.height(), 0, 0, image18);
        imageAll.draw_image(96 * 7, image1.height(), 0, 0, image19);
        imageAll.draw_image(96 * 8, image1.height(), 0, 0, image20);
        imageAll.draw_image(96 * 9, image1.height(), 0, 0, image21);
        imageAll.draw_image(96 * 10, image1.height(), 0, 0, image22);
    }
    imageAll.save_png("ATMS-ALL.png");

    std::cout << "346 Composite..." << std::endl;
    cimg_library::CImg<unsigned short> image346(96, image1.height(), 1, 3);
    {
        image346.draw_image(0, 0, 0, 0, image3);
        image346.draw_image(0, 0, 0, 1, image4);
        image346.draw_image(0, 0, 0, 2, image6);
    }
    image346.equalize(1000);
    image346.save_png("ATMS-RGB-346.png");

    std::cout << "335 Composite..." << std::endl;
    cimg_library::CImg<unsigned short> image335(96, image1.height(), 1, 3);
    {
        image335.draw_image(0, 0, 0, 0, image3);
        image335.draw_image(0, 0, 0, 1, image3);
        image335.draw_image(0, 0, 0, 2, image5);
    }
    image335.equalize(1000);
    image335.save_png("ATMS-RGB-335.png");

    std::cout << "4.3.17 Composite..." << std::endl;
    cimg_library::CImg<unsigned short> image4317(96, image1.height(), 1, 3);
    {
        image4317.draw_image(0, 0, 0, 0, image4);
        image4317.draw_image(0, 0, 0, 1, image3);
        image4317.draw_image(0, 0, 0, 2, image17);
    }
    image4317.equalize(1000);
    image4317.save_png("ATMS-RGB-4.3.17.png");

    std::cout << "445 Composite..." << std::endl;
    cimg_library::CImg<unsigned short> image445(96, image1.height(), 1, 3);
    {
        image445.draw_image(0, 0, 0, 0, image4);
        image445.draw_image(0, 0, 0, 1, image4);
        image445.draw_image(0, 0, 0, 2, image5);
    }
    image445.equalize(1000);
    image445.save_png("ATMS-RGB-445.png");

    std::cout << "4.4.17 Composite..." << std::endl;
    cimg_library::CImg<unsigned short> image4417(96, image1.height(), 1, 3);
    {
        image4417.draw_image(0, 0, 0, 0, image4);
        image4417.draw_image(0, 0, 0, 1, image4);
        image4417.draw_image(0, 0, 0, 2, image17);
    }
    image4417.equalize(1000);
    image4417.save_png("ATMS-RGB-4.4.17.png");

    std::cout << "4.16.17 Composite..." << std::endl;
    cimg_library::CImg<unsigned short> image41617(96, image1.height(), 1, 3);
    {
        image41617.draw_image(0, 0, 0, 0, image4);
        image41617.draw_image(0, 0, 0, 1, image16);
        image41617.draw_image(0, 0, 0, 2, image17);
    }
    image41617.equalize(1000);
    image41617.save_png("ATMS-RGB-4.16.17.png");

    std::cout << "3.4.17 Composite..." << std::endl;
    cimg_library::CImg<unsigned short> image3417(96, image1.height(), 1, 3);
    {
        image3417.draw_image(0, 0, 0, 0, image3);
        image3417.draw_image(0, 0, 0, 1, image4);
        image3417.draw_image(0, 0, 0, 2, image17);
    }
    image3417.equalize(1000);
    image3417.save_png("ATMS-RGB-3.4.17.png");

    std::cout << "5.5.17 Composite..." << std::endl;
    cimg_library::CImg<unsigned short> image5517(96, image1.height(), 1, 3);
    {
        image5517.draw_image(0, 0, 0, 0, image5);
        image5517.draw_image(0, 0, 0, 1, image5);
        image5517.draw_image(0, 0, 0, 2, image17);
    }
    image5517.equalize(1000);
    image5517.save_png("ATMS-RGB-5.5.17.png");

    std::cout << "6.4.17 Composite..." << std::endl;
    cimg_library::CImg<unsigned short> image6417(96, image1.height(), 1, 3);
    {
        image6417.draw_image(0, 0, 0, 0, image6);
        image6417.draw_image(0, 0, 0, 1, image4);
        image6417.draw_image(0, 0, 0, 2, image17);
    }
    image6417.equalize(1000);
    image6417.save_png("ATMS-RGB-6.4.17.png");

    std::cout << "16.4.17 Composite..." << std::endl;
    cimg_library::CImg<unsigned short> image16417(96, image1.height(), 1, 3);
    {
        image16417.draw_image(0, 0, 0, 0, image16);
        image16417.draw_image(0, 0, 0, 1, image4);
        image16417.draw_image(0, 0, 0, 2, image17);
    }
    image16417.equalize(1000);
    image16417.save_png("ATMS-RGB-16.4.17.png");

    std::cout << "17.16.6 Composite..." << std::endl;
    cimg_library::CImg<unsigned short> image17166(96, image1.height(), 1, 3);
    {
        image17166.draw_image(0, 0, 0, 0, image17);
        image17166.draw_image(0, 0, 0, 1, image16);
        image17166.draw_image(0, 0, 0, 2, image6);
    }
    image17166.equalize(1000);
    image17166.save_png("ATMS-RGB-17.16.6.png");

    // Output a few nice composites as well
    std::cout << "Global Composite..." << std::endl;
    cimg_library::CImg<unsigned short> imageRgbAll(96 * 6, image1.height() * 2, 1, 3);
    {
        // Row 1
        imageRgbAll.draw_image(96 * 0, 0, 0, 0, image346);
        imageRgbAll.draw_image(96 * 1, 0, 0, 0, image335);
        imageRgbAll.draw_image(96 * 2, 0, 0, 0, image4317);
        imageRgbAll.draw_image(96 * 3, 0, 0, 0, image445);
        imageRgbAll.draw_image(96 * 4, 0, 0, 0, image4417);
        imageRgbAll.draw_image(96 * 5, 0, 0, 0, image41617);

        // Row 2
        imageRgbAll.draw_image(96 * 0, image1.height(), 0, 0, image3417);
        imageRgbAll.draw_image(96 * 1, image1.height(), 0, 0, image5517);
        imageRgbAll.draw_image(96 * 2, image1.height(), 0, 0, image6417);
        imageRgbAll.draw_image(96 * 3, image1.height(), 0, 0, image16417);
        imageRgbAll.draw_image(96 * 4, image1.height(), 0, 0, image17166);
    }
    imageRgbAll.save_png("ATMS-RGB-ALL.png");

    data_out.close();
}
