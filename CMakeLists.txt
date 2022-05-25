cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_FLAGS "-std=gnu++2a -O3")

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/includes)

set(logger-files
    ${INC_DIR}/Logger.h
    ${SRC_DIR}/Logger.cpp
    ${SRC_DIR}/LoggerImpl.h
)

add_library(logger-lib
    ${logger-files}
)

include_directories(
    ${INC_DIR}
)

set_target_properties(${logger-lib} PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
)