# HRD Decoder

Small program taking demodulated soft symbols from the HRD downlinks and outputs CADU frames out.

Usage : `./HRD-Decoder softsymbols.bin outputframes.bin`

The differential decoder, correlator, viterbi wrapper and RS (using libcorrect in the background) are both from https://github.com/opensatelliteproject/libsathelper.