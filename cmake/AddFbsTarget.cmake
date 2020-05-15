include(CMakeParseArguments)

function(add_fbs_target TARGET SOURCES)

  set(FBS_OPTIONS VERBOSE
    RC_SCHEMAS
    REFLECT_NAMES
    GEN_OBJECT_API
    GEN_NAME_STRINGS
    GEN_MUTABLE
    SCOPED_ENUMS
    )
  set(FBS_ONE_VALUE_ARG
    GENERATED_INCLUDE_DIR
    BINARY_SCHEMA_DIR
    FILENAME_EXT
    FILENAME_SUFFIX
    FILENAME_RC_SUFFIX
    COPY_TEXT_SCHEMA_DIR
    )
  set(FBS_MULTI_VALUE_ARG
    DEPENDENCIES
    FLATC_ARGUMENTS
    INCLUDE_DIR
    )
  # parse the function arguments
  cmake_parse_arguments(ARGFBS "${FBS_OPTIONS}" "${FBS_ONE_VALUE_ARG}" "${FBS_MULTI_VALUE_ARG}" ${ARGN})

  set(FBS_TARGET ${TARGET})
  set(FBS_SRCS ${SOURCES})
  set(FBS_VERBOSE ${ARGFBS_VERBOSE})
  set(FBS_RC_SCHEMAS ${ARGFBS_RC_SCHEMAS})
  set(FBS_GENERATED_INCLUDE_DIR ${ARGFBS_GENERATED_INCLUDE_DIR})
  set(FBS_BINARY_SCHEMA_DIR ${ARGFBS_BINARY_SCHEMA_DIR})
  set(FBS_COPY_TEXT_SCHEMA_DIR ${ARGFBS_COPY_TEXT_SCHEMA_DIR})
  set(FBS_DEPENDENCIES ${ARGFBS_DEPENDENCIES})
  set(FBS_FLATC_ARGUMENTS ${ARGFBS_FLATC_ARGUMENTS})
  set(FBS_INCLUDE_DIR ${ARGFBS_INCLUDE_DIR})

    if(ARGFBS_REFLECT_NAMES)
      set(FBS_REFLECT_NAMES --reflect-names)
    endif()
    if(ARGFBS_GEN_OBJECT_API)
      set(FBS_GEN_OBJECT_API --gen-object-api)
    endif()
    if(ARGFBS_GEN_NAME_STRINGS)
      set(FBS_GEN_NAME_STRINGS --gen-name-strings)
    endif()
    if(ARGFBS_SCOPED_ENUMS)
      set(FBS_SCOPED_ENUMS --scoped-enums)
    endif()
    if(ARGFBS_GEN_MUTABLE)
      set(FBS_GEN_MUTABLE --gen-mutable)
    endif()

    if(ARGFBS_FILENAME_EXT)
      set(FBS_FILENAME_EXT --filename-ext ${ARGFBS_FILENAME_EXT})
    else()
      set(ARGFBS_FILENAME_EXT h)
    endif()
    if(ARGFBS_FILENAME_SUFFIX)
      set(FBS_FILENAME_SUFFIX --filename-suffix ${ARGFBS_FILENAME_SUFFIX})
    else()
      set(ARGFBS_FILENAME_SUFFIX _generated)
    endif()
    if(ARGFBS_FILENAME_RC_SUFFIX)
      set(FBS_FILENAME_RC_SUFFIX --filename-rc-suffix ${ARGFBS_FILENAME_RC_SUFFIX})
    else()
      set(ARGFBS_FILENAME_RC_SUFFIX _rc)
    endif()

    # Print verbose parameters for easy debugging
    if(FBS_VERBOSE)

      message(STATUS "Add Flat Buffers generated target : ${TARGET}")
      message(STATUS "FBS_TARGET                : ${FBS_TARGET}")
      message(STATUS "FBS_SRCS                  : ${FBS_SRCS}")
      message(STATUS "FBS_VERBOSE               : ${FBS_VERBOSE}")
      message(STATUS "FBS_RC_SCHEMAS            : ${FBS_RC_SCHEMAS}")
      message(STATUS "FBS_GENERATED_INCLUDE_DIR : ${FBS_GENERATED_INCLUDE_DIR}")
      message(STATUS "FBS_BINARY_SCHEMA_DIR     : ${FBS_BINARY_SCHEMA_DIR}")
      message(STATUS "FBS_COPY_TEXT_SCHEMA_DIR  : ${FBS_COPY_TEXT_SCHEMA_DIR}")
      message(STATUS "FBS_DEPENDENCIES          : ${FBS_DEPENDENCIES}")
      message(STATUS "FBS_FLATC_ARGUMENTS       : ${FBS_FLATC_ARGUMENTS}")
      message(STATUS "FBS_FILENAME_EXT          : ${FBS_FILENAME_EXT}")
      message(STATUS "FBS_FILENAME_SUFFIX       : ${FBS_FILENAME_SUFFIX}")
      message(STATUS "FBS_INCLUDE_DIR           : ${FBS_INCLUDE_DIR}")

    endif(FBS_VERBOSE)

    # ALl the generated created by this function
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
        set(GENERATED_INCLUDE ${FBS_GENERATED_INCLUDE_DIR}/${FILENAME}${ARGFBS_FILENAME_SUFFIX}.${ARGFBS_FILENAME_EXT})

        # Add the rule for each files
        message(STATUS "Add rule to build ${FILENAME}${ARGFBS_FILENAME_SUFFIX}.${ARGFBS_FILENAME_EXT} from ${SRC}")
        add_custom_command(
          OUTPUT ${GENERATED_INCLUDE}
          COMMAND flatc ${FBS_FLATC_ARGUMENTS}
          -o ${FBS_GENERATED_INCLUDE_DIR}
          ${INCLUDE_PARAMS}
          -c ${SRC}
          ${FBS_FILENAME_EXT}
          ${FBS_FILENAME_SUFFIX}
          ${FBS_REFLECT_NAMES}
          ${FBS_GEN_NAME_STRINGS}
          ${FBS_GEN_MUTABLE}
          ${FBS_GEN_OBJECT_API}
          ${FBS_SCOPED_ENUMS}
          DEPENDS flatc ${SRC} ${FBS_DEPENDENCIES}
          COMMENT "Generate ${GENERATED_INCLUDE} from ${SRC}"
          WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
        list(APPEND ALL_GENERATED_FILES ${GENERATED_INCLUDE})

        if(FBS_RC_SCHEMAS)

          # Name of the output generated file
          set(GENERATED_RC ${FBS_GENERATED_INCLUDE_DIR}/${FILENAME}${ARGFBS_FILENAME_RC_SUFFIX}.${ARGFBS_FILENAME_EXT})

          message(STATUS "Add rule to build ${GENERATED_RC} from ${SRC}")
          add_custom_command(
            OUTPUT ${GENERATED_RC}
            COMMAND flat2h
            -i ${SRC}
            -o ${FBS_GENERATED_INCLUDE_DIR}
            ${FBS_FILENAME_EXT}
            ${FBS_FILENAME_RC_SUFFIX}
            DEPENDS flat2h ${SRC} ${FBS_DEPENDENCIES}
            COMMENT "Generate ${GENERATED_RC} from ${SRC}"
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
          list(APPEND ALL_GENERATED_FILES ${GENERATED_RC})
        endif()

        endif() # (NOT ${FBS_GENERATED_INCLUDE_DIR} STREQUAL "")

        # Should we also build bfbs
        if (NOT ${FBS_BINARY_SCHEMA_DIR} STREQUAL "")

          # Name of the output binary buffer file
          set(BINARY_SCHEMA ${FBS_BINARY_SCHEMA_DIR}/${FILENAME}.bfbs)
          message(STATUS "Add rule to build ${FILENAME}.bfbs from ${SRC}")
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
            message(STATUS "Add rule to copy ${FILENAME}.fbs")
            add_custom_command(
              OUTPUT ${COPY_SCHEMA}
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
              DEPENDS flatc ${ALL_GENERATED_FILES} ${FBS_DEPENDENCIES}
              )


          endfunction()
