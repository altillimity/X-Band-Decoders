#include <iostream>
#include <fstream>
#include <complex>
#include <vector>

#include "mersi_deframer.h"
#include "mersi_250m_reader.h"
#include "mersi_1000m_reader.h"

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
    // Output and Input file
    std::ifstream data_in(argv[1], std::ios::binary);

    // Read buffer
    uint8_t *buffer = new uint8_t[1024];

    // Deframer
    MersiDeframer mersiDefra;

    // MERSI Readers
    MERSI250Reader reader1, reader2, reader3, reader4, reader5, reader6;
    MERSI1000Reader reader7, reader8, reader9, reader10, reader11, reader12, reader13, reader14,
        reader15, reader16, reader17, reader18, reader19, reader20, reader21, reader22, reader23, reader24;

    // Graphics
    std::cout << "---------------------------" << std::endl;
    std::cout << "       FengYun-3 (D)" << std::endl;
    std::cout << " MERSI-2 Decoder by Aang23" << std::endl;
    std::cout << "---------------------------" << std::endl;
    std::cout << std::endl;

    std::cout << "Demultiplexing and deframing..." << std::endl;

    int vcidFrames = 0;
    int m1000Frames = 0;
    int m250Frames = 0;

    // Read until EOF
    while (!data_in.eof())
    {
        if (argc != 2)
        {
            std::cout << "Usage : " << argv[0] << " inputFrames.bin" << std::endl;
            return 0;
        }

        // Complete filesize
        size_t filesize = getFilesize(argv[1]);

        // Read buffer
        data_in.read((char *)buffer, 1024);

        // Extract VCID
        int vcid = buffer[5] % ((int)pow(2, 6));

        if (vcid == 3)
        {
            vcidFrames++;

            // Deframe MERSI
            std::vector<uint8_t> defraVec;
            defraVec.insert(defraVec.end(), &buffer[14], &buffer[14 + 882]);
            std::vector<std::vector<uint8_t>> out = mersiDefra.work(defraVec);

            for (std::vector<uint8_t> frameVec : out)
            {
                int marker = (frameVec[3] % (int)pow(2, 3)) << 7 | frameVec[4] >> 1;
                //std::cout << marker << std::endl;
                if (marker > 239)
                {
                    m1000Frames++;

                    // Demultiplex them all!
                    if (marker > 39 + 40 * 5 && marker < 39 + 40 * 5 + 10)
                        reader7.pushFrame(frameVec);
                    else if (marker > 39 + 40 * 5 + 10 * 2 && marker < 39 + 40 * 5 + 10 * (2 + 1))
                        reader8.pushFrame(frameVec);
                    else if (marker > 39 + 40 * 5 + 10 * 3 && marker < 39 + 40 * 5 + 10 * (3 + 1))
                        reader9.pushFrame(frameVec);
                    else if (marker > 39 + 40 * 5 + 10 * 4 && marker < 39 + 40 * 5 + 10 * (4 + 1))
                        reader10.pushFrame(frameVec);
                    else if (marker > 39 + 40 * 5 + 10 * 5 && marker < 39 + 40 * 5 + 10 * (5 + 1))
                        reader11.pushFrame(frameVec);
                    else if (marker > 39 + 40 * 5 + 10 * 6 && marker < 39 + 40 * 5 + 10 * (6 + 1))
                        reader12.pushFrame(frameVec);
                    else if (marker > 39 + 40 * 5 + 10 * 7 && marker < 39 + 40 * 5 + 10 * (7 + 1))
                        reader13.pushFrame(frameVec);
                    else if (marker > 39 + 40 * 5 + 10 * 8 && marker < 39 + 40 * 5 + 10 * (8 + 1))
                        reader14.pushFrame(frameVec);
                    else if (marker > 39 + 40 * 5 + 10 * 9 && marker < 39 + 40 * 5 + 10 * (9 + 1))
                        reader15.pushFrame(frameVec);
                    else if (marker > 39 + 40 * 5 + 10 * 10 && marker < 39 + 40 * 5 + 10 * (10 + 1))
                        reader16.pushFrame(frameVec);
                    else if (marker > 39 + 40 * 5 + 10 * 11 && marker < 39 + 40 * 5 + 10 * (11 + 1))
                        reader17.pushFrame(frameVec);
                    else if (marker > 39 + 40 * 5 + 10 * 12 && marker < 39 + 40 * 5 + 10 * (12 + 1))
                        reader18.pushFrame(frameVec);
                    else if (marker > 39 + 40 * 5 + 10 * 13 && marker < 39 + 40 * 5 + 10 * (13 + 1))
                        reader19.pushFrame(frameVec);
                    else if (marker > 39 + 40 * 5 + 10 * 14 && marker < 39 + 40 * 5 + 10 * (14 + 1))
                        reader20.pushFrame(frameVec);
                    else if (marker > 39 + 40 * 5 + 10 * 15 && marker < 39 + 40 * 5 + 10 * (15 + 1))
                        reader21.pushFrame(frameVec);
                    else if (marker > 39 + 40 * 5 + 10 * 16 && marker < 39 + 40 * 5 + 10 * (16 + 1))
                        reader22.pushFrame(frameVec);
                    else if (marker > 39 + 40 * 5 + 10 * 17 && marker < 39 + 40 * 5 + 10 * (17 + 1))
                        reader23.pushFrame(frameVec);
                    else if (marker > 39 + 40 * 5 + 10 * 18 && marker < 39 + 40 * 5 + 10 * (18 + 1))
                        reader24.pushFrame(frameVec);
                }
                else if (marker <= 239)
                {
                    m250Frames++;

                    // Demux those lonely 250m ones
                    if (marker < 39)
                        reader1.pushFrame(frameVec);
                    else if (marker > 39 && marker < 39 + 40)
                        reader2.pushFrame(frameVec);
                    else if (marker > 39 + 40 && marker < 39 + 40 + 40)
                        reader3.pushFrame(frameVec);
                    else if (marker > 39 + 40 * 2 && marker < 39 + 40 * (2 + 1))
                        reader4.pushFrame(frameVec);
                    else if (marker > 39 + 40 * 3 && marker < 39 + 40 * (3 + 1))
                        reader5.pushFrame(frameVec);
                    else if (marker > 39 + 40 * 4 && marker < 39 + 40 * (4 + 1))
                        reader6.pushFrame(frameVec);
                }
            }
        }

        // Show our progress
        std::cout << "\rProgress : " << round(((float)data_in.tellg() / (float)filesize) * 1000.0f) / 10.0f << "%     " << std::flush;
    }

    std::cout << std::endl;
    std::cout << std::endl;

    // Say what we found
    std::cout << "VCID 3 Frames         : " << vcidFrames << std::endl;
    std::cout << "250m Channels frames  : " << m250Frames << std::endl;
    std::cout << "1000m Channels frames : " << m1000Frames << std::endl;

    std::cout << std::endl;

    // Write images out
    std::cout << "Writing images... (Can take a while)" << std::endl;

    cimg_library::CImg<unsigned short> image1 = reader1.getImage();
    cimg_library::CImg<unsigned short> image2 = reader2.getImage();
    cimg_library::CImg<unsigned short> image3 = reader3.getImage();
    cimg_library::CImg<unsigned short> image4 = reader4.getImage();
    cimg_library::CImg<unsigned short> image5 = reader5.getImage();
    cimg_library::CImg<unsigned short> image6 = reader6.getImage();
    cimg_library::CImg<unsigned short> image7 = reader7.getImage();
    cimg_library::CImg<unsigned short> image8 = reader8.getImage();
    cimg_library::CImg<unsigned short> image9 = reader9.getImage();
    cimg_library::CImg<unsigned short> image10 = reader10.getImage();
    cimg_library::CImg<unsigned short> image11 = reader11.getImage();
    cimg_library::CImg<unsigned short> image12 = reader12.getImage();
    cimg_library::CImg<unsigned short> image13 = reader13.getImage();
    cimg_library::CImg<unsigned short> image14 = reader14.getImage();
    cimg_library::CImg<unsigned short> image15 = reader15.getImage();
    cimg_library::CImg<unsigned short> image16 = reader16.getImage();
    cimg_library::CImg<unsigned short> image17 = reader17.getImage();
    cimg_library::CImg<unsigned short> image18 = reader18.getImage();
    cimg_library::CImg<unsigned short> image19 = reader19.getImage();
    cimg_library::CImg<unsigned short> image20 = reader20.getImage();
    cimg_library::CImg<unsigned short> image21 = reader21.getImage();
    cimg_library::CImg<unsigned short> image22 = reader22.getImage();
    cimg_library::CImg<unsigned short> image23 = reader23.getImage();
    cimg_library::CImg<unsigned short> image24 = reader24.getImage();

    // They all need to be flipped horizontally
    image1.mirror('y');
    image2.mirror('y');
    image3.mirror('y');
    image4.mirror('y');
    image5.mirror('y');
    image6.mirror('y');
    image7.mirror('y');
    image8.mirror('y');
    image9.mirror('y');
    image10.mirror('y');
    image11.mirror('y');
    image12.mirror('y');
    image13.mirror('y');
    image14.mirror('y');
    image15.mirror('y');
    image16.mirror('y');
    image17.mirror('y');
    image18.mirror('y');
    image19.mirror('y');
    image20.mirror('y');
    image21.mirror('y');
    image22.mirror('y');
    image23.mirror('y');
    image24.mirror('y');

    // Takes a while so we say how we're doing
    std::cout << "Channel 1..." << std::endl;
    image1.save_png("MERSI2-1.png");

    std::cout << "Channel 2..." << std::endl;
    image2.save_png("MERSI2-2.png");

    std::cout << "Channel 3..." << std::endl;
    image3.save_png("MERSI2-3.png");

    std::cout << "Channel 4..." << std::endl;
    image4.save_png("MERSI2-4.png");

    std::cout << "Channel 5..." << std::endl;
    image5.save_png("MERSI2-5.png");

    std::cout << "Channel 6..." << std::endl;
    image6.save_png("MERSI2-6.png");

    std::cout << "Channel 7..." << std::endl;
    image7.save_png("MERSI2-7.png");

    std::cout << "Channel 8..." << std::endl;
    image8.save_png("MERSI2-8.png");

    std::cout << "Channel 9..." << std::endl;
    image9.save_png("MERSI2-9.png");

    std::cout << "Channel 10..." << std::endl;
    image10.save_png("MERSI2-10.png");

    std::cout << "Channel 11..." << std::endl;
    image11.save_png("MERSI2-11.png");

    std::cout << "Channel 12..." << std::endl;
    image12.save_png("MERSI2-12.png");

    std::cout << "Channel 13..." << std::endl;
    image13.save_png("MERSI2-13.png");

    std::cout << "Channel 14..." << std::endl;
    image14.save_png("MERSI2-14.png");

    std::cout << "Channel 15..." << std::endl;
    image15.save_png("MERSI2-15.png");

    std::cout << "Channel 16..." << std::endl;
    image16.save_png("MERSI2-16.png");

    std::cout << "Channel 17..." << std::endl;
    image17.save_png("MERSI2-17.png");

    std::cout << "Channel 18..." << std::endl;
    image18.save_png("MERSI2-18.png");

    std::cout << "Channel 19..." << std::endl;
    image19.save_png("MERSI2-19.png");

    std::cout << "Channel 20..." << std::endl;
    image20.save_png("MERSI2-20.png");

    std::cout << "Channel 21..." << std::endl;
    image21.save_png("MERSI2-21.png");

    std::cout << "Channel 22..." << std::endl;
    image22.save_png("MERSI2-22.png");

    std::cout << "Channel 23..." << std::endl;
    image23.save_png("MERSI2-23.png");

    std::cout << "Channel 24..." << std::endl;
    image24.save_png("MERSI2-24.png");

    // Output a few nice composites as well
    std::cout << "221 Composite..." << std::endl;
    cimg_library::CImg<unsigned short> image221(8192, std::max(image1.height(), image2.height()), 1, 3);
    {
        cimg_library::CImg<unsigned short> tempImage2 = image2, tempImage1 = image1;
        tempImage2.equalize(1000);
        tempImage1.equalize(1000);
        image221.draw_image(0, 0, 0, 0, tempImage2);
        image221.draw_image(0, 0, 0, 1, tempImage2);
        image221.draw_image(0, 0, 0, 2, tempImage1);
    }
    image221.save_png("MERSI2-RGB-221.png");

    std::cout << "341 Composite..." << std::endl;
    cimg_library::CImg<unsigned short> image341(8192, std::max(image1.height(), std::max(image3.height(), image4.height())), 1, 3);
    image341.draw_image(0, 0, 0, 0, image3);
    image341.draw_image(0, 0, 0, 1, image4);
    image341.draw_image(0, 0, 0, 2, image1);
    image341.save_png("MERSI2-RGB-341.png");

    std::cout << "441 Composite..." << std::endl;
    cimg_library::CImg<unsigned short> image441(8192, std::max(image3.height(), image4.height()), 1, 3);
    image441.draw_image(0, 0, 0, 0, image4);
    image441.draw_image(0, 0, 0, 1, image4);
    image441.draw_image(0, 0, 0, 2, image1);
    image441.save_png("MERSI2-RGB-441.png");

    std::cout << "321 Composite..." << std::endl;
    cimg_library::CImg<unsigned short> image321(8192, std::max(image3.height(), std::max(image2.height(), image3.height())), 1, 3);
    {
        cimg_library::CImg<unsigned short> tempImage3 = image3, tempImage2 = image2, tempImage1 = image1;
        tempImage3.equalize(1000);
        tempImage2.equalize(1000);
        tempImage1.equalize(1000);
        image321.draw_image(0, 0, 0, 0, tempImage3);
        image321.draw_image(0, 0, 0, 1, tempImage2);
        image321.draw_image(0, 0, 0, 2, tempImage1);
    }
    image321.save_png("MERSI2-RGB-321.png");

    /*std::cout << "321 Composite..." << std::endl;
    cimg_library::CImg<unsigned short> image321t(8192, std::max(image3.height(), std::max(image2.height(), image3.height())), 1, 3);
    {
        cimg_library::CImg<unsigned short> tempImage3 = image6, tempImage2 = image5, tempImage1 = image4;
        tempImage3.equalize(1000);
        tempImage2.equalize(1000);
        tempImage1.equalize(1000);
        image321t.draw_image(0, 0, 0, 0, tempImage3);
        image321t.draw_image(0, 0, 0, 1, tempImage2);
        image321t.draw_image(0, 0, 0, 2, tempImage1);
    }
    image321t.save_png("MERSI2-RGB-654.png");*/

    std::cout << "17.19.18 Composite..." << std::endl;
    cimg_library::CImg<unsigned short> image171918(2048, std::max(image17.height(), std::max(image18.height(), image19.height())), 1, 3);
    image171918.draw_image(0, 0, 0, 0, image17);
    image171918.draw_image(0, 0, 0, 1, image19);
    image171918.draw_image(0, 0, 0, 2, image18);
    image171918.save_png("MERSI2-RGB-18.19.17.png");

    data_in.close();
}