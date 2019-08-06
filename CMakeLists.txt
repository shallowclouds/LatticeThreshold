cmake_minimum_required(VERSION 3.10)
project(bipic)

set(CMAKE_CXX_STANDARD 11)

add_executable(bipic main.cpp CJPG/CJPG.cpp CJPG/CJPG.h)
add_executable(opencv_test opencv_test.cpp)

FIND_PACKAGE(OpenCV REQUIRED)
IF(OpenCV_FOUND)
    INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
    TARGET_LINK_LIBRARIES(bipic ${OpenCV_LIBS})
    TARGET_LINK_LIBRARIES(opencv_test ${OpenCV_LIBS})
ELSE(OpenCV_FOUND)
    MESSAGE(FATAL_ERROR "OpenCV library not found")
ENDIF(OpenCV_FOUND)
