cmake_minimum_required(VERSION 3.12)
project(trident-pm)

find_package(LibXml2 REQUIRED)
find_package(ZLIB REQUIRED)

set(TRDLIB "libtrdpm")
set(TRDPM "trpm")
set(TRDBLD "trdbld")


#CMAKE requirements
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(COMMON_FLGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always -O2 -Wall -Wextra -Wunreachable-code -Wfloat-equal")

set(CMAKE_CXX_FLAGS_DEBUG "${COMMON_FLGS} -g")
#undefine macro that exports dbgprintf()
set(CMAKE_CXX_FLAGS_RELEASE "${COMMON_FLGS} -D__DEBUG_PROJ_TARGET")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/lib)

file(GLOB_RECURSE LIB_TRD_SRC src/libtrident/*.cpp)
file(GLOB_RECURSE APP_TRD_SRC src/app/*.cpp)
file(GLOB_RECURSE TRD_IMG_BUILDER src/imgb/*.cpp)
file(GLOB_RECURSE LIBS_EXTERNAL src/libs/*.cpp)



add_library(${TRDLIB} SHARED ${LIB_TRD_SRC})
target_include_directories(${TRDLIB} PUBLIC
    src/libtrident/include
    src/libtrident/err
    src/libtrident/pkgio
    src/libtrident
    src/libtrident/header
    src/libtrident/utils
    src/libtrident/utils/tuid
    src/libtrident/sdesc
)



target_include_directories(${TRDLIB} PRIVATE ${LIBXML2_INCLUDE_DIR})
target_link_libraries(${TRDLIB} PRIVATE ${LIBXML2_LIBRARIES} ZLIB::ZLIB)
if (NOT TRD_BUILD_LIBONLY)
    add_executable(${TRDPM} ${APP_TRD_SRC} ${LIBS_EXTERNAL})
    add_executable(${TRDBLD} ${TRD_IMG_BUILDER} ${LIBS_EXTERNAL})
   # target_link_libraries(${TRDPM} PRIVATE ${TRDLIB} ZLIB::ZLIB)
    target_include_directories(${TRDBLD} PRIVATE ${LIBXML2_INCLUDE_DIR})
    target_link_libraries(${TRDBLD} PRIVATE ${TRDLIB} ${LIBXML2_LIBRARIES} ZLIB::ZLIB)
  #  target_link_libraries(${TRDBLD} PRIVATE ${TRDLIB} ZLIB::ZLIB)
   
endif()



option(TRD_BUILD_LIBONLY "Do not build the trpm standalone app" OFF)
