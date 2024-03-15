cmake_minimum_required(VERSION 3.10)

project(die_library LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(${PROJECT_SOURCE_DIR})

if(WIN32)
  add_definitions(-DWIN32)
endif()

add_definitions(
    -DQT_DEPRECATED_WARNINGS
    -DLIB_SOURCE_LIBRARY
    )

include(${CMAKE_CURRENT_LIST_DIR}/../../dep/die_script/die_script.cmake)

set(PROJECT_SOURCES
        ${DIE_SCRIPT_SOURCES}
        die_lib.cpp
        die_lib.h
)
    
add_library(die_library SHARED
    ${PROJECT_SOURCES}
)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

target_link_libraries(die_library PRIVATE bzip2)
target_link_libraries(die_library PRIVATE lzma)
target_link_libraries(die_library PRIVATE zlib)
target_link_libraries(die_library PRIVATE capstone_x86)

target_link_libraries(die_library PRIVATE Qt${QT_VERSION_MAJOR}::Core)

if(${QT_VERSION_MAJOR} EQUAL 5)
    target_link_libraries(die_library PRIVATE Qt${QT_VERSION_MAJOR}::Script)
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    target_link_libraries(die_library PRIVATE Qt${QT_VERSION_MAJOR}::Qml)
endif()

if(WIN32)
    target_link_libraries(die_library PRIVATE Wintrust)
    target_link_libraries(die_library PRIVATE Crypt32)
    target_link_libraries(die_library PRIVATE comsuppwd)
endif()

if(WIN32)
    install (TARGETS die_library DESTINATION "./")
    install (DIRECTORY ../../dep/Detect-It-Easy/db DESTINATION "./")
    install (DIRECTORY ../../dep/Detect-It-Easy/db_custom DESTINATION "./")
    install (FILES ../include/die.h DESTINATION "./include"  OPTIONAL)
    deploy_qt()
    deploy_msvc()
else()
    install (TARGETS die_library DESTINATION ${CMAKE_INSTALL_LIBDIR})
    install (DIRECTORY ../../dep/Detect-It-Easy/db DESTINATION ${CMAKE_INSTALL_LIBDIR}/die_library OPTIONAL)
    install (DIRECTORY ../../dep/Detect-It-Easy/db_custom DESTINATION ${CMAKE_INSTALL_LIBDIR}/die_library OPTIONAL)
    install (FILES ../include/die.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/die_library  OPTIONAL)
endif()
