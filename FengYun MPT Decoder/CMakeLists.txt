cmake_minimum_required (VERSION 3.0.0)

project (FengYun-MPT-Decoder)

file(GLOB_RECURSE FengYun-MPT-Decoder_CPPS src/*.cpp src/viterbi_lib/metrics.c src/viterbi_lib/tab.c src/viterbi_lib/viterbi.c)

if(APPLE)
    include_directories(/usr/local/include)
    link_directories(/usr/local/lib)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
endif()

add_executable(FengYun-MPT-Decoder ${FengYun-MPT-Decoder_CPPS})

target_include_directories(FengYun-MPT-Decoder PUBLIC src)

target_link_libraries(FengYun-MPT-Decoder PUBLIC m)

find_package(Threads REQUIRED)
target_link_libraries(FengYun-MPT-Decoder PUBLIC Threads::Threads)

target_link_libraries(FengYun-MPT-Decoder PUBLIC "correct")

install(TARGETS FengYun-MPT-Decoder DESTINATION bin)