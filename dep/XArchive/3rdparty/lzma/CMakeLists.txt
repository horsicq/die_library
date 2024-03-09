cmake_minimum_required(VERSION 3.14)

project(lzma LANGUAGES C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${PROJECT_SOURCE_DIR}/src/)

add_library(lzma STATIC
${PROJECT_SOURCE_DIR}/src/LzmaDec.c
${PROJECT_SOURCE_DIR}/src/Ppmd7.c
${PROJECT_SOURCE_DIR}/src/Ppmd7Dec.c
)

set_target_properties(lzma PROPERTIES LINKER_LANGUAGE C)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()
