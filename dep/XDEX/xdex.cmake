include_directories(${CMAKE_CURRENT_LIST_DIR})

include(${CMAKE_CURRENT_LIST_DIR}/../Formats/xbinary.cmake)

set(XDEX_SOURCES
    ${XBINARY_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/xandroidbinary.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xandroidbinary.h
    ${CMAKE_CURRENT_LIST_DIR}/xdex.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xdex.h
    ${CMAKE_CURRENT_LIST_DIR}/xdex_def.h
)
