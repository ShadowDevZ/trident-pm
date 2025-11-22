cmake_minimum_required(VERSION 3.12)
project(trident-pm)

###find_package(LibXml2 REQUIRED)
find_package(ZLIB REQUIRED)

set(TRDLIB "libtrdpm")
set(TRDPM "trpm")
set(TRDBLD "trdbld")
#for clang-tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# CMAKE requirements
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(COMMON_FLGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always -Wall -Wextra -Wunreachable-code -Wfloat-equal")

##### COMMENT TO DISABLE SANITIZERS FOR DEBUG BUILD
set(ASANON_FLAGS "-fsanitize=address")
set(UBSANON_FLAGS "-fsanitize=undefined")
set(LSANON_FLAGS "-fsanitize=leak")

# Concatenate strings with spaces
set(SANITIZER_FLAGS "${ASANON_FLAGS} ${UBSANON_FLAGS} ${LSANON_FLAGS}")

#####

set(CMAKE_CXX_FLAGS_DEBUG "${COMMON_FLGS} -D__DEBUG_PROJ_TARGET -g ${SANITIZER_FLAGS} -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${COMMON_FLGS} -O2 -D_FORTIFY_SOURCE=2")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/lib)

file(GLOB_RECURSE LIB_TRD_SRC src/libtrident/*.cpp)
file(GLOB_RECURSE APP_TRD_SRC src/app/*.cpp)
file(GLOB_RECURSE TRD_IMG_BUILDER src/imgb/*.cpp)
file(GLOB_RECURSE LIBS_EXTERNAL src/libs/*.cpp)



# --- Library target ---
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
    src/libtrident/utils/uid/suid
    src/libtrident/utils/uid
)
###target_include_directories(${TRDLIB} PRIVATE ${LIBXML2_INCLUDE_DIR})
###target_link_libraries(${TRDLIB} PRIVATE ${LIBXML2_LIBRARIES} ZLIB::ZLIB)
target_link_libraries(${TRDLIB} PRIVATE ZLIB::ZLIB)
# --- Executable targets (only if not building library-only) ---
option(TRD_BUILD_LIBONLY "Do not build the trpm standalone app" OFF)

if(NOT TRD_BUILD_LIBONLY)
    add_executable(${TRDPM} ${APP_TRD_SRC} ${LIBS_EXTERNAL})
    target_link_options(${TRDPM} PRIVATE -pie)

    add_executable(${TRDBLD} ${TRD_IMG_BUILDER} ${LIBS_EXTERNAL})
    target_include_directories(${TRDBLD} PRIVATE ${LIBXML2_INCLUDE_DIR})
    target_link_libraries(${TRDBLD} PRIVATE ${TRDLIB} ${LIBXML2_LIBRARIES} ZLIB::ZLIB)
    target_link_options(${TRDBLD} PRIVATE -pie)
endif()
