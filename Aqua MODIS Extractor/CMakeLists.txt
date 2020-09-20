cmake_minimum_required (VERSION 3.0.0)

project (AQUA-MODIS-Extractor)
file(GLOB_RECURSE AQUA-MODIS-Extractor_CPPS src/*.cpp)

if(APPLE)
    include_directories(/usr/local/include)
    link_directories(/usr/local/lib)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
endif()

add_executable(AQUA-MODIS-Extractor ${AQUA-MODIS-Extractor_CPPS})

target_include_directories(AQUA-MODIS-Extractor PUBLIC src)

target_link_libraries(AQUA-MODIS-Extractor PUBLIC m)

install(TARGETS AQUA-MODIS-Extractor DESTINATION bin)
