cmake_minimum_required(VERSION 3.10)
option(DIE_BUILD_AS_STATIC "Build DieLib as a static library (instead of shared)" OFF)
project(dielib)
add_subdirectory(src)
