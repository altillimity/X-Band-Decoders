# FengYun 3D Decoder

Small program made to decode (Viterbi / Diff / Deframe / Derand / RS) X-Band data from the FY3D satellite. It will output frames from soft symbols.

Usage : `./FY3DDecoder symbols.bin outputframes.bin`

The differential decoder and RS (using libcorrect in the background) are both from https://github.com/opensatelliteproject/libsathelper.