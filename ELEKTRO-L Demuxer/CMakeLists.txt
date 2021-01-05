cmake_minimum_required (VERSION 3.0.0)

project (ELEKTRO-L-Demuxer)
file(GLOB_RECURSE ELEKTRO-L-Demuxer_CPPS src/*.cpp)

add_executable(ELEKTRO-L-Demuxer ${ELEKTRO-L-Demuxer_CPPS})

target_include_directories(ELEKTRO-L-Demuxer PUBLIC src)

target_link_libraries(ELEKTRO-L-Demuxer PUBLIC m)

install(TARGETS ELEKTRO-L-Demuxer DESTINATION bin)
