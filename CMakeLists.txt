cmake_minimum_required(VERSION 3.12)
project(trident-pm)

find_package(LibXml2 REQUIRED)
find_package(ZLIB REQUIRED)

set(TRDLIB "libtrdpm")
set(TRDPM "trpm")
set(TRDBLD "trdbld")


#CMAKE requirements
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O2 -Wall -Wextra -Wno-unused-variable -Wno-unused-parameter -Wno-unused-value")


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/lib)

file(GLOB_RECURSE LIB_TRD_SRC src/libtrident/*.c)
file(GLOB_RECURSE APP_TRD_SRC src/app/*.c)
file(GLOB_RECURSE TRD_IMG_BUILDER src/imgb/*.c)
file(GLOB_RECURSE LIBS_EXTERNAL src/libs/*.c)

add_library(${TRDLIB} SHARED ${LIB_TRD_SRC})
target_include_directories(${TRDLIB} PUBLIC src/libtrident/include)
target_include_directories(${TRDLIB} PUBLIC src/libs)
if (NOT TRD_BUILD_LIBONLY)
    add_executable(${TRDPM} ${APP_TRD_SRC} ${LIBS_EXTERNAL})
    add_executable(${TRDBLD} ${TRD_IMG_BUILDER} ${LIBS_EXTERNAL})
    target_link_libraries(${TRDPM} PRIVATE ${TRDLIB} )
    target_include_directories(${TRDBLD} PRIVATE ${LIBXML2_INCLUDE_DIRS})
    target_link_libraries(${TRDBLD} PRIVATE ${TRDLIB} ${LIBXML2_LIBRARIES} ZLIB::ZLIB)
   
endif()



option(TRD_BUILD_LIBONLY "Do not build the trpm standalone app" OFF)
