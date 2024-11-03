cmake_minimum_required(VERSION 3.12)
project(trident-pm)

set(TRDLIB "libtrdpm")
set(TRDPM "trpm")
set(TRDBLD "trdbld")


#CMAKE requirements
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O2 -Wall -Wextra -Wno-unused-variable -Wno-unused-parameter")


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/lib)

file(GLOB_RECURSE LIB_TRD_SRC src/libtrident/*.c)
file(GLOB_RECURSE APP_TRD_SRC src/app/*.c)
file(GLOB_RECURSE TRD_IMG_BUILDER src/imgb/*.c)

add_library(${TRDLIB} SHARED ${LIB_TRD_SRC})
target_include_directories(${TRDLIB} PUBLIC src/libtrident/include)

if (NOT TRD_BUILD_LIBONLY)
    add_executable(${TRDPM} ${APP_TRD_SRC})
    add_executable(${TRDBLD} ${TRD_IMG_BUILDER})
    target_link_libraries(${TRDPM} PRIVATE ${TRDLIB})
    target_link_libraries(${TRDBLD} PRIVATE ${TRDLIB})
endif()



option(TRD_BUILD_LIBONLY "Do not build the trpm standalone app" OFF)
