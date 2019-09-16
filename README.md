# FlatBuffersCMake

## What is it?

This project provide a simple CMake macro to build flatc executable and generate flatbuffers c++ headers.

This macro use `FetchContent` cmake's command to download and build [flatbuffers](https://github.com/google/flatbuffers). 

It is based on the `BuildFlatBuffers.cmake` file in this project. 

## How to use

Copy the content of the file is what you want or use FetchContent.

```cmake
FetchContent_Declare(
    flatbufferscmake
    GIT_REPOSITORY "https://github.com/OlivierLDff/FlatBuffersCMake"
    GIT_TAG        "master"
)
FetchContent_MakeAvailable(flatbufferscmake)
```

Then you need to call the macro that has been registered.

```cmake
set(FBS_SRCS ...)
set(FBS_INCLUDE_DIR ...)

add_fbs_target(FbsTarget ${FBS_SRCS}
    GENERATED_INCLUDE_DIR "path/to/generated"
    BINARY_SCHEMA_DIR "path/to/bfbs"
    COPY_TEXT_SCHEMA_DIR "path/to/fbs"
    FLATC_ARGUMENTS "--gen-mutable"
    INCLUDE_DIR ${FBS_INCLUDE_DIR}
    DEPENDENCIES ...
    VERBOSE
    )
```

## Options

```cmake
add_fbs_target(Target Sources
    [GENERATED_INCLUDE_DIR dir]
    [BINARY_SCHEMA_DIR dir]
    [COPY_TEXT_SCHEMA_DIR dir]
    [INCLUDE_DIR dir]
    [FLATC_ARGUMENTS arg arg arg ...]
    [DEPENDENCIES depend depend ...]
    [VERBOSE]
    )
```

**Target**

The output target you will add to your project with `add_dependencies` command.

**Sources**

A list of all the `.fbs` files for which we need to generate a c++ header.

**GENERATED_INCLUDE_DIR**

Path to the directory where C++ headers are going to be generated. If no path is specified, no headers file will be generated.

**BINARY_SCHEMA_DIR**

Path to the directory to generate `.bfbs` files.

**COPY_TEXT_SCHEMA_DIR**

Path to the directory where `.fbs` are going to be copied.

**INCLUDE_DIR**

List of directories to include for flat buffers include statements. This will be turn into `-I` arguments for `flatc`.

**FLATC_ARGUMENTS**

Custom arguments appended to `flatc` when generating C++ headers. Options can be found [here](https://google.github.io/flatbuffers/flatbuffers_guide_using_schema_compiler.html).

**DEPENDENCIES**

Add dependencies to the target `Target`.

**VERBOSE**

Print the configuration during build time.

## Related Link

* [FlatBuffers official repository.](https://github.com/google/flatbuffers)
* [FlatBuffers documentation](https://google.github.io/flatbuffers/).

## Contact

- Olivier Le Doeuff: [olivier.ldff@gmail.com](mailto:olivier.ldff@gmail.com)