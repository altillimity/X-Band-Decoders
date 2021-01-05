cmake_minimum_required (VERSION 3.0.0)

project (CADU-TerraSynchroderand)
file(GLOB_RECURSE CADU-TerraSynchroderand_CPPS src/*.cpp)

if(APPLE)
    include_directories(/usr/local/include)
    link_directories(/usr/local/lib)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
endif()

add_executable(CADU-TerraSynchroderand ${CADU-TerraSynchroderand_CPPS})

target_include_directories(CADU-TerraSynchroderand PUBLIC src)

target_link_libraries(CADU-TerraSynchroderand PUBLIC m)
target_link_libraries(CADU-TerraSynchroderand PUBLIC correct)

install(TARGETS CADU-TerraSynchroderand DESTINATION bin)
