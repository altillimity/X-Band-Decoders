cmake_minimum_required (VERSION 3.0.0)

project (AQUADecoder)
file(GLOB_RECURSE AQUADecoder_CPPS src/*.cpp)

if(APPLE)
    include_directories(/usr/local/include)
    link_directories(/usr/local/lib)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

add_executable(AQUADecoder ${AQUADecoder_CPPS})

target_include_directories(AQUADecoder PUBLIC src)

target_link_libraries(AQUADecoder PUBLIC m)

target_link_libraries (AQUADecoder PUBLIC "correct")
