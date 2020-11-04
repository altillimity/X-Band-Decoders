cmake_minimum_required (VERSION 3.0.0)

project (HRD-Decoder)
file(GLOB_RECURSE HRD-Decoder_CPPS src/*.cpp src/viterbi_lib/metrics.c src/viterbi_lib/tab.c src/viterbi_lib/viterbi.c)

add_executable(HRD-Decoder ${HRD-Decoder_CPPS})

target_include_directories(HRD-Decoder PUBLIC src)

target_link_libraries(HRD-Decoder PUBLIC m)

find_package(Threads REQUIRED)
target_link_libraries(HRD-Decoder PUBLIC Threads::Threads)

target_link_libraries (HRD-Decoder PUBLIC "correct")

install(TARGETS HRD-Decoder DESTINATION bin)