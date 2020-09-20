cmake_minimum_required (VERSION 3.0.0)

project (ELEKTRO-L-Decoder)
file(GLOB_RECURSE ELEKTRO-L-Decoder_CPPS src/*.cpp src/viterbi_lib/metrics.c src/viterbi_lib/tab.c src/viterbi_lib/viterbi.c)

if(APPLE)
    include_directories(/usr/local/include)
    link_directories(/usr/local/lib)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
endif()

add_executable(ELEKTRO-L-Decoder ${ELEKTRO-L-Decoder_CPPS})

target_include_directories(ELEKTRO-L-Decoder PUBLIC src)

target_link_libraries(ELEKTRO-L-Decoder PUBLIC m)

find_package(Threads REQUIRED)
target_link_libraries(ELEKTRO-L-Decoder PUBLIC Threads::Threads)

if(WIN32 AND NOT MINGW)
    find_package(PNG CONFIG REQUIRED)
else()
    find_package(PNG REQUIRED)
endif()
target_link_libraries(ELEKTRO-L-Decoder PUBLIC PNG::PNG)

install(TARGETS ELEKTRO-L-Decoder DESTINATION bin)