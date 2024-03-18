cmake_minimum_required(VERSION 3.10)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
#find_package(Qt4 4.8.7 REQUIRED QtCore QtScript)

if(${QT_VERSION_MAJOR} EQUAL 5)
    find_package(Qt5 REQUIRED COMPONENTS Script)
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Qml)
endif()

include(../dep/build_tools/cmake/deploy.cmake)
file (STRINGS ${PROJECT_SOURCE_DIR}/release_version.txt X_PROJECT_VERSION)

project(XNetDumper VERSION ${X_PROJECT_VERSION} LANGUAGES CXX)

set(CPACK_PACKAGE_INSTALL_DIRECTORY "dielib")
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "dielib")
set(CPACK_PACKAGE_NAME "dielib")
set(CPACK_PACKAGE_VENDOR "ntinfo")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "horsicq@gmail.com")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/horsicq/die_library")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Detect It Easy library")

deploy_init()

add_subdirectory(../dep/XCapstone/x86 XCapstone)
add_subdirectory(../dep/XArchive XArchive)
add_subdirectory(lib dielib)
