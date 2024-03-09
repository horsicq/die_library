cmake_minimum_required(VERSION 3.14)

project(bzip2 LANGUAGES C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${PROJECT_SOURCE_DIR}/src/)

add_library(bzip2 STATIC
${PROJECT_SOURCE_DIR}/src/bzip2.c
${PROJECT_SOURCE_DIR}/src/crctable.c
${PROJECT_SOURCE_DIR}/src/bzlib.c
${PROJECT_SOURCE_DIR}/src/compress.c
${PROJECT_SOURCE_DIR}/src/decompress.c
${PROJECT_SOURCE_DIR}/src/blocksort.c
${PROJECT_SOURCE_DIR}/src/randtable.c
${PROJECT_SOURCE_DIR}/src/huffman.c
)

set_target_properties(bzip2 PROPERTIES LINKER_LANGUAGE C)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()
