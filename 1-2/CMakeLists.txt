cmake_minimum_required(VERSION 2.8)
project(test2)
set(CMAKE_CXX_STAMDARD 14)
add_executable(test2
  test2.cpp
)
find_package(OpenCV REQUIRED)
target_link_libraries(test2 ${OpenCV_LIBS})
