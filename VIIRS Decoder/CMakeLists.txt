cmake_minimum_required (VERSION 3.0.0)

project (VIIRS-Decoder)
file(GLOB_RECURSE VIIRS-Decoder_CPPS src/*.cpp)

add_executable(VIIRS-Decoder ${VIIRS-Decoder_CPPS})

target_include_directories(VIIRS-Decoder PUBLIC src)

target_link_libraries(VIIRS-Decoder PUBLIC m)

find_library(AEC_LIBRARY aec HINTS /usr/lib REQUIRED)
target_link_libraries(VIIRS-Decoder PUBLIC ${AEC_LIBRARY})

find_library(CCSDS_LIBRARY ccsds REQUIRED)
target_link_libraries(VIIRS-Decoder PUBLIC ${CCSDS_LIBRARY})

find_package(Threads REQUIRED)
target_link_libraries(VIIRS-Decoder PUBLIC Threads::Threads)

if(WIN32 AND NOT MINGW)
    find_package(PNG CONFIG REQUIRED)
else()
    find_package(PNG REQUIRED)
endif()
target_link_libraries(VIIRS-Decoder PUBLIC PNG::PNG)

install(TARGETS VIIRS-Decoder DESTINATION bin)
