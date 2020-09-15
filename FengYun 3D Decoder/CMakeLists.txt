cmake_minimum_required (VERSION 3.0.0)

project (FY3DDecoder)

file(GLOB_RECURSE FY3DDecoder_CPPS src/*.cpp src/viterbi_lib/metrics.c src/viterbi_lib/tab.c src/viterbi_lib/viterbi.c)

add_executable(FY3DDecoder ${FY3DDecoder_CPPS})

target_include_directories(FY3DDecoder PUBLIC src)

target_link_libraries(FY3DDecoder PUBLIC m)

find_package(Threads REQUIRED)
target_link_libraries(FY3DDecoder PUBLIC Threads::Threads)

target_link_libraries (FY3DDecoder PUBLIC "correct")