cmake_minimum_required(VERSION 2.8)
project(test3)
set(CMAKE_CXX_STANDARD 14)
add_executable(test3
  test3.cpp)
add_executable(eeic
  eeic.cpp)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})
target_link_libraries(test3 ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})
target_link_libraries(eeic ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})
