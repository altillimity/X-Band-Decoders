cmake_minimum_required (VERSION 3.0.0)

project (Aqua-AMSU-Decoder)
file(GLOB_RECURSE Aqua-AMSU-Decoder_CPPS src/*.cpp)

add_executable(Aqua-AMSU-Decoder ${Aqua-AMSU-Decoder_CPPS})

target_include_directories(Aqua-AMSU-Decoder PUBLIC src)

target_link_libraries(Aqua-AMSU-Decoder PUBLIC m)

find_package(Threads REQUIRED)
target_link_libraries(Aqua-AMSU-Decoder PUBLIC Threads::Threads)

if(WIN32 AND NOT MINGW)
    find_package(PNG CONFIG REQUIRED)
else()
    find_package(PNG REQUIRED)
endif()
target_link_libraries(Aqua-AMSU-Decoder PUBLIC PNG::PNG)

find_library(CCSDS_LIBRARY ccsds REQUIRED)
target_link_libraries(Aqua-AMSU-Decoder PUBLIC ${CCSDS_LIBRARY})

install(TARGETS Aqua-AMSU-Decoder DESTINATION bin)
