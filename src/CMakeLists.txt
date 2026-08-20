cmake_minimum_required(VERSION 3.10)

# project() MUST precede find_package(Qt...). Before project() no language is
# enabled, so TARGET_SUPPORTS_SHARED_LIBS is false and Qt5Config creates its
# imported targets STATIC ("ADD_LIBRARY called with SHARED option but the target
# platform does not support dynamic linking"). CMake then links a STATIC imported
# target through IMPORTED_LOCATION (bin\Qt5*.dll) and never consults
# IMPORTED_IMPLIB (lib\Qt5*.lib), so link.exe is handed .dll files and fails with
# LNK1107: invalid or corrupt file.
#
# ${PROJECT_SOURCE_DIR} cannot be used for the version file here: standalone it is
# empty until project() runs, and once project() moves up it points at src/ rather
# than the repo root. Address the file relative to this list file instead, which
# is the repo root both standalone and from the root build.
file(STRINGS ${CMAKE_CURRENT_LIST_DIR}/../release_version.txt X_PROJECT_VERSION)

project(die_lib VERSION ${X_PROJECT_VERSION} LANGUAGES CXX)
option(USE_DIE "Enable Detect It Easy integration" ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
#find_package(Qt4 4.8.6 REQUIRED QtCore QtScript)
#find_package(Qt4 4.8.7 REQUIRED QtCore QtScript)

if(${QT_VERSION_MAJOR} EQUAL 5)
    find_package(Qt5 REQUIRED COMPONENTS Script)
    find_package(Qt5 REQUIRED COMPONENTS Concurrent)
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Qml)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Concurrent)
endif()

include(../dep/build_tools/cmake/deploy.cmake)

set(X_COMPANYNAME "ntinfo")
set(X_PROJECTNAME "dielib")
set(X_MAINTAINER "horsicq@gmail.com")
set(X_DESCRIPTION "Detect It Easy library")
set(X_HOMEPAGE "https://github.com/horsicq/die_library")
set(X_ORIGINAL_FILENAME "dielib")

deploy_init()

add_subdirectory(../dep/XCapstone/x86 XCapstone)
add_subdirectory(../dep/XArchive XArchive)
add_subdirectory(../dep/Formats/xsimd XSIMD)
add_subdirectory(lib dielib)
