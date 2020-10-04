cmake_minimum_required (VERSION 3.0.0)

project (MODIS-Decoder)
file(GLOB_RECURSE MODIS-Decoder_CPPS src/*.cpp)

add_executable(MODIS-Decoder ${MODIS-Decoder_CPPS})

target_include_directories(MODIS-Decoder PUBLIC src)

target_link_libraries(MODIS-Decoder PUBLIC m)

find_library(AEC_LIBRARY aec HINTS /usr/lib REQUIRED)
target_link_libraries(MODIS-Decoder PUBLIC ${AEC_LIBRARY})

find_library(CCSDS_LIBRARY ccsds REQUIRED)
target_link_libraries(MODIS-Decoder PUBLIC ${CCSDS_LIBRARY})

if(WIN32 AND NOT MINGW)
    find_package(PNG CONFIG REQUIRED)
else()
    find_package(PNG REQUIRED)
endif()
target_link_libraries(MODIS-Decoder PUBLIC PNG::PNG)

install(TARGETS MODIS-Decoder DESTINATION bin)
