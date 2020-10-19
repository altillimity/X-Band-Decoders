#pragma once

#include "channel_reader.h"

/*
Simple correlator between VIIRS channels only keeping common segments
That's required for composites and proper differential decoding
*/

std::pair<VIIRSReader, VIIRSReader> correlateChannels(VIIRSReader segments1, VIIRSReader segments2); // Correlate 2 channels
std::tuple<VIIRSReader, VIIRSReader, VIIRSReader> correlateThreeChannels(VIIRSReader segments1, VIIRSReader segments2, VIIRSReader segments3); // Correlate 3 channels