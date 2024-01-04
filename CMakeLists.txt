cmake_minimum_required(VERSION 3.12)

if(POLICY CMP0063)
  cmake_policy(SET CMP0063 NEW)
endif()

if(POLICY CMP0092)
  cmake_policy(SET CMP0092 NEW)
endif()

project(cmark-gfm)

set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 29)
set(PROJECT_VERSION_PATCH 0)
set(PROJECT_VERSION_GFM 13)
set(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.gfm.${PROJECT_VERSION_GFM})

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)

include(CheckFileOffsetBits)
include(FindAsan)
include(GenerateExportHeader)
include(GNUInstallDirs)

if(NOT MSVC OR CMAKE_HOST_SYSTEM_NAME STREQUAL Windows)
  set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS ON)
  include(InstallRequiredSystemLibraries)
endif()

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "Do not build in-source.\nPlease remove CMakeCache.txt and the CMakeFiles/ directory.\nThen: mkdir build ; cd build ; cmake .. ; make")
endif()

option(CMARK_TESTS "Build cmark-gfm tests and enable testing" ON)
option(CMARK_STATIC "Build static libcmark-gfm library" ON)
option(CMARK_SHARED "Build shared libcmark-gfm library" ON)
option(CMARK_LIB_FUZZER "Build libFuzzer fuzzing harness" OFF)
option(CMARK_THREADING "Add locks around static accesses" OFF)

# set a required C standard so we can load stdbool.h
if(MSVC)
  set(CMAKE_C_STANDARD 11)
else()
  set(CMAKE_C_STANDARD 99)
endif()
set(CMAKE_C_STANDARD_REQUIRED YES)

# Use CMake's generated headers instead of the Swift package prebuilt ones
add_compile_definitions(CMARK_USE_CMAKE_HEADERS)

option(CMARK_FUZZ_QUADRATIC "Build quadratic fuzzing harness" OFF)

if(CMARK_FUZZ_QUADRATIC)
  set(FUZZER_FLAGS "-fsanitize=fuzzer-no-link,address -g")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FUZZER_FLAGS}")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FUZZER_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${FUZZER_FLAGS}")
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${FUZZER_FLAGS}")
endif()

add_subdirectory(src)
add_subdirectory(extensions)
if(CMARK_TESTS AND (CMARK_SHARED OR CMARK_STATIC))
  add_subdirectory(api_test)
endif()
# TODO(compnerd) should this be enabled for MinGW, which sets CMAKE_SYSTEM_NAME
# to Windows, but defines `MINGW`.
if(NOT CMAKE_SYSTEM_NAME STREQUAL Windows)
  add_subdirectory(man)
endif()
if(CMARK_TESTS)
  enable_testing()
  add_subdirectory(test testdir)
endif()
if(CMARK_FUZZ_QUADRATIC)
  add_subdirectory(fuzz)
endif()

set(CMARK_TARGETS)
if(CMARK_SHARED)
  list(APPEND CMARK_TARGETS libcmark-gfm)
  list(APPEND CMARK_TARGETS libcmark-gfm-extensions)
endif()
if(CMARK_STATIC)
  list(APPEND CMARK_TARGETS libcmark-gfm_static)
  list(APPEND CMARK_TARGETS libcmark-gfm-extensions_static)
endif()
export(TARGETS ${CMARK_TARGETS} FILE cmark-gfmConfig.cmake)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING
  "Choose the type of build, options are: Debug Profile Release Asan Ubsan." FORCE)
endif(NOT CMAKE_BUILD_TYPE)
