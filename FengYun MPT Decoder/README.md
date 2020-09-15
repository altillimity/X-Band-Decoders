# FengYun MPT Decoder

Program that can decode FengYun MPT (A/B/C) downlinks from hard symbols to frames.

Usage : `./FengYun-MPT-Decoder symbols.bin outputframes.bin`

The differential decoder and RS (using libcorrect in the background) are both from https://github.com/opensatelliteproject/libsathelper.