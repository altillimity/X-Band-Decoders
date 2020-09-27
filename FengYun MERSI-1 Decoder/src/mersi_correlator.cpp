#include "mersi_correlator.h"

MERSICorrelator::MERSICorrelator()
{
    checking = false;
    m250Frames = 0;
    m1000Frames = 0;
}

// Export decoded data
void MERSICorrelator::makeImages()
{
    image1 = reader1.getImage();
    image2 = reader2.getImage();
    image3 = reader3.getImage();
    image4 = reader4.getImage();
    image5 = reader5.getImage();
    image6 = reader6.getImage();
    image7 = reader7.getImage();
    image8 = reader8.getImage();
    image9 = reader9.getImage();
    image10 = reader10.getImage();
    image11 = reader11.getImage();
    image12 = reader12.getImage();
    image13 = reader13.getImage();
    image14 = reader14.getImage();
    image15 = reader15.getImage();

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
}

void MERSICorrelator::processScan()
{
    for (std::vector<uint8_t> &frameVec : scanBuffer)
    {
        int marker = (frameVec[3] % (int)pow(2, 3)) << 7 | frameVec[4] >> 1;

        if (marker > 199)
        {
            m1000Frames++;

            // Demultiplex them all!
            if (marker > 39 + 40 * 5 && marker < 39 + 40 * 5 + 10)
                reader6.pushFrame(frameVec);
            else if (marker > 39 + 40 * 5 + 10 * 2 && marker < 39 + 40 * 5 + 10 * (2 + 1))
                reader7.pushFrame(frameVec);
            else if (marker > 39 + 40 * 5 + 10 * 3 && marker < 39 + 40 * 5 + 10 * (3 + 1))
                reader8.pushFrame(frameVec);
            else if (marker > 39 + 40 * 5 + 10 * 4 && marker < 39 + 40 * 5 + 10 * (4 + 1))
                reader9.pushFrame(frameVec);
            else if (marker > 39 + 40 * 5 + 10 * 5 && marker < 39 + 40 * 5 + 10 * (5 + 1))
                reader10.pushFrame(frameVec);
            else if (marker > 39 + 40 * 5 + 10 * 6 && marker < 39 + 40 * 5 + 10 * (6 + 1))
                reader11.pushFrame(frameVec);
            else if (marker > 39 + 40 * 5 + 10 * 7 && marker < 39 + 40 * 5 + 10 * (7 + 1))
                reader12.pushFrame(frameVec);
            else if (marker > 39 + 40 * 5 + 10 * 8 && marker < 39 + 40 * 5 + 10 * (8 + 1))
                reader13.pushFrame(frameVec);
            else if (marker > 39 + 40 * 5 + 10 * 9 && marker < 39 + 40 * 5 + 10 * (9 + 1))
                reader14.pushFrame(frameVec);
            else if (marker > 39 + 40 * 5 + 10 * 10 && marker < 39 + 40 * 5 + 10 * (10 + 1))
                reader15.pushFrame(frameVec);
        }
        else if (marker <= 199)
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
        }
    }

    scanBuffer.clear();
}

void MERSICorrelator::feedFrames(int marker, std::vector<uint8_t> &data)
{
    // Scan start
    if (marker == 0) // First frame has marker 0
    {
        checking = true;
        lastMarker = marker;
        scanBuffer.push_back(data); // Buffer it
        return;
    }

    if (checking)
    {
        if (lastMarker + 1 == marker)
        {
            lastMarker++;
            scanBuffer.push_back(data); // Buffer it

            // Complete! Let's read it.
            if (marker == 349)
            {
                processScan();
                complete++;
            }
        }
        else // The scan is incomplete, discard
        {
            incomplete++;
            scanBuffer.clear();
            checking = false;
        }
    }
}