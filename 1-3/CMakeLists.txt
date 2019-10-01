cmake_minimum_required(VERSION 2.8)
project(test3)
set(CMAKE_CXX_STAMDARD 14)
add_executable(test3
  test3.cpp
)
find_package(OpenCV REQUIRED)
target_link_libraries(test3 ${OpenCV_LIBS})
