cmake_minimum_required (VERSION 2.6)

project(bad)


set (bad_VERSION_MAJOR 1)
set (bad_VERSION_MINOR 0)

configure_file (
  "${PROJECT_SOURCE_DIR}/include/badConfig.h.in"
  "${PROJECT_BINARY_DIR}/include/badConfig.h"
)

include_directories(
  "${PROJECT_BINARY_DIR}"
)


add_executable(bad src/bad.cpp src/CANbus src/BarrettHand.cpp src/HighLevelBAD.cpp src/kinematics.cpp src/extra_library.cpp)

