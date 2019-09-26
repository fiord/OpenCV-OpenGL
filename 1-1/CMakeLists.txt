cmake_minimum_required(VERSION 2.8)
project(test1)
set(CMAKE_CXX_STAMDARD 14)
add_executable(test1
  test1.cpp
)
find_package(OpenCV REQUIRED)
target_link_libraries(test1 ${OpenCV_LIBS})
