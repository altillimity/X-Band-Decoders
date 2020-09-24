cmake_minimum_required (VERSION 3.0.0)

project (FengYun-MERSI-1-Decoder)
file(GLOB_RECURSE FengYun-MERSI-1-Decoder_CPPS src/*.cpp)

add_executable(FengYun-MERSI-1-Decoder ${FengYun-MERSI-1-Decoder_CPPS})

if(APPLE)
    include_directories(/usr/local/include)
    link_directories(/usr/local/lib)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
endif()

target_include_directories(FengYun-MERSI-1-Decoder PUBLIC src)

target_link_libraries(FengYun-MERSI-1-Decoder PUBLIC m)

find_package(Threads REQUIRED)
target_link_libraries(FengYun-MERSI-1-Decoder PUBLIC Threads::Threads)

if(WIN32 AND NOT MINGW)
    find_package(PNG CONFIG REQUIRED)
else()
    find_package(PNG REQUIRED)
endif()
target_link_libraries(FengYun-MERSI-1-Decoder PUBLIC PNG::PNG)

install(TARGETS FengYun-MERSI-1-Decoder DESTINATION bin)