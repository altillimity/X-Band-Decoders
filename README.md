# X-Band Decoders

A few projects aimed at decoding various X-Band satellites

### Dependencies

Some projects will require (or / and) :
- [libccsds](https://github.com/altillimity/libccsds)
- [libcorrect](https://github.com/quiet/libcorrect)
- [libpng](https://github.com/glennrp/libpng) + [zlib](https://github.com/madler/zlib)
- [libaec](https://github.com/opensatelliteproject/libaec)

The flowcharts require GNU Radio 3.8 or above.

### Standalone Demodulators

All flowcharts contained in this repository can be substitued with those standalone demodulators. It will be easier and faster to use for most users https://github.com/altillimity/Standalone-Demodulators.

# ELEKTRO-L 2/3

**Supported downlink :** RDAS, 7500Mhz    
**Modulation :** BPSK  
**Symbolrate :** 30.72Mbps  
**Recording bandwidth :** >= 31MSPS, something like 40MSPS is preferred  

**Decoding :**
- Record a baseband of a full (better!) dump
- Demodulate with ELEKTRO-L Demodulator
- Convert it to synced, derandomized CADUs with the CADU Synchroderand
- Process to get MSU-GS data (imagery) with ELEKTRO-L Decoder  

# Aqua

**Supported downlink :** DB, 8160Mhz    
**Modulation :** SPQPSK   
**Symbolrate :** 15Mbps  
**Recording bandwidth :** >= 15MSPS 

**Decoding :**
- Record a baseband 
- Demodulate with AQUA Demodulator
- Process the soft symbols with Aqua Decoder   
- Run the resulting CADU file through MODIS Decoder     
- Run the resulting CADU file through Aqua AIRS Decoder (for AIRS instrument data)   
- Run the resulting CADU file through CERES Decoder (for CERES instrument data)   
- Run the resulting CADU file through Aqua AMSU Decoder (for AMSU-A1/A2 instrument data)     

# Terra

**Supported downlink :** DB, 8160Mhz    
**Modulation :** SPQPSK, 4:1, Q Channel  
**Symbolrate :** 26.25Mbps  
**Recording bandwidth :** >= 27MSPS, 45MSPS or so preferred 

**Decoding :**
- Record a baseband 
- Demodulate with TERRA Demodulator
- Process the soft symbols with Terra Decoder   
- Run the resulting CADU file through MODIS Decoder in Terra mode  
- Run the resulting CADU file through CERES Decoder in Terra mode (for CERES instrument data)       

# FengYun-3 A/B/C

**Supported downlink :** MPT, 7775Mhz (3C 7780Mhz)    
**Modulation :** QPSK  
**Symbolrate :** 18.7Mbps  
**Recording bandwidth :** >= 19MSPS, 30MSPS or so preferred 

**Decoding :**
- Record a baseband 
- Demodulate with MPT Demodulator
- Process the soft symbols with FengYun MPT Decoder   
- Run the resulting CADU file through the MERSI-1 Decoder  

# FengYun-3 D

**Supported downlink :** AHRPT, 7820Mhz  
**Modulation :** QPSK  
**Symbolrate :** 30Mbps  
**Recording bandwidth :** >= 30MSPS, 45MSPS or so preferred 

**Decoding :**
- Record a baseband 
- Demodulate with FengYun 3D Demodulator
- Process the soft symbols with FengYun 3D Decoder   
- Run the resulting CADU file through the MERSI-2 Decoder  

# Suomi NPP / JPSS-1 (NOAA 20)

**Supported downlink :** HRD, 7812Mhz  
**Modulation :** QPSK  
**Symbolrate :** 15Mbps  
**Recording bandwidth :** >= 15MSPS, 25MSPS or so preferred 

**Decoding :**
- Record a baseband 
- Demodulate with HRD Demodulator
- Process the soft symbols with HRD Decoder   
- Run the resulting CADU file through the VIIRS Decoder to get VIIRS (imager) data    
- Run the resulting CADU file through the ATMS Decoder to get ATMS (microwave sounder) data    
