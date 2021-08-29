# FlatBuffersCMake

[![CI](https://github.com/OlivierLDff/FlatBuffersDlc/actions/workflows/main.yml/badge.svg)](https://github.com/OlivierLDff/FlatBuffersDlc/actions/workflows/main.yml)

## What is it?

This project provide a simple CMake macro to build flatc executable and generate flatbuffers c++ headers. Including the `CMakeLists.txt` also build the library `flatbuffers` that you can link to.

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
    RC_SCHEMAS
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

**RC_SCHEMAS**

Create a C++ header that embed into a `const char*` the content `.fbs`. This is useful if you need the schema to be embedded as a string into your binary. This avoid the need to deploy your schema to parse json files.

Here is an adaptation of `sample_text.cpp` with the monster generated file.

```c++
#include "flatbuffers/idl.h"
#include "flatbuffers/util.h"
#include "monster_generated.h"  
#include "monster_rc.h" 

using namespace MyGame::Sample;

// This is an example of parsing text straight into a buffer and then
// generating flatbuffer (JSON) text from the buffer.
int main(int /*argc*/, const char * /*argv*/[]) {
  // load FlatBuffer schema (.fbs) and JSON from disk
  std::string schemafile;
  std::string jsonfile;
  bool ok = flatbuffers::LoadFile("samples/monsterdata.json", false, &jsonfile);
  if (!ok) {
    printf("couldn't load files!\n");
    return 1;
  }

  // parse schema first, so we can use it to parse the data after
  flatbuffers::Parser parser;
  const char *include_directories[] = { "samples", nullptr };
  ok = parser.Parse(MonsterRc.data(), include_directories, "samples") &&
       parser.Parse(jsonfile.c_str(), include_directories);
  assert(ok);

  // here, parser.builder_ contains a binary buffer that is the parsed data.

  // to ensure it is correct, we now generate text back from the binary,
  // and compare the two:
  std::string jsongen;
  if (!GenerateText(parser, parser.builder_.GetBufferPointer(), &jsongen)) {
    printf("Couldn't serialize parsed data to JSON!\n");
    return 1;
  }

  if (jsongen != jsonfile) {
    printf("%s----------------\n%s", jsongen.c_str(), jsonfile.c_str());
  }

  printf("The FlatBuffer has been parsed from JSON successfully.\n");
}

```

**GEN_OBJECT_API**

Generate an additional object-based API. This API is more convenient for object construction and mutation than the base API, at the cost of efficiency (object allocation). Recommended only to be used if other options are insufficient.
Associated flag is `--gen-object-api`.

**GEN_NAME_STRINGS**

Generate type name functions for C++.
Associated flag is `--gen-name-strings`.

**GEN_MUTABLE**

Generate additional non-const accessors for mutating FlatBuffers in-place.
Associated flag is `--gen-mutable`.

**GEN_COMPARE**

Generate operator== for object-based API types.
Associated flag is `--gen-compare`.

**GEN_SHARED_PTR**

If flag `GEN_SHARED_PTR` is set, then `--cpp-ptr-type "std::shared_ptr"` will be appended to `flatc` args.
Default is `std::unique_ptr`.
More information in [Using different string type.](https://google.github.io/flatbuffers/flatbuffers_guide_use_cpp.html#autotoc_md44)

**SCOPED_ENUMS**

Use C++11 style scoped and strongly typed enums in generated C++. This also implies --no-prefix.
Associated flag is `--scoped-enums`.

**DEPENDENCIES**

Add dependencies to the target `Target`.

**VERBOSE**

Print the configuration during build time.

## Related Link

* [FlatBuffers official repository.](https://github.com/google/flatbuffers)
* [FlatBuffers documentation](https://google.github.io/flatbuffers/).

## Contact

- Olivier Le Doeuff: [olivier.ldff@gmail.com](mailto:olivier.ldff@gmail.com)
