#include <iostream>
#include <fstream>
#include <cstdint>
#include <math.h>
#include <cstring>
#include <ccsds/demuxer.h>
#include <ccsds/vcdu.h>
#include "channel_reader.h"
#include "channel_correlator.h"
#include "viirs_defrag.h"
#include <limits>

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
        std::cout << "Usage : " << argv[0] << " cadufile.bin" << std::endl;
        return 0;
    }

    // Complete filesize
    size_t filesize = getFilesize(argv[1]);

    // Output and Input file
    data_in = std::ifstream(argv[1], std::ios::binary);

    // Read buffer
    libccsds::CADU cadu;

    // Counters
    uint64_t vcidFrames = 0, virr_cadu = 0, ccsds_frames = 0;

    libccsds::Demuxer ccsdsDemux;

    // Readers for all APIDs
    VIIRSReader reader_m4(VIIRSChannels[800]),
        reader_m5(VIIRSChannels[801]),
        reader_m3(VIIRSChannels[802]),
        reader_m2(VIIRSChannels[803]),
        reader_m1(VIIRSChannels[804]),
        reader_m6(VIIRSChannels[805]),
        reader_m7(VIIRSChannels[806]),
        reader_m9(VIIRSChannels[807]),
        reader_m10(VIIRSChannels[808]),
        reader_m8(VIIRSChannels[809]),
        reader_m11(VIIRSChannels[810]),
        reader_m13(VIIRSChannels[811]),
        reader_m12(VIIRSChannels[812]),
        reader_m16(VIIRSChannels[814]),
        reader_m15(VIIRSChannels[815]),
        reader_m14(VIIRSChannels[816]);

    VIIRSReader reader_i1(VIIRSChannels[818]),
        reader_i2(VIIRSChannels[819]),
        reader_i3(VIIRSChannels[820]),
        reader_i4(VIIRSChannels[813]),
        reader_i5(VIIRSChannels[817]);

    VIIRSReader reader_dnb(VIIRSChannels[821]),
        reader_dnb_mgs(VIIRSChannels[822]),
        reader_dnb_lgs(VIIRSChannels[823]);

    // Graphics
    std::cout << "---------------------------" << std::endl;
    std::cout << "       VIIRS Decoder" << std::endl;
    std::cout << "  by Aang23, based on the" << std::endl;
    std::cout << "    work by Luigi Cruz" << std::endl;
    std::cout << "---------------------------" << std::endl;
    std::cout << std::endl;

    std::cout << "Demultiplexing and processing..." << std::endl;

    uint16_t lastCount = 0;

    // Read until EOF
    while (!data_in.eof())
    {
        // Read buffer
        data_in.read((char *)&cadu, 1024);
        vcidFrames++;

        // Parse this transport frame
        libccsds::VCDU vcdu = libccsds::parseVCDU(cadu);

        // Right channel? (VCID 30 is MODIS)
        if (vcdu.vcid == 16)
        {
            virr_cadu++;

            std::vector<libccsds::CCSDSPacket> ccsds2 = ccsdsDemux.work(cadu);

            ccsds_frames += ccsds2.size();

            for (libccsds::CCSDSPacket &pkt : ccsds2)
            {
                // Moderate resolution channels
                reader_m4.feed(pkt);
                reader_m5.feed(pkt);
                reader_m3.feed(pkt);
                reader_m2.feed(pkt);
                reader_m1.feed(pkt);
                reader_m6.feed(pkt);
                reader_m7.feed(pkt);
                reader_m9.feed(pkt);
                reader_m10.feed(pkt);
                reader_m8.feed(pkt);
                reader_m11.feed(pkt);
                reader_m13.feed(pkt);
                reader_m12.feed(pkt);
                reader_m16.feed(pkt);
                reader_m15.feed(pkt);
                reader_m14.feed(pkt);

                // Imaging channels
                reader_i1.feed(pkt);
                reader_i2.feed(pkt);
                reader_i3.feed(pkt);
                reader_i4.feed(pkt);
                reader_i5.feed(pkt);

                // DNB channels
                reader_dnb.feed(pkt);
                reader_dnb_mgs.feed(pkt);
                reader_dnb_lgs.feed(pkt);
            }
        }

        std::cout << "\rProgress : " << round(((float)data_in.tellg() / (float)filesize) * 1000.0f) / 10.0f << "%     " << std::flush;
    }

    std::cout << std::endl;
    std::cout << std::endl;

    // Say what we found
    std::cout << "VCID 16 Frames         : " << vcidFrames << std::endl;
    std::cout << "CCSDS Frames           : " << ccsds_frames << std::endl;

    std::cout << std::endl;

    // Process them all
    std::cout << "Decompressing and decoding channels... (Can take a while)" << std::endl;

    // Moderate resolution channels
    reader_m4.process();
    reader_m5.process();
    reader_m3.process();
    reader_m2.process();
    reader_m1.process();
    reader_m6.process();
    reader_m7.process();
    reader_m9.process();
    reader_m10.process();
    reader_m8.process();
    reader_m11.process();
    reader_m13.process();
    reader_m12.process();
    reader_m16.process();
    reader_m15.process();
    reader_m14.process();

    // Imaging channels
    reader_i1.process();
    reader_i2.process();
    reader_i3.process();
    reader_i4.process();
    reader_i5.process();

    // DNB channels
    reader_dnb.process();
    reader_dnb_mgs.process();
    reader_dnb_lgs.process();

    // Differential decoding for M5, M3, M2, M1
    std::cout << "Diff M5..." << std::endl;
    {
        std::pair<VIIRSReader, VIIRSReader> correlatedChannels = correlateChannels(reader_m4, reader_m5);
        correlatedChannels.second.differentialDecode(correlatedChannels.first, 1);
        reader_m5 = correlatedChannels.second;
    }
    std::cout << "Diff M3..." << std::endl;
    {
        std::pair<VIIRSReader, VIIRSReader> correlatedChannels = correlateChannels(reader_m4, reader_m3);
        correlatedChannels.second.differentialDecode(correlatedChannels.first, 1);
        reader_m3 = correlatedChannels.second;
    }
    std::cout << "Diff M2..." << std::endl;
    {
        std::pair<VIIRSReader, VIIRSReader> correlatedChannels = correlateChannels(reader_m3, reader_m2);
        correlatedChannels.second.differentialDecode(correlatedChannels.first, 1);
        reader_m2 = correlatedChannels.second;
    }
    std::cout << "Diff M1..." << std::endl;
    {
        std::pair<VIIRSReader, VIIRSReader> correlatedChannels = correlateChannels(reader_m2, reader_m1);
        correlatedChannels.second.differentialDecode(correlatedChannels.first, 1);
        reader_m1 = correlatedChannels.second;
    }

    // Differential decoding for M8, M11
    std::cout << "Diff M8..." << std::endl;
    {
        std::pair<VIIRSReader, VIIRSReader> correlatedChannels = correlateChannels(reader_m10, reader_m8);
        correlatedChannels.second.differentialDecode(correlatedChannels.first, 1);
        reader_m8 = correlatedChannels.second;
    }
    std::cout << "Diff M11..." << std::endl;
    {
        std::pair<VIIRSReader, VIIRSReader> correlatedChannels = correlateChannels(reader_m10, reader_m11);
        correlatedChannels.second.differentialDecode(correlatedChannels.first, 1);
        reader_m11 = correlatedChannels.second;
    }

    // Differential decoding for M14
    std::cout << "Diff M14..." << std::endl;
    {
        std::pair<VIIRSReader, VIIRSReader> correlatedChannels = correlateChannels(reader_m15, reader_m14);
        correlatedChannels.second.differentialDecode(correlatedChannels.first, 1);
        reader_m14 = correlatedChannels.second;
    }

    // Differential decoding for I2, I3
    std::cout << "Diff I2..." << std::endl;
    {
        std::pair<VIIRSReader, VIIRSReader> correlatedChannels = correlateChannels(reader_i1, reader_i2);
        correlatedChannels.second.differentialDecode(correlatedChannels.first, 1);
        reader_i2 = correlatedChannels.second;
    }
    std::cout << "Diff I3..." << std::endl;
    {
        std::pair<VIIRSReader, VIIRSReader> correlatedChannels = correlateChannels(reader_i2, reader_i3);
        correlatedChannels.second.differentialDecode(correlatedChannels.first, 1);
        reader_i3 = correlatedChannels.second;
    }

    // Differential decoding for I4 and I5
    std::cout << "Diff I4..." << std::endl;
    {
        std::pair<VIIRSReader, VIIRSReader> correlatedChannels = correlateChannels(reader_m12, reader_i4);
        correlatedChannels.second.differentialDecode(correlatedChannels.first, 2);
        reader_i4 = correlatedChannels.second;
    }
    std::cout << "Diff I5..." << std::endl;
    {
        std::pair<VIIRSReader, VIIRSReader> correlatedChannels = correlateChannels(reader_m15, reader_i5);
        correlatedChannels.second.differentialDecode(correlatedChannels.first, 2);
        reader_i5 = correlatedChannels.second;
    }

    std::cout << std::endl;

    // Write images out
    std::cout << "Writing images... (Can take a while)" << std::endl;

    cimg_library::CImg<unsigned short> image_m1 = reader_m1.getImage();
    cimg_library::CImg<unsigned short> image_m2 = reader_m2.getImage();
    cimg_library::CImg<unsigned short> image_m3 = reader_m3.getImage();
    cimg_library::CImg<unsigned short> image_m4 = reader_m4.getImage();
    cimg_library::CImg<unsigned short> image_m5 = reader_m5.getImage();
    cimg_library::CImg<unsigned short> image_m6 = reader_m6.getImage();
    cimg_library::CImg<unsigned short> image_m7 = reader_m7.getImage();
    cimg_library::CImg<unsigned short> image_m8 = reader_m8.getImage();
    cimg_library::CImg<unsigned short> image_m9 = reader_m9.getImage();
    cimg_library::CImg<unsigned short> image_m10 = reader_m10.getImage();
    cimg_library::CImg<unsigned short> image_m11 = reader_m11.getImage();
    cimg_library::CImg<unsigned short> image_m12 = reader_m12.getImage();
    cimg_library::CImg<unsigned short> image_m13 = reader_m13.getImage();
    cimg_library::CImg<unsigned short> image_m14 = reader_m14.getImage();
    cimg_library::CImg<unsigned short> image_m15 = reader_m15.getImage();
    cimg_library::CImg<unsigned short> image_m16 = reader_m16.getImage();

    cimg_library::CImg<unsigned short> image_i1 = reader_i1.getImage();
    cimg_library::CImg<unsigned short> image_i2 = reader_i2.getImage();
    cimg_library::CImg<unsigned short> image_i3 = reader_i3.getImage();
    cimg_library::CImg<unsigned short> image_i4 = reader_i4.getImage();
    cimg_library::CImg<unsigned short> image_i5 = reader_i5.getImage();

    cimg_library::CImg<unsigned short> image_dnb = reader_dnb.getImage();
    cimg_library::CImg<unsigned short> image_dnb_mgs = reader_dnb_mgs.getImage();
    cimg_library::CImg<unsigned short> image_dnb_lgs = reader_dnb_lgs.getImage();

    // Defrag, to correct those lines on the edges...
    std::cout << "Defragmenting..." << std::endl;
    defragChannel(image_m1, reader_m1.channelSettings);
    defragChannel(image_m2, reader_m2.channelSettings);
    defragChannel(image_m3, reader_m3.channelSettings);
    defragChannel(image_m4, reader_m4.channelSettings);
    defragChannel(image_m5, reader_m5.channelSettings);
    defragChannel(image_m6, reader_m6.channelSettings);
    defragChannel(image_m7, reader_m7.channelSettings);
    defragChannel(image_m8, reader_m8.channelSettings);
    defragChannel(image_m9, reader_m9.channelSettings);
    defragChannel(image_m10, reader_m10.channelSettings);
    defragChannel(image_m11, reader_m11.channelSettings);
    defragChannel(image_m12, reader_m12.channelSettings);
    defragChannel(image_m13, reader_m13.channelSettings);
    defragChannel(image_m14, reader_m14.channelSettings);
    defragChannel(image_m15, reader_m15.channelSettings);
    defragChannel(image_m16, reader_m16.channelSettings);

    defragChannel(image_i1, reader_i1.channelSettings);
    defragChannel(image_i2, reader_i2.channelSettings);
    defragChannel(image_i3, reader_i3.channelSettings);
    defragChannel(image_i4, reader_i4.channelSettings);
    defragChannel(image_i5, reader_i5.channelSettings);

    defragChannel(image_dnb, reader_dnb.channelSettings);
    defragChannel(image_dnb_lgs, reader_dnb_lgs.channelSettings);
    defragChannel(image_dnb_mgs, reader_dnb_mgs.channelSettings);

    // Correct mirrored channels
    image_i1.mirror('x');
    image_i2.mirror('x');
    image_i3.mirror('x');
    image_i4.mirror('x');
    image_i5.mirror('x');
    image_m1.mirror('x');
    image_m2.mirror('x');
    image_m3.mirror('x');
    image_m4.mirror('x');
    image_m5.mirror('x');
    image_m6.mirror('x');
    image_m7.mirror('x');
    image_m8.mirror('x');
    image_m9.mirror('x');
    image_m10.mirror('x');
    image_m11.mirror('x');
    image_m12.mirror('x');
    image_m13.mirror('x');
    image_m14.mirror('x');
    image_m15.mirror('x');
    image_m16.mirror('x');
    image_dnb.mirror('x');
    image_dnb_lgs.mirror('x');
    image_dnb_mgs.mirror('x');

    // Normalize channels, otherwise bit depth is used inconsistently between channels
    std::cout << "Normalizing imaging channels..." << std::endl;
    image_i1.normalize(0, std::numeric_limits<unsigned char>::max());
    image_i2.normalize(0, std::numeric_limits<unsigned char>::max());
    image_i3.normalize(0, std::numeric_limits<unsigned char>::max());
    image_i4.normalize(0, std::numeric_limits<unsigned char>::max());
    image_i5.normalize(0, std::numeric_limits<unsigned char>::max());

    std::cout << "Normalizing moderate channels..." << std::endl;
    image_m1.normalize(0, std::numeric_limits<unsigned char>::max());
    image_m2.normalize(0, std::numeric_limits<unsigned char>::max());
    image_m3.normalize(0, std::numeric_limits<unsigned char>::max());
    image_m4.normalize(0, std::numeric_limits<unsigned char>::max());
    image_m5.normalize(0, std::numeric_limits<unsigned char>::max());
    image_m6.normalize(0, std::numeric_limits<unsigned char>::max());
    image_m7.normalize(0, std::numeric_limits<unsigned char>::max());
    image_m8.normalize(0, std::numeric_limits<unsigned char>::max());
    image_m9.normalize(0, std::numeric_limits<unsigned char>::max());
    image_m10.normalize(0, std::numeric_limits<unsigned char>::max());
    image_m11.normalize(0, std::numeric_limits<unsigned char>::max());
    image_m12.normalize(0, std::numeric_limits<unsigned char>::max());
    image_m13.normalize(0, std::numeric_limits<unsigned char>::max());
    image_m14.normalize(0, std::numeric_limits<unsigned char>::max());
    image_m15.normalize(0, std::numeric_limits<unsigned char>::max());
    image_m16.normalize(0, std::numeric_limits<unsigned char>::max());

    std::cout << "Normalizing DNB channels..." << std::endl;
    image_dnb.normalize(0, std::numeric_limits<unsigned char>::max());
    image_dnb_lgs.normalize(0, std::numeric_limits<unsigned char>::max());
    image_dnb_mgs.normalize(0, std::numeric_limits<unsigned char>::max());

    // Histogram equalization so it doesn't look like crap
    std::cout << "Equalizing imaging channels..." << std::endl;
    image_i1.equalize(1000);
    image_i2.equalize(1000);
    image_i3.equalize(1000);
    image_i4.equalize(1000);
    image_i5.equalize(1000);

    std::cout << "Equalizing moderate channels..." << std::endl;
    image_m1.equalize(1000);
    image_m2.equalize(1000);
    image_m3.equalize(1000);
    image_m4.equalize(1000);
    image_m5.equalize(1000);
    image_m6.equalize(1000);
    image_m7.equalize(1000);
    image_m8.equalize(1000);
    image_m9.equalize(1000);
    image_m10.equalize(1000);
    image_m11.equalize(1000);
    image_m12.equalize(1000);
    image_m13.equalize(1000);
    image_m14.equalize(1000);
    image_m15.equalize(1000);
    image_m16.equalize(1000);

    std::cout << "Equalizing DNB channels..." << std::endl;
    image_dnb.equalize(1000);
    image_dnb_lgs.equalize(1000);
    image_dnb_mgs.equalize(1000);

    // Takes a while so we say how we're doing
    std::cout << "Channel M1..." << std::endl;
    image_m1.save_png("VIIRS-M1.png");

    std::cout << "Channel M2..." << std::endl;
    image_m2.save_png("VIIRS-M2.png");

    std::cout << "Channel M3..." << std::endl;
    image_m3.save_png("VIIRS-M3.png");

    std::cout << "Channel M4..." << std::endl;
    image_m4.save_png("VIIRS-M4.png");

    std::cout << "Channel M5..." << std::endl;
    image_m5.save_png("VIIRS-M5.png");

    std::cout << "Channel M6..." << std::endl;
    image_m6.save_png("VIIRS-M6.png");

    std::cout << "Channel M7..." << std::endl;
    image_m7.save_png("VIIRS-M7.png");

    std::cout << "Channel M8..." << std::endl;
    image_m8.save_png("VIIRS-M8.png");

    std::cout << "Channel M9..." << std::endl;
    image_m9.save_png("VIIRS-M9.png");

    std::cout << "Channel M10..." << std::endl;
    image_m10.save_png("VIIRS-M10.png");

    std::cout << "Channel M11..." << std::endl;
    image_m11.save_png("VIIRS-M11.png");

    std::cout << "Channel M12..." << std::endl;
    image_m12.save_png("VIIRS-M12.png");

    std::cout << "Channel M13..." << std::endl;
    image_m13.save_png("VIIRS-M13.png");

    std::cout << "Channel M14..." << std::endl;
    image_m14.save_png("VIIRS-M14.png");

    std::cout << "Channel M15..." << std::endl;
    image_m15.save_png("VIIRS-M15.png");

    std::cout << "Channel M16..." << std::endl;
    image_m16.save_png("VIIRS-M16.png");

    std::cout << "Channel I1..." << std::endl;
    image_i1.save_png("VIIRS-I1.png");

    std::cout << "Channel I2..." << std::endl;
    image_i2.save_png("VIIRS-I2.png");

    std::cout << "Channel I3..." << std::endl;
    image_i3.save_png("VIIRS-I3.png");

    std::cout << "Channel I4..." << std::endl;
    image_i4.save_png("VIIRS-I4.png");

    std::cout << "Channel I5..." << std::endl;
    image_i5.save_png("VIIRS-I5.png");

    std::cout << "Channel DNB..." << std::endl;
    image_dnb.save_png("VIIRS-DNB.png");

    std::cout << "Channel DNB-MGS..." << std::endl;
    image_dnb_mgs.save_png("VIIRS-DNB-MGS.png");

    std::cout << "Channel DNB-LGS..." << std::endl;
    image_dnb_lgs.save_png("VIIRS-DNB-LGS.png");

    // Output a few nice composites as well
    std::cout << "I221 Composite..." << std::endl;
    cimg_library::CImg<unsigned short> image221;
    {
        std::pair<VIIRSReader, VIIRSReader> correlatedChannels = correlateChannels(reader_i1, reader_i2);
        cimg_library::CImg<unsigned short> tempImage2 = correlatedChannels.second.getImage(), tempImage1 = correlatedChannels.first.getImage();
        image221 = cimg_library::CImg<unsigned short>(6400, tempImage1.height(), 1, 3);
        defragChannel(tempImage2, std::get<0>(correlatedChannels).channelSettings);
        defragChannel(tempImage1, std::get<1>(correlatedChannels).channelSettings);
        image221.draw_image(0, 0, 0, 0, tempImage2);
        image221.draw_image(0, 0, 0, 1, tempImage2);
        image221.draw_image(0, 0, 0, 2, tempImage1);
        image221.normalize(0, std::numeric_limits<unsigned char>::max());
        image221.equalize(1000);
        image221.mirror('x');
    }
    image221.save_png("VIIRS-RGB-I221.png");

    std::cout << "I312 Composite..." << std::endl;
    cimg_library::CImg<unsigned short> image321;
    {
        std::tuple<VIIRSReader, VIIRSReader, VIIRSReader> correlatedChannels = correlateThreeChannels(reader_i1, reader_i2, reader_i3);
        cimg_library::CImg<unsigned short> tempImage2 = std::get<0>(correlatedChannels).getImage(), tempImage1 = std::get<1>(correlatedChannels).getImage(), tempImage3 = std::get<2>(correlatedChannels).getImage();
        image321 = cimg_library::CImg<unsigned short>(6400, tempImage1.height(), 1, 3);
        defragChannel(tempImage2, std::get<0>(correlatedChannels).channelSettings);
        defragChannel(tempImage1, std::get<1>(correlatedChannels).channelSettings);
        defragChannel(tempImage3, std::get<2>(correlatedChannels).channelSettings);
        image321.draw_image(0, 0, 0, 0, tempImage3);
        image321.draw_image(0, 0, 0, 1, tempImage1);
        image321.draw_image(0, 0, 0, 2, tempImage2);
        image321.normalize(0, std::numeric_limits<unsigned char>::max());
        image321.equalize(1000);
        image321.mirror('x');
    }
    image321.save_png("VIIRS-RGB-I312.png");

    std::cout << "M453 Composite..." << std::endl;
    cimg_library::CImg<unsigned short> image453;
    {
        std::tuple<VIIRSReader, VIIRSReader, VIIRSReader> correlatedChannels = correlateThreeChannels(reader_m4, reader_m5, reader_m3);
        cimg_library::CImg<unsigned short> tempImage4 = std::get<0>(correlatedChannels).getImage(), tempImage5 = std::get<1>(correlatedChannels).getImage(), tempImage3 = std::get<2>(correlatedChannels).getImage();
        image453 = cimg_library::CImg<unsigned short>(3200, tempImage5.height(), 1, 3);
        defragChannel(tempImage5, std::get<1>(correlatedChannels).channelSettings);
        defragChannel(tempImage4, std::get<0>(correlatedChannels).channelSettings);
        defragChannel(tempImage3, std::get<2>(correlatedChannels).channelSettings);
        tempImage5.equalize(1000);
        tempImage4.equalize(1000);
        tempImage3.equalize(1000);
        image453.draw_image(0, 0, 0, 0, tempImage4);
        image453.draw_image(0, 0, 0, 1, tempImage5);
        image453.draw_image(0, 0, 0, 2, tempImage3);
        image453.normalize(0, std::numeric_limits<unsigned char>::max());
        image453.mirror('x');
    }
    image453.save_png("VIIRS-RGB-M453.png");

    std::cout << "M543 Composite..." << std::endl;
    cimg_library::CImg<unsigned short> image543;
    {
        std::tuple<VIIRSReader, VIIRSReader, VIIRSReader> correlatedChannels = correlateThreeChannels(reader_m4, reader_m5, reader_m3);
        cimg_library::CImg<unsigned short> tempImage4 = std::get<0>(correlatedChannels).getImage(), tempImage5 = std::get<1>(correlatedChannels).getImage(), tempImage3 = std::get<2>(correlatedChannels).getImage();
        image543 = cimg_library::CImg<unsigned short>(3200, tempImage5.height(), 1, 3);
        defragChannel(tempImage5, std::get<1>(correlatedChannels).channelSettings);
        defragChannel(tempImage4, std::get<0>(correlatedChannels).channelSettings);
        defragChannel(tempImage3, std::get<2>(correlatedChannels).channelSettings);
        tempImage5.equalize(1000);
        tempImage4.equalize(1000);
        tempImage3.equalize(1000);
        image543.draw_image(0, 0, 0, 0, tempImage5);
        image543.draw_image(0, 0, 0, 1, tempImage4);
        image543.draw_image(0, 0, 0, 2, tempImage3);
        image543.normalize(0, std::numeric_limits<unsigned char>::max());
        image543.mirror('x');
    }
    image543.save_png("VIIRS-RGB-M543.png");

    data_in.close();
}
