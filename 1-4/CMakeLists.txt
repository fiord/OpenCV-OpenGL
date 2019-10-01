cmake_minimum_required(VERSION 2.8)
project(test4)
set(CMAKE_CXX_STAMDARD 14)
add_executable(test4
  test4.cpp
)
add_executable(test4-2
  test4-2.cpp
)
find_package(OpenCV REQUIRED)
target_link_libraries(test4 ${OpenCV_LIBS})
target_link_libraries(test4-2 ${OpenCV_LIBS})
