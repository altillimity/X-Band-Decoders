# Terra Decoder

Small program taking demodulated soft symbols from the Terra satellites and outputs CADU frames out.

Usage : `./TERRA-Decoder softsymbols.bin outputframes.bin`

The differential decoder, correlator, viterbi wrapper and RS (using libcorrect in the background) are both from https://github.com/opensatelliteproject/libsathelper.