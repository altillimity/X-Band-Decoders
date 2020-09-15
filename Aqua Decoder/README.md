# Aqua Decoder

Small program taking demodulated soft symbols from the Aqua satellites and outputs CADU frames out.

Usage : `./AQUA-Decoder softsymbols.bin outputframes.bin`

The differential decoder and RS (using libcorrect in the background) are both from https://github.com/opensatelliteproject/libsathelper.