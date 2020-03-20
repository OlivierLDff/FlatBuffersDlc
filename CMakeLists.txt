#
#   Flat buffers CMake integration
#   Base on BuildFlatBuffers.cmake from flatbuffers repo
#
#   Copyright Olivier Le Doeuff 2019
#

# MakeAvailable require 3.14. FetchContent 3.11
cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project("FlatBuffersCMake" VERSION 1.0.0)

set(FLATBUFFERS_REPOSITORY "https://github.com/google/flatbuffers" CACHE STRING "Repository of flatbuffers")
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

FetchContent_MakeAvailable(flatbuffers)
set_target_properties(flatbuffers flatc
    PROPERTIES FOLDER ${FBS_FOLDER_PREFIX})
set_target_properties(flathash flatsamplebfbs flatsamplebinary flatsampletext generated_code
    PROPERTIES FOLDER ${FBS_FOLDER_PREFIX}/Examples)
set_target_properties(flattests generated_code
    PROPERTIES FOLDER ${FBS_FOLDER_PREFIX}/Tests)

if(${CMAKE_SYSTEM_NAME} STREQUAL ${CMAKE_HOST_SYSTEM_NAME})
    # Create flat2h
    add_executable(flat2h ${CMAKE_CURRENT_SOURCE_DIR}/src/flat2h.cpp)
    if(FBS_FOLDER_PREFIX AND NOT ${FBS_FOLDER_PREFIX} STREQUAL "")
        set_target_properties(flat2h
            PROPERTIES FOLDER ${FBS_FOLDER_PREFIX})
    endif()
    if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
        target_compile_definitions(flat2h PRIVATE -D_CRT_SECURE_NO_WARNINGS)
    endif()
endif()

set(FLATBUFFERS_CMAKE_ROOT ${CMAKE_CURRENT_SOURCE_DIR} CACHE STRING "Root of FlatBuffersCMake that will later be referenced into AddFbsTarget.cmake")

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/AddFbsTarget.cmake)
