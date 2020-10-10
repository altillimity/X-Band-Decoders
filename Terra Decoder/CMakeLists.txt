cmake_minimum_required (VERSION 3.0.0)

project (TERRA-Decoder)
file(GLOB_RECURSE TERRA-Decoder_CPPS src/*.cpp  src/viterbi_lib/metrics.c src/viterbi_lib/tab.c src/viterbi_lib/viterbi.c)

if(APPLE)
    include_directories(/usr/local/include)
    link_directories(/usr/local/lib)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

add_executable(TERRA-Decoder ${TERRA-Decoder_CPPS})

target_include_directories(TERRA-Decoder PUBLIC src)

target_link_libraries(TERRA-Decoder PUBLIC m)

target_link_libraries (TERRA-Decoder PUBLIC "correct")
