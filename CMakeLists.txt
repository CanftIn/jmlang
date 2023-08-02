cmake_minimum_required(VERSION 3.20)
project(
  jmlang
  VERSION 0.0.1
  LANGUAGES C CXX
)

set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE "Debug")
set(project_version "${${PROJECT_NAME}_VERSION}")

message(STATUS "Project '${PROJECT_NAME}', version: '${project_version}'")

# Require standard C++17
set(CMAKE_CXX_STANDARD 17 CACHE STRING "The C++ standard to use. Require 17 or higher.")
option(CMAKE_CXX_STANDARD_REQUIRED "When enabled, the value of CMAKE_CXX_STANDARD is a requirement." ON)
option(CMAKE_CXX_EXTENSIONS "When enabled, compiler-specific language extensions are enabled (e.g. -std=gnu++17)" OFF)

if(CMAKE_CXX_STANDARD LESS 17)
  message(FATAL_ERROR "jmlang requires C++17 or newer but CMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}")
endif()

option(JMLANG_OPT_BUILD_TESTS "Build all jmlang tests" ON)
option(JMLANG_OPT_BUILD_BENCHMARKS "Build all jmlang benchmarks" ON)
option(JMLANG_OPT_BUILD_SHARED_LIBS "Build jmlang shared libraries" ON)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  # for debug of stl structure while using clang compile
  add_compile_options(-fstandalone-debug)
endif()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Import useful standard modules
include(CMakeDependentOption)
include(CheckCXXSymbolExists)

# Make our custom helpers.
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
include(LLVM)

include(CTest)
enable_testing()

# Build jmlang with ccache if the package is present
option(JMLANG_CCACHE_BUILD "Set to ON for a ccache enabled build" OFF)
mark_as_advanced(JMLANG_CCACHE_BUILD)

if (JMLANG_CCACHE_BUILD)
  find_program(CCACHE_PROGRAM ccache REQUIRED)

  set(JMLANG_CCACHE_PARAMS
      CCACHE_CPP2=yes
      CCACHE_HASHDIR=yes
      CCACHE_SLOPPINESS=pch_defines,time_macros,include_file_mtime,include_file_ctime
      CACHE STRING "Parameters to pass through to ccache")
  mark_as_advanced(JMLANG_CCACHE_PARAMS)

  set(CMAKE_C_COMPILER_LAUNCHER ${CMAKE_COMMAND} -E env ${JMLANG_CCACHE_PARAMS} ${CCACHE_PROGRAM})
  set(CMAKE_CXX_COMPILER_LAUNCHER ${CMAKE_COMMAND} -E env ${JMLANG_CCACHE_PARAMS} ${CCACHE_PROGRAM})

  # Per https://ccache.dev/manual/latest.html#_precompiled_headers,
  # we must set -fno-pch-timestamp when using Clang + CCache + PCH
  if(CMAKE_C_COMPILER_ID MATCHES "Clang")
    string(APPEND CMAKE_C_FLAGS " -Xclang -fno-pch-timestamp")
  endif()
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    string(APPEND CMAKE_CXX_FLAGS " -Xclang -fno-pch-timestamp")
  endif()

  message(STATUS "Enabling ccache usage for building.")
endif ()

include_directories(include)

set(JMLANG_SRCS)

set(STATIC_LIB_NAME ${PROJECT_NAME})

file(GLOB_RECURSE LIB_PATH
    ./lib/*/*.cc
  )
list(APPEND JMLANG_SRCS ${LIB_PATH})

add_library(${STATIC_LIB_NAME} STATIC ${JMLANG_SRCS})
target_link_libraries(${STATIC_LIB_NAME} JMLANG::LLVM)
target_compile_features(${STATIC_LIB_NAME} PUBLIC cxx_std_17)

if (JMLANG_OPT_BUILD_TESTS)
  add_subdirectory(test #[[EXCLUDE_FROM_ALL]])
endif()

# if (JMLANG_OPT_BUILD_BENCHMARKS)
#   add_subdirectory(benchmarks #[[EXCLUDE_FROM_ALL]])
# endif()
