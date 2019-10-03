cmake_minimum_required(VERSION 2.8)
project(test5)
set(CMAKE_CXX_STANDARD 14)
add_executable(test5
  test5.cpp)
<<<<<<< HEAD
add_executable(test5-2
  test5-2.cpp)
=======
>>>>>>> d60e1e951292fd884f3ebd95a88f45824bee53f0
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(OpenCV REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})
<<<<<<< HEAD
target_link_libraries(test5 ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${OpenCV_LIBS} m)
target_link_libraries(test5-2 ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${OpenCV_LIBS} m)
=======
target_link_libraries(test5 ${OpenCV_LIBS} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})
>>>>>>> d60e1e951292fd884f3ebd95a88f45824bee53f0
