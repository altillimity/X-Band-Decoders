#include <iostream>
#include <fstream>
#include <cstdint>
#include <math.h>
#include <cstring>

/*
Very basic MODIS frame extractor, currently handling handling day properly.
This will be entirely recoded in the future to handle CCSDS routines as they should
*/

// Return filesize
size_t getFilesize(std::string filepath)
{
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    std::size_t fileSize = file.tellg();
    file.close();
    return fileSize;
}

// Convenience only
typedef uint8_t CADU[1024];

struct VCDU
{
    uint8_t version;
    uint8_t spacecraft_id;
    uint8_t vcid;
    uint32_t vcdu_counter;
    bool replay_flag;
};

struct MPDU
{
    uint16_t first_header_pointer;
    uint8_t *data;
};

struct CCSDSHeader
{
    uint8_t version;
    bool type;
    bool secondary_header_flag;
    uint16_t apid;
    uint8_t sequence_flag;
    uint16_t packet_sequence_count;
    uint16_t packet_length;
};

// Parse VCDU
VCDU parseVCDU(CADU &cadu)
{
    uint8_t version = cadu[4] >> 6;
    uint8_t spacecraft_id = (cadu[4] % (int)pow(2, 6)) << 2 | cadu[5] >> 6;
    uint8_t vcid = cadu[5] % (int)pow(2, 6);
    uint32_t vcdu_counter = cadu[6] << 16 | cadu[7] << 8 | cadu[8];
    bool replay_flag = cadu[9] >> 7;
    return {version, spacecraft_id, vcid, vcdu_counter, replay_flag};
}

// Parse MPDU
MPDU parseMPDU(CADU &cadu)
{
    uint16_t first_header_pointer = (cadu[10] % (int)pow(2, 3)) << 8 | cadu[11];
    return {first_header_pointer, &cadu[12]};
}

// Parse CCSDS header
CCSDSHeader parseCCSDSHeader(uint8_t *header)
{
    uint8_t version = header[0] >> 5;
    bool type = (header[0] >> 4) % 2;
    bool secondary_header_flag = (header[0] >> 3) % 2;
    uint16_t apid = (header[0] % (int)pow(2, 3)) << 8 | header[1];
    uint8_t sequence_flag = header[2] >> 6;
    uint16_t packet_sequence_count = (header[2] % (int)pow(2, 6)) << 8 | header[3];
    uint16_t packet_length = header[4] << 8 | header[5];
    return {version, type, secondary_header_flag, apid, sequence_flag, packet_sequence_count, packet_length};
}

// IO files
std::ifstream data_in;
std::ofstream data_out;

// Modis buffer, only handling day frames right now...
uint8_t modisDayBuffer[642];
int inModisDayBuffer = 0;

// Write MODIS frame to output
void writePacket()
{
    data_out.write((char *)modisDayBuffer, 642);
    inModisDayBuffer = 0;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cout << "Usage : " << argv[0] << " modisCadu.bin modisFrames.bin" << std::endl;
        return 0;
    }

    // Complete filesize
    size_t filesize = getFilesize(argv[argc - 2]);

    // Output and Input file
    data_in = std::ifstream(argv[1], std::ios::binary);
    data_out = std::ofstream(argv[2], std::ios::binary);

    // Read buffer
    CADU cadu;

    // Counters
    uint64_t total_cadu = 0, modis_cadu = 0, ccsds = 0;

    // Graphics
    std::cout << "---------------------------" << std::endl;
    std::cout << " MODIS Extractor by Aang23" << std::endl;
    std::cout << "---------------------------" << std::endl;
    std::cout << std::endl;

    // Read until EOF
    while (!data_in.eof())
    {
        // Read buffer
        data_in.read((char *)cadu, 1024);
        total_cadu++;

        // Parse this transport frame
        VCDU vcdu = parseVCDU(cadu);

        // Right channel? (VCID 30 is MODIS)
        if (vcdu.vcid == 30)
        {
            modis_cadu++;

            // Parse MPDU header
            MPDU mpdu = parseMPDU(cadu);

            // If this at least our second run and there's still stuff to write from previous frame, do it.
            if (ccsds > 0 && 642 - inModisDayBuffer > 0)
            {
                int toWrite = 642 - inModisDayBuffer;
                //std::cout << toWrite << std::endl;
                std::memcpy(&modisDayBuffer[inModisDayBuffer], &mpdu.data[0], toWrite);
                inModisDayBuffer += toWrite;
            }

            // Write frame out if we ran at least once
            if (ccsds > 0)
                writePacket();

            // Parse first CCSDS header
            int firstHeaderPointer = mpdu.first_header_pointer;
            CCSDSHeader firstHeader = parseCCSDSHeader(&mpdu.data[firstHeaderPointer]);

            // Check if this frames contains more than one CCSDS header
            bool hasSecondHeader = 884 >= firstHeaderPointer + firstHeader.packet_length + 7;

            // 2 Headers
            if (hasSecondHeader)
            {
                // Compute second header
                int secondHeaderPointer = firstHeaderPointer + firstHeader.packet_length + 7;
                CCSDSHeader secondHeader = parseCCSDSHeader(&mpdu.data[secondHeaderPointer]);
                //std::cout << firstHeader.apid << " " << secondHeader.apid << std::endl;

                // Write first frame out if it matches
                if (firstHeader.apid == 64 && firstHeaderPointer < 884)
                {
                    std::memcpy(modisDayBuffer, &mpdu.data[firstHeaderPointer], 642);
                    inModisDayBuffer += 642;
                    writePacket();
                    ccsds++;
                }

                // Write second frame out if it matches
                if (secondHeader.apid == 64 && secondHeaderPointer < 884)
                {
                    std::memcpy(modisDayBuffer, &mpdu.data[secondHeaderPointer], 884 - secondHeaderPointer);
                    inModisDayBuffer += 884 - secondHeaderPointer;
                    ccsds++;
                }
            }
            else
            {
                // Write frame out if it matches
                if (firstHeader.apid == 64 && firstHeaderPointer < 884)
                {
                    std::cout << 884 - firstHeaderPointer << std::endl;
                    //std::cout << firstHeader.apid << std::endl;
                    std::memcpy(modisDayBuffer, &mpdu.data[firstHeaderPointer], 884 - firstHeaderPointer);
                    inModisDayBuffer = 884 - firstHeaderPointer; // Supposed to be += but compiler bug!?
                    ccsds++;
                }
            }
        }

        // While this may seem kinda dumb, it made an insane speed difference!
        if (total_cadu % 1000)
            std::cout << "\rCADUs : " << total_cadu << ", MODIS (VCID 30) CADUs : " << modis_cadu << ", CCSDS : " << ccsds << ", Progress : " << round(((float)data_in.tellg() / (float)filesize) * 1000.0f) / 10.0f << "%     " << std::flush;
    }

    data_in.close();
    data_out.close();
}
