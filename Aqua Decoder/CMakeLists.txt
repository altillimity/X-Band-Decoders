cmake_minimum_required (VERSION 3.0.0)

project (AQUADecoder)
file(GLOB_RECURSE AQUADecoder_CPPS src/*.cpp)

add_executable(AQUADecoder ${AQUADecoder_CPPS})

target_include_directories(AQUADecoder PUBLIC src)

target_link_libraries(AQUADecoder PUBLIC m)

target_link_libraries (AQUADecoder PUBLIC "correct")