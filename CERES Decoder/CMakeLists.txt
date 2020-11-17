cmake_minimum_required (VERSION 3.0.0)

project (CERES-Decoder)
file(GLOB_RECURSE CERES-Decoder_CPPS src/*.cpp)

add_executable(CERES-Decoder ${CERES-Decoder_CPPS})

target_include_directories(CERES-Decoder PUBLIC src)

target_link_libraries(CERES-Decoder PUBLIC m)

find_library(CCSDS_LIBRARY ccsds REQUIRED)
target_link_libraries(CERES-Decoder PUBLIC ${CCSDS_LIBRARY})

find_package(Threads REQUIRED)
target_link_libraries(CERES-Decoder PUBLIC Threads::Threads)

if(WIN32 AND NOT MINGW)
    find_package(PNG CONFIG REQUIRED)
else()
    find_package(PNG REQUIRED)
endif()
target_link_libraries(CERES-Decoder PUBLIC PNG::PNG)

install(TARGETS CERES-Decoder DESTINATION bin)
