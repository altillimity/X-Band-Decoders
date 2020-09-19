cmake_minimum_required (VERSION 3.0.0)

project (CADU-Synchroderand)
file(GLOB_RECURSE CADU-Synchroderand_CPPS src/*.cpp)

add_executable(CADU-Synchroderand ${CADU-Synchroderand_CPPS})

target_include_directories(CADU-Synchroderand PUBLIC src)

target_link_libraries(CADU-Synchroderand PUBLIC m)

install(TARGETS CADU-Synchroderand DESTINATION bin)
