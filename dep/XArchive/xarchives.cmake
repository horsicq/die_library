include_directories(${CMAKE_CURRENT_LIST_DIR})

include(${CMAKE_CURRENT_LIST_DIR}/xarchive.cmake)

set(XARCHIVES_SOURCES
    ${XARCHIVE_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/xarchives.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xarchives.h
)
