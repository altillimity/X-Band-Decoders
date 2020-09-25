#include <iostream>
#include <fstream>
#include <complex>
#include <vector>
#include <cstring>
#include <iomanip>
#include "reedsolomon.h"
#include "differentialencoding.h"
#include "deframer.h"

/*
Yes, this will need a cleanup as well! Could be worse...
*/

// Return filesize
size_t getFilesize(std::string filepath)
{
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    std::size_t fileSize = file.tellg();
    file.close();
    return fileSize;
}

// Processing buffer size
#define BUFFER_SIZE (1024 * 8)

// Returns the asked bit!
template <typename T>
inline bool getBit(T data, int bit)
{
    return (data >> bit) & 1;
}

// Clamp symbols
int8_t clamp(int8_t &x)
{
    if (x >= 0)
    {
        return 1;
    }
    if (x <= -1)
    {
        return -1;
    }
    return x > 255.0 / 2.0;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cout << "Usage : " << argv[0] << " softsymbols.bin outputframes.bin" << std::endl;
        return 1;
    }

    // Output and Input file
    std::ifstream data_in(argv[1], std::ios::binary);
    std::ofstream data_out(argv[2], std::ios::binary);

    // Read buffer
    uint8_t buffer[BUFFER_SIZE];

    // Complete filesize
    size_t filesize = getFilesize(argv[1]);

    // Data we wrote out
    size_t data_out_total = 0;

    // CADU Stuff
    SatHelper::DifferentialEncoding diff;
    SatHelper::ReedSolomon reedSolomon;
    CADUDeframer deframer;

    // I/Q Buffers
    uint8_t decodedBufI[BUFFER_SIZE / 2];
    uint8_t decodedBufQ[BUFFER_SIZE / 2];
    uint8_t bufI[(BUFFER_SIZE / 8) / 2];
    uint8_t bufQ[(BUFFER_SIZE / 8) / 2];

    // Final buffer after decoding
    uint8_t finalBuffer[(BUFFER_SIZE / 8)];

    // Buffer for RS decoding
    uint8_t rsWorkBuffer[255];

    // Bits => Bytes stuff
    uint8_t byteShifter;
    int inByteShifter = 0;
    int inFinalByteBuffer;

    // Graphics
    std::cout << "---------------------------" << std::endl;
    std::cout << "  AQUA Decoder by Aang23" << std::endl;
    std::cout << "---------------------------" << std::endl;
    std::cout << std::endl;

    // Read until EOF
    while (!data_in.eof())
    {
        // Read buffer
        data_in.read((char *)buffer, BUFFER_SIZE);

        // Demodulate QPSK... This is the crappy way but it works
        for (int i = 0; i < BUFFER_SIZE / 2; i++)
        {
            int8_t sample_i = clamp(*((int8_t *)&buffer[i * 2]));
            int8_t sample_q = clamp(*((int8_t *)&buffer[i * 2 + 1]));

            if (sample_i == -1 && sample_q == -1)
            {
                decodedBufQ[i] = 0;
                decodedBufI[i] = 0;
            }
            else if (sample_i == 1 && sample_q == -1)
            {
                decodedBufQ[i] = 0;
                decodedBufI[i] = 1;
            }
            else if (sample_i == 1 && sample_q == 1)
            {
                decodedBufQ[i] = 1;
                decodedBufI[i] = 1;
            }
            else if (sample_i == -1 && sample_q == 1)
            {
                decodedBufQ[i] = 1;
                decodedBufI[i] = 0;
            }
        }

        // Group symbols into bytes now, I channel
        inFinalByteBuffer = 0;
        inByteShifter = 0;
        for (int i = 0; i < BUFFER_SIZE / 2; i++)
        {
            byteShifter = byteShifter << 1 | decodedBufI[i];
            inByteShifter++;

            if (inByteShifter == 8)
            {
                bufI[inFinalByteBuffer++] = byteShifter;
                inByteShifter = 0;
            }
        }

        // Group symbols into bytes now, Q channel
        inFinalByteBuffer = 0;
        inByteShifter = 0;
        for (int i = 0; i < BUFFER_SIZE / 2; i++)
        {
            byteShifter = byteShifter << 1 | decodedBufQ[i];
            inByteShifter++;

            if (inByteShifter == 8)
            {
                bufQ[inFinalByteBuffer++] = byteShifter;
                inByteShifter = 0;
            }
        }

        // Differential decoding for both of them
        diff.nrzmDecode(bufI, (BUFFER_SIZE / 8) / 2);
        diff.nrzmDecode(bufQ, (BUFFER_SIZE / 8) / 2);

        // Interleave them back
        for (int i = 0; i < (BUFFER_SIZE / 8) / 2; i++)
        {
            finalBuffer[i * 2] = getBit<uint8_t>(bufI[i], 7) << 7 |
                                 getBit<uint8_t>(bufQ[i], 7) << 6 |
                                 getBit<uint8_t>(bufI[i], 6) << 5 |
                                 getBit<uint8_t>(bufQ[i], 6) << 4 |
                                 getBit<uint8_t>(bufI[i], 5) << 3 |
                                 getBit<uint8_t>(bufQ[i], 5) << 2 |
                                 getBit<uint8_t>(bufI[i], 4) << 1 |
                                 getBit<uint8_t>(bufQ[i], 4) << 0;
            finalBuffer[i * 2 + 1] = getBit<uint8_t>(bufI[i], 3) << 7 |
                                     getBit<uint8_t>(bufQ[i], 3) << 6 |
                                     getBit<uint8_t>(bufI[i], 2) << 5 |
                                     getBit<uint8_t>(bufQ[i], 2) << 4 |
                                     getBit<uint8_t>(bufI[i], 1) << 3 |
                                     getBit<uint8_t>(bufQ[i], 1) << 2 |
                                     getBit<uint8_t>(bufI[i], 0) << 1 |
                                     getBit<uint8_t>(bufQ[i], 0) << 0;
        }

        // Deframe that! (Integrated derand)
        std::vector<std::array<uint8_t, CADU_SIZE>> frameBuffer = deframer.work(finalBuffer, (BUFFER_SIZE / 8));

        // If we found frames, write them out
        if (frameBuffer.size() > 0)
        {
            for (std::array<uint8_t, CADU_SIZE> cadu : frameBuffer)
            {
                // RS Correction
                int errors = 0;
                for (int i = 0; i < 4; i++)
                {
                    reedSolomon.deinterleave(&cadu[4], rsWorkBuffer, i, 4);
                    errors = reedSolomon.decode_ccsds(rsWorkBuffer);
                    reedSolomon.interleave(rsWorkBuffer, &cadu[4], i, 4);
                }

                // Write it to our output file!
                data_out_total += CADU_SIZE;
                data_out.write((char *)&cadu, CADU_SIZE);
            }
        }

        // Console stuff
        if (deframer.getState() == 0)
            std::cout << "\r State : NOSYNC  " << std::flush;
        else if (deframer.getState() == 2 | deframer.getState() == 6)
            std::cout << "\r State : SYNCING " << std::flush;
        else if (deframer.getState() > 6)
            std::cout << "\r State : SYNCED  " << std::flush;
        std::cout << ", CADUs : " << (float)(data_out_total / 1024) << ", Data out : " << round(data_out_total / 1e5) / 10.0f << " MB, Progress : " << round(((float)data_in.tellg() / (float)filesize) * 1000.0f) / 10.0f << "%     " << std::flush;
    }

    data_in.close();
    data_out.close();
}