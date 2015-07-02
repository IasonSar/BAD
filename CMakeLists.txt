cmake_minimum_required (VERSION 2.6)
project(bad)

# The version of bad project
set (bad_VERSION_MAJOR 1)
set (bad_VERSION_MINOR 0)


configure_file (
  "${PROJECT_SOURCE_DIR}/include/badConfig.h.in"
  "${PROJECT_BINARY_DIR}/include/badConfig.h"
)


add_library(
  CANbus ${PROJECT_SOURCE_DIR}/barrett_can/CANbus.cpp
  BHand ${PROJECT_SOURCE_DIR}/barrett_can/BarrettHand.cpp  
)

include_directories(
  "${PROJECT_BINARY_DIR}"
  "${PROJECT_SOURCE_DIR}/barrett_can"
)

add_subdirectory(
  barrett_can
)


add_executable(
  bad src/bad.cpp
)

target_link_libraries (
  bad CANbus BHand
)
