cmake_minimum_required(VERSION 2.8.3)

project(bad)

# Use C++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CANBUS_LIBS "pcanbasic")

find_package(orocos_kdl)
find_package(catkin REQUIRED
  orocos_kdl # use KDL functionalities
  rviz_visual_tools
  roslint
  geometry_msgs
)

catkin_package(
  INCLUDE_DIRS include
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)

add_library( bad
  src/BarrettHand.cpp
  src/CANbus.cpp
  src/extra_library.cpp
  src/HighLevelBAD.cpp
  src/kinematics.cpp
)

add_dependencies( bad
  ${${PROJECT_NAME}_EXPORTED_TARGETS}
  ${catkin_EXPORTED_TARGETS}
)

target_link_libraries( bad
  ${catkin_LIBRARIES}
  ${CANBUS_LIBS}
)
