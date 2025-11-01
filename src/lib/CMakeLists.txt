cmake_minimum_required(VERSION 3.10)

project(die LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(${PROJECT_SOURCE_DIR})

if(WIN32)
    add_definitions(-DWIN32)
    if(${QT_VERSION_MAJOR} LESS 5)
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:wchar_t-")
        set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zc:wchar_t-")
        set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zc:wchar_t-")
        set (CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /Zc:wchar_t-")
    endif()
else()
    add_definitions(-fPIC)
endif()

add_definitions(
    -DQT_DEPRECATED_WARNINGS
    -DLIB_SOURCE_LIBRARY
    -DUSE_DEX
    -DUSE_PDF
    -DUSE_ARCHIVE
    -DUSE_YARA
)

include(${CMAKE_CURRENT_LIST_DIR}/../../dep/die_script/die_script.cmake)
include(GNUInstallDirs)

set(PROJECT_SOURCES
        ${DIE_SCRIPT_SOURCES}
        die_lib.cpp
        die_lib.h
)

if(DIE_BUILD_AS_STATIC)
    message(STATUS "Building DieLib as static")
    add_library(die STATIC
        ${PROJECT_SOURCES}
    )
else()
    message(STATUS "Building DieLib as shared")
    add_library(die SHARED
        ${PROJECT_SOURCES}
    )
endif()

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

target_link_libraries(die PRIVATE bzip2)
target_link_libraries(die PRIVATE lzma)
target_link_libraries(die PRIVATE zlib)
target_link_libraries(die PRIVATE ppmd)
target_link_libraries(die PRIVATE capstone_x86)

if(${QT_VERSION_MAJOR} EQUAL 4)
    target_link_libraries(die PRIVATE Qt4::QtCore)
    target_link_libraries(die PRIVATE Qt4::QtScript)
    target_link_libraries(die PRIVATE Qt4::Concurrent)
endif()

if(${QT_VERSION_MAJOR} EQUAL 5)
    target_link_libraries(die PRIVATE Qt${QT_VERSION_MAJOR}::Core)
    target_link_libraries(die PRIVATE Qt${QT_VERSION_MAJOR}::Script)
    target_link_libraries(die PRIVATE Qt${QT_VERSION_MAJOR}::Concurrent)
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    target_link_libraries(die PRIVATE Qt${QT_VERSION_MAJOR}::Core)
    target_link_libraries(die PRIVATE Qt${QT_VERSION_MAJOR}::Qml)
    target_link_libraries(die PRIVATE Qt${QT_VERSION_MAJOR}::Concurrent)
endif()

if(WIN32)
    target_link_libraries(die PRIVATE Wintrust)
    target_link_libraries(die PRIVATE Crypt32)
    target_link_libraries(die PRIVATE comsuppwd)
endif()

if(WIN32)
    install (TARGETS die DESTINATION "./")
    install (DIRECTORY ../../dep/Detect-It-Easy/db DESTINATION "./")
    install (FILES ../include/die.h DESTINATION "./include"  OPTIONAL)
    deploy_qt()
    deploy_msvc()
else()
    install (TARGETS die DESTINATION ${CMAKE_INSTALL_LIBDIR})
    install (DIRECTORY ../../dep/Detect-It-Easy/db DESTINATION "${CMAKE_INSTALL_LIBDIR}/dielib" OPTIONAL)
    install (FILES ../include/die.h DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/dielib"  OPTIONAL)
endif()
