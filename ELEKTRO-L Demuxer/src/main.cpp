#include <iostream>
#include <fstream>
#include <cstdint>
#include <math.h>
#include <cstring>
#include "deframer.h"
#include "simpledeframer.h"
#include <algorithm>

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

// Modis buffer, only handling day frames right now...
uint8_t modisDayBuffer[642];
int inModisDayBuffer = 0;

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cout << "Usage : " << argv[0] << " modisCadu.bin modisFrames.bin" << std::endl;
        return 0;
    }

    CADUDeframer deframer;

    // Complete filesize
    size_t filesize = getFilesize(argv[argc - 2]);

    // Output and Input file
    data_in = std::ifstream(argv[1], std::ios::binary);
    data_out = std::ofstream(argv[2], std::ios::binary);

    // Read buffer
    uint8_t buffer[1024];

    // Counters
    uint64_t total_cadu = 0, modis_cadu = 0, ccsds = 0;

    // Graphics
    std::cout << "---------------------------" << std::endl;
    std::cout << "ELEKTRO-L Demuxer by Aang23" << std::endl;
    std::cout << "---------------------------" << std::endl;
    std::cout << std::endl;

    std::vector<int> vcids;

    SimpleDeframer<uint64_t, 64, 121680, 0x0218a7a392dd9abf> msuDefra;

    // Read until EOF
    while (!data_in.eof())
    {
        // Read buffer
        data_in.read((char *)buffer, 1024);

        total_cadu++;

        int vcid = buffer[5];

        if (!std::count(vcids.begin(), vcids.end(), vcid))
            vcids.push_back(vcid);

        //std::cout << vcid << std::endl;

        // Right channel? (VCID 30 is MODIS)
        if (vcid == 90)
        {
            modis_cadu++;

            int todel = 24;
            //data_out.write((char *)&buffer[todel], 1024 - todel);

            std::vector<uint8_t> defraVec;
            defraVec.insert(defraVec.end(), &buffer[todel], &buffer[todel] + (1024 - todel));

            std::vector<std::vector<uint8_t>> msu = msuDefra.work(defraVec);

            ccsds += msu.size();

            for(std::vector<uint8_t>& frame : msu)
                for(uint8_t& byte : frame)
                    data_out.put(byte);
            //data_out.write((char *)&cadu[0], 1024);
        }

        // While this may seem kinda dumb, it made an insane speed difference!
        if (total_cadu % 1000)
            std::cout << "\rCADUs : " << total_cadu << ", MODIS (VCID 30) CADUs : " << modis_cadu << ", CCSDS : " << ccsds << ", Progress : " << round(((float)data_in.tellg() / (float)filesize) * 1000.0f) / 10.0f << "%     " << std::flush;
    }

    for (int vcid : vcids)
        std::cout << vcid << std::endl;

    data_in.close();
    data_out.close();
}
