#
#   Flat buffers CMake integration
#   Base on BuildFlatBuffers.cmake from flatbuffers repo
#
#   Copyright Olivier Le Doeuff 2019
#

# MakeAvailable require 3.14. FetchContent 3.11
cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project("FlatBuffersCMake" VERSION 1.0.0)

set(FLATBUFFERS_REPOSITORY "https://github.com/OlivierLDff/flatbuffers" CACHE STRING "Repository of flatbuffers")
set(FLATBUFFERS_TAG "master" CACHE STRING "Git tag/branch of flatbuffers")

include(FetchContent)

# flatbuffers
FetchContent_Declare(
  flatbuffers
  GIT_REPOSITORY ${FLATBUFFERS_REPOSITORY}
  GIT_TAG        ${FLATBUFFERS_TAG}
  )

if(NOT FBS_FOLDER_PREFIX)
  set(FBS_FOLDER_PREFIX "Dependencies/flatbuffers" CACHE STRING "")
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL ${CMAKE_HOST_SYSTEM_NAME})
  set(FBS_IS_HOST_COMPILING TRUE)
endif()

if(FBS_IS_HOST_COMPILING)
  set(FLATBUFFERS_BUILD_FLATC ON CACHE BOOL "Enable the build of the flatbuffers compiler")
else()
  set(FLATBUFFERS_BUILD_FLATC OFF CACHE BOOL "Enable the build of the flatbuffers compiler")
endif()
set(FLATBUFFERS_INSTALL OFF CACHE BOOL "Enable the installation of targets.")
set(FLATBUFFERS_BUILD_TESTS OFF CACHE BOOL "Enable the installation of targets.")
set(FLATBUFFERS_BUILD_FLATHASH OFF CACHE BOOL "Enable the build of flathash")

FetchContent_MakeAvailable(flatbuffers)

set_target_properties(flatbuffers
  PROPERTIES FOLDER ${FBS_FOLDER_PREFIX})

if(FLATBUFFERS_BUILD_FLATC)
  set_target_properties(flatc
    PROPERTIES FOLDER ${FBS_FOLDER_PREFIX})
endif()

if(FLATBUFFERS_BUILD_FLATHASH)
  set_target_properties(flathash
    PROPERTIES FOLDER ${FBS_FOLDER_PREFIX})
endif()

if(FLATBUFFERS_BUILD_TESTS)
  set_target_properties(flatsamplebfbs flatsamplebinary flatsampletext generated_code
    PROPERTIES FOLDER ${FBS_FOLDER_PREFIX}/Examples)
  set_target_properties(flattests
    PROPERTIES FOLDER ${FBS_FOLDER_PREFIX}/Tests)
endif()

# Create flat2h
if(FBS_IS_HOST_COMPILING AND FLATBUFFERS_BUILD_FLATC)
  add_executable(flat2h ${CMAKE_CURRENT_SOURCE_DIR}/src/flat2h.cpp)
  if(FBS_FOLDER_PREFIX AND NOT ${FBS_FOLDER_PREFIX} STREQUAL "")
    set_target_properties(flat2h
      PROPERTIES FOLDER ${FBS_FOLDER_PREFIX})
  endif()
  if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    target_compile_definitions(flat2h PRIVATE -D_CRT_SECURE_NO_WARNINGS)
  endif()
endif()

add_library(flatbuffers_dlc INTERFACE)
target_compile_features(flatbuffers_dlc INTERFACE cxx_std_11)
target_sources(flatbuffers_dlc INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/flatbuffers/json_parser.h)
target_include_directories(flatbuffers_dlc INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(flatbuffers_dlc INTERFACE flatbuffers)

# Include the function
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/AddFbsTarget.cmake)
