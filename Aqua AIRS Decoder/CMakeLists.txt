cmake_minimum_required (VERSION 3.0.0)

project (Aqua-AIRS-Decoder)
file(GLOB_RECURSE Aqua-AIRS-Decoder_CPPS src/*.cpp)

add_executable(Aqua-AIRS-Decoder ${Aqua-AIRS-Decoder_CPPS})

target_include_directories(Aqua-AIRS-Decoder PUBLIC src)

target_link_libraries(Aqua-AIRS-Decoder PUBLIC m)

find_package(Threads REQUIRED)
target_link_libraries(Aqua-AIRS-Decoder PUBLIC Threads::Threads)

if(WIN32 AND NOT MINGW)
    find_package(PNG CONFIG REQUIRED)
else()
    find_package(PNG REQUIRED)
endif()
target_link_libraries(Aqua-AIRS-Decoder PUBLIC PNG::PNG)

find_library(CCSDS_LIBRARY ccsds REQUIRED)
target_link_libraries(Aqua-AIRS-Decoder PUBLIC ${CCSDS_LIBRARY})

install(TARGETS Aqua-AIRS-Decoder DESTINATION bin)
