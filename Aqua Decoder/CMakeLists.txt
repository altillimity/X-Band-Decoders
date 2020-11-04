cmake_minimum_required (VERSION 3.0.0)

project (AQUA-Decoder)
file(GLOB_RECURSE AQUA-Decoder_CPPS src/*.cpp)

if(APPLE)
    include_directories(/usr/local/include)
    link_directories(/usr/local/lib)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

add_executable(AQUA-Decoder ${AQUA-Decoder_CPPS})

target_include_directories(AQUA-Decoder PUBLIC src)

target_link_libraries(AQUA-Decoder PUBLIC m)

target_link_libraries (AQUA-Decoder PUBLIC "correct")

install(TARGETS AQUA-Decoder DESTINATION bin)