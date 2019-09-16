#
#   Flat buffers CMake integration
#   Base on BuildFlatBuffers.cmake from flatbuffers repo
#
#   Copyright Olivier Le Doeuff 2019
#

cmake_minimum_required( VERSION 3.11 FATAL_ERROR )

set( FLATBUFFERS_REPOSITORY "https://github.com/google/flatbuffers" CACHE STRING "Repository of flatbuffers" )
set( FLATBUFFERS_TAG "master" CACHE STRING "Git tag/branch of flatbuffers" )

# flatbuffers
FetchContent_Declare(
    flatbuffers
    GIT_REPOSITORY ${FLATBUFFERS_REPOSITORY}
    GIT_TAG        ${FLATBUFFERS_TAG}
)

macro(add_fbs_target TARGET SOURCES)

    FetchContent_MakeAvailable(flatbuffers)
    set_target_properties(flatbuffers flatc
        PROPERTIES FOLDER Dependencies/flatbuffers)
    set_target_properties(flathash flatsamplebfbs flatsamplebinary flatsampletext
        PROPERTIES FOLDER Dependencies/flatbuffers/Examples)
    set_target_properties(flattests
        PROPERTIES FOLDER Dependencies/flatbuffers/Tests)

    set(FBS_OPTIONS VERBOSE
        )
    set(FBS_ONE_VALUE_ARG
        GENERATED_INCLUDE_DIR
        BINARY_SCHEMA_DIR
        COPY_TEXT_SCHEMA_DIR
        )
    set(FBS_MULTI_VALUE_ARG
        DEPENDENCIES
        FLATC_ARGUMENTS
        INCLUDE_DIR
        )
     # parse the macro arguments
    cmake_parse_arguments(ARGFBS "${FBS_OPTIONS}" "${FBS_ONE_VALUE_ARG}" "${FBS_MULTI_VALUE_ARG}" ${ARGN})

    set(FBS_TARGET ${TARGET})
    set(FBS_SRCS ${SOURCES})
    set(FBS_VERBOSE ${ARGFBS_VERBOSE})
    set(FBS_GENERATED_INCLUDE_DIR ${ARGFBS_GENERATED_INCLUDE_DIR})
    set(FBS_BINARY_SCHEMA_DIR ${ARGFBS_BINARY_SCHEMA_DIR})
    set(FBS_COPY_TEXT_SCHEMA_DIR ${ARGFBS_COPY_TEXT_SCHEMA_DIR})
    set(FBS_DEPENDENCIES ${ARGFBS_DEPENDENCIES})
    set(FBS_FLATC_ARGUMENTS ${ARGFBS_FLATC_ARGUMENTS})
    set(FBS_INCLUDE_DIR ${ARGFBS_INCLUDE_DIR})

    # Print verbose parameters for easy debugging
    if(FBS_VERBOSE)

        message(STATUS "Add Flat Buffers generated target : ${TARGET}")
        message(STATUS "FBS_TARGET                : ${FBS_TARGET}")
        message(STATUS "FBS_SRCS                  : ${FBS_SRCS}")
        message(STATUS "FBS_VERBOSE               : ${FBS_VERBOSE}")
        message(STATUS "FBS_GENERATED_INCLUDE_DIR : ${FBS_GENERATED_INCLUDE_DIR}")
        message(STATUS "FBS_BINARY_SCHEMA_DIR     : ${FBS_BINARY_SCHEMA_DIR}")
        message(STATUS "FBS_COPY_TEXT_SCHEMA_DIR  : ${FBS_COPY_TEXT_SCHEMA_DIR}")
        message(STATUS "FBS_DEPENDENCIES          : ${FBS_DEPENDENCIES}")
        message(STATUS "FBS_FLATC_ARGUMENTS       : ${FBS_FLATC_ARGUMENTS}")
        message(STATUS "FBS_INCLUDE_DIR           : ${FBS_INCLUDE_DIR}")

    endif(FBS_VERBOSE)

    # ALl the generated created by this macro
    set(ALL_GENERATED_FILES "")
    set(INCLUDE_PARAMS "")

    # Generate the include file param
    # The form is -I path/to/dir1 -I path/to/dir2 etc...
    foreach (INCLUDE_DIR ${FBS_INCLUDE_DIR})

        set(INCLUDE_PARAMS -I ${INCLUDE_DIR} ${INCLUDE_PARAMS})

    endforeach()

    foreach(SRC ${FBS_SRCS})

        # Isolate filename to create the generated filename
        get_filename_component(FILENAME ${SRC} NAME_WE)

        # We check that we have an output directory before generating a rule
        if (NOT ${FBS_GENERATED_INCLUDE_DIR} STREQUAL "")

            # Name of the output generated file
            set(GENERATED_INCLUDE ${FBS_GENERATED_INCLUDE_DIR}/${FILENAME}_generated.h)

            # Add the rule for each files
            message(STATUS "Add rule to build ${FILENAME}_generated.h from ${SRC}")
            add_custom_command(
                OUTPUT ${GENERATED_INCLUDE} ${FBS_FLATC_ARGUMENTS}
                COMMAND ${CMAKE_COMMAND} -E echo "Generate ${FILENAME}_generated.h in ${FBS_GENERATED_INCLUDE_DIR}"
                COMMAND flatc ${FLATC_SCHEMA_ARGS}
                -o ${FBS_GENERATED_INCLUDE_DIR}
                ${INCLUDE_PARAMS}
                -c ${SRC}
                DEPENDS flatc ${SRC} ${FBS_DEPENDENCIES}
                COMMENT "Generate ${FILENAME}_generated.h in ${FBS_GENERATED_INCLUDE_DIR} from ${SRC}"
                WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
            list(APPEND ALL_GENERATED_FILES ${GENERATED_INCLUDE})

        endif() # (NOT ${FBS_GENERATED_INCLUDE_DIR} STREQUAL "")

        # Should we also build bfbs
        if (NOT ${FBS_BINARY_SCHEMA_DIR} STREQUAL "")

            # Name of the output binary buffer file
            set(BINARY_SCHEMA ${FBS_BINARY_SCHEMA_DIR}/${FILENAME}.bfbs)
            add_custom_command(
                OUTPUT ${BINARY_SCHEMA}
                COMMAND flatc -b --schema
                -o ${FBS_BINARY_SCHEMA_DIR}
                ${INCLUDE_PARAMS}
                ${SRC}
                DEPENDS ${FLATC_TARGET} ${SRC} ${FBS_DEPENDENCIES}
                COMMENT "Generate ${FILENAME}.bfbs in ${FBS_GENERATED_INCLUDE_DIR} from ${SRC}"
                WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
            list(APPEND ALL_GENERATED_FILES ${BINARY_SCHEMA})

        endif() # (NOT ${FBS_BINARY_SCHEMA_DIR} STREQUAL "")

        if (NOT ${FBS_COPY_TEXT_SCHEMA_DIR} STREQUAL "")

            # Name of the output binary buffer file
            set(COPY_SCHEMA ${FBS_COPY_TEXT_SCHEMA_DIR}/${FILENAME}.fbs)
            add_custom_command(
                OUTPUT ${COPY_SCHEMA}
                COMMAND ${CMAKE_COMMAND} -E echo "Copy ${FILENAME}.fbs in ${FBS_COPY_TEXT_SCHEMA_DIR}"
                COMMAND ${CMAKE_COMMAND} -E copy ${SRC} ${COPY_SCHEMA}
                DEPENDS ${FLATC_TARGET} ${SRC} ${FBS_DEPENDENCIES}
                COMMENT "Copy file ${SRC} to ${COPY_SCHEMA}"
                WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
            list(APPEND ALL_GENERATED_FILES ${COPY_SCHEMA})
        endif() # (NOT ${FBS_COPY_TEXT_SCHEMA_DIR} STREQUAL "")

    endforeach() # SRC ${FBS_SRCS}

    if(FBS_VERBOSE)
        message(STATUS "${FBS_TARGET} generated files : ${ALL_GENERATED_FILES}")
    endif(FBS_VERBOSE)

    add_custom_target(${FBS_TARGET}
                    DEPENDS ${ALL_GENERATED_FILES} flatc ${FBS_DEPENDENCIES}
                    SOURCES ${FBS_SRCS})

endmacro()