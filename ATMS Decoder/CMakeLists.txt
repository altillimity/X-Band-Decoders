cmake_minimum_required (VERSION 3.0.0)

project (ATMS-Decoder)
file(GLOB_RECURSE ATMS-Decoder_CPPS src/*.cpp)

add_executable(ATMS-Decoder ${ATMS-Decoder_CPPS})

target_include_directories(ATMS-Decoder PUBLIC src)

target_link_libraries(ATMS-Decoder PUBLIC m)

find_library(CCSDS_LIBRARY ccsds REQUIRED)
target_link_libraries(ATMS-Decoder PUBLIC ${CCSDS_LIBRARY})

find_package(Threads REQUIRED)
target_link_libraries(ATMS-Decoder PUBLIC Threads::Threads)

if(WIN32 AND NOT MINGW)
    find_package(PNG CONFIG REQUIRED)
else()
    find_package(PNG REQUIRED)
endif()
target_link_libraries(ATMS-Decoder PUBLIC PNG::PNG)

install(TARGETS ATMS-Decoder DESTINATION bin)
