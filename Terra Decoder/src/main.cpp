#include <iostream>
#include <fstream>
#include <complex>
#include <vector>
#include <cstring>
#include <iomanip>
#include "correlator.h"
#include "packetfixer.h"
#include "viterbi27.h"
#include "derandomizer.h"
#include "reedsolomon.h"
#include "differentialencoding.h"

#define FRAME_SIZE 1024
#define ENCODED_FRAME_SIZE 1024 * 8 * 2

// Return filesize
size_t getFilesize(std::string filepath)
{
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    std::size_t fileSize = file.tellg();
    file.close();
    return fileSize;
}

void shiftWithConstantSize(uint8_t *arr, int pos, int length)
{
    for (int i = 0; i < length - pos; i++)
    {
        arr[i] = arr[pos + i];
    }
}

// Small function that returns 1 bit from any type
template <typename T>
inline bool getBit(T data, int bit)
{
    return (data >> bit) & 1;
}

// Processing buffer size
#define BUFFER_SIZE (ENCODED_FRAME_SIZE)

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cout << "Usage : " << argv[0] << " inputsoftsymbols.bin outputframes.bin" << std::endl;
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

    // Correlator to find the sync word
    SatHelper::Correlator correlator;

    // All possible sync words
    correlator.addWord((uint64_t)0xfc4ef4fd0cc2df89);
    correlator.addWord((uint64_t)0x56275254a66b45ec);
    correlator.addWord((uint64_t)0x03b10b02f33d2076);
    correlator.addWord((uint64_t)0xa9d8adab5994ba89);

    correlator.addWord((uint64_t)0xfc8df8fe0cc1ef46);
    correlator.addWord((uint64_t)0xa91ba1a859978adc);
    correlator.addWord((uint64_t)0x03720701f33e1089);
    correlator.addWord((uint64_t)0x56e45e57a6687546);

    // LibSatHelper objects
    SatHelper::PhaseShift phaseShift;
    SatHelper::PacketFixer packetFixer;
    SatHelper::Viterbi27 viterbi(8192);
    SatHelper::DeRandomizer derand;
    SatHelper::ReedSolomon reedSolomon;
    SatHelper::DifferentialEncoding diff;

    uint8_t postViterbiWorkBuffer[1024];

    // Locking mecanism
    bool locked = false;
    int syncedRunsCount = 0;

    // Graphics
    std::cout << "---------------------------" << std::endl;
    std::cout << " TERRA Decoder by Aang23" << std::endl;
    std::cout << "  Based on libsathelper" << std::endl;
    std::cout << "      by Luigi Cruz" << std::endl;
    std::cout << "---------------------------" << std::endl;
    std::cout << std::endl;

    // RS
    uint8_t rsWorkBuffer[255];

    bool iqinv;

    // Read until EOF
    while (!data_in.eof())
    {
        // Read buffer
        data_in.read((char *)buffer, BUFFER_SIZE);

        // Locked? If so, correlate less to speed things up
        if (!locked)
            correlator.correlate(buffer, ENCODED_FRAME_SIZE);
        else
        {
            correlator.correlate(buffer, ENCODED_FRAME_SIZE / 64);
            if (correlator.getHighestCorrelationPosition() != 0)
                correlator.correlate(buffer, ENCODED_FRAME_SIZE);
        }

        // Correlation statistics
        uint32_t cor = correlator.getHighestCorrelation();
        uint32_t word = correlator.getCorrelationWordNumber();
        uint32_t pos = correlator.getHighestCorrelationPosition();

        if (cor > 46)
        {
            // Depending on which sync word correlated correctly, guess the current phase shift
            iqinv = (word / 4) > 0;
            switch (word % 4)
            {
            case 0:
                phaseShift = SatHelper::PhaseShift::DEG_0;
                break;

            case 1:
                phaseShift = SatHelper::PhaseShift::DEG_90;
                break;

            case 2:
                phaseShift = SatHelper::PhaseShift::DEG_180;
                break;

            case 3:
                phaseShift = SatHelper::PhaseShift::DEG_270;
                break;

            default:
                break;
            }

            // Fill up a "frame"
            if (pos != 0)
            {
                shiftWithConstantSize(buffer, pos, ENCODED_FRAME_SIZE);
                uint32_t offset = ENCODED_FRAME_SIZE - pos;
                uint8_t buffer_2[pos];

                data_in.read((char *)buffer_2, pos);

                for (int i = offset; i < ENCODED_FRAME_SIZE; i++)
                {
                    buffer[i] = buffer_2[i - offset];
                }

                syncedRunsCount = 0;
            }
            else
            {
                syncedRunsCount++;
                if (syncedRunsCount == 5)
                    locked = true;
            }

            // Correct phase
            packetFixer.fixPacket(buffer, ENCODED_FRAME_SIZE, phaseShift, iqinv);

            // Viterbi decoding
            viterbi.decode(buffer, postViterbiWorkBuffer);

            // Differential decoding
            diff.nrzmDecode(postViterbiWorkBuffer, FRAME_SIZE);

            // Hm, Terra is weird on there
            derand.DeRandomize(&postViterbiWorkBuffer[10], FRAME_SIZE - 9);

            // Reed-Solomon
            int errors = 0;
            for (int i = 0; i < 4; i++)
            {
                reedSolomon.deinterleave(&postViterbiWorkBuffer[4], rsWorkBuffer, i, 4);
                errors = reedSolomon.decode_rs8(rsWorkBuffer);
                reedSolomon.interleave(rsWorkBuffer, &postViterbiWorkBuffer[4], i, 4);
            }

            // Output this
            data_out_total += FRAME_SIZE;
            data_out.write((char *)postViterbiWorkBuffer, FRAME_SIZE);
        }
        else
        {
            locked = false;
            syncedRunsCount = 0;
        }

        // Console stuff
        uint32_t current_asm = postViterbiWorkBuffer[0] << 24 |
                               postViterbiWorkBuffer[1] << 16 |
                               postViterbiWorkBuffer[2] << 8 |
                               postViterbiWorkBuffer[3];
        std::cout << '\r' << ("State : " + (std::string)(locked ? "SYNCED" : "NOSYNC")) << ", ASM : " << std::setfill('0') << std::setw(8) << std::hex << current_asm << ", CADUs : " << (float)(data_out_total / 1024) << ", Data out : " << round(data_out_total / 1e5) / 10.0f << " MB, Progress : " << round(((float)data_in.tellg() / (float)filesize) * 1000.0f) / 10.0f << "%     " << std::flush;
    }

    data_in.close();
    data_out.close();
}