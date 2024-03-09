include_directories(${CMAKE_CURRENT_LIST_DIR})
include_directories(${CMAKE_CURRENT_LIST_DIR}/3rdparty/Capstone/src/include/)
# TODO Check includes
set(XCAPSTONE_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/xcapstone.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xcapstone.h
)
