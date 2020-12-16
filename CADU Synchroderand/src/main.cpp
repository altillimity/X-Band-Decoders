#include <iostream>
#include <fstream>
#include <cstdint>
#include <math.h>
#include <cstring>
#include "deframer.h"

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
    if (argc != 3 && argc != 4)
    {
        std::cout << "Usage : " << argv[0] << " unsynchedrand.bin syncderand.bin [-t]" << std::endl;
        return 0;
    }

    bool terra = false;

    // Terra mode?
    if (argc == 4)
    {
        if (std::string(argv[3]) == "-t")
            terra = true;
    }

    CADUDeframer deframer(terra);

    // Complete filesize
    size_t filesize = getFilesize(argv[1]);

    // Output and Input file
    data_in = std::ifstream(argv[1], std::ios::binary);
    data_out = std::ofstream(argv[2], std::ios::binary);

    // Read buffer
    uint8_t buffer[1024];

    // Counters
    uint64_t total_cadu = 0;

    // Graphics
    std::cout << "---------------------------" << std::endl;
    std::cout << "   CADU Deframer and" << std::endl;
    std::cout << "derandomizer by Aang23" << std::endl;
    std::cout << "---------------------------" << std::endl;
    std::cout << std::endl;

    // Read until EOF
    while (!data_in.eof())
    {
        // Read buffer
        data_in.read((char *)buffer, 1024);

        std::vector<std::array<uint8_t, CADU_SIZE>> frameBuffer = deframer.work(buffer, 1024);

        // If we found frames, write them out
        if (frameBuffer.size() > 0)
        {

            for (std::array<uint8_t, CADU_SIZE> cadu : frameBuffer)
            {
                total_cadu++;
                data_out.write((char *)&cadu[0], 1024);
            }
        }

        // Console
        if (deframer.getState() == 0)
            std::cout << "\r State : NOSYNC  " << std::flush;
        else if (deframer.getState() == 2 | deframer.getState() == 6)
            std::cout << "\r State : SYNCING " << std::flush;
        else if (deframer.getState() > 6)
            std::cout << "\r State : SYNCED  " << std::flush;
        std::cout << "CADUs : " << total_cadu << ", Progress : " << round(((float)data_in.tellg() / (float)filesize) * 1000.0f) / 10.0f << "%     " << std::flush;
    }

    data_in.close();
    data_out.close();
}
