cmake_minimum_required(VERSION 2.8)
project(test2)
set(CMAKE_CXX_STANDARD 14)
add_executable(test2
  test2.cpp)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})
target_link_libraries(test2 ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})
