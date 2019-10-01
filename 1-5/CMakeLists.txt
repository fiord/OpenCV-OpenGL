cmake_minimum_required(VERSION 2.8)
project(test5)
set(CMAKE_CXX_STAMDARD 14)
add_executable(test5
  test5.cpp
)
add_executable(test5-2
  test5-2.cpp
)
find_package(OpenCV REQUIRED)
target_link_libraries(test5 ${OpenCV_LIBS})
target_link_libraries(test5-2 ${OpenCV_LIBS})
