cmake_minimum_required(VERSION 3.1.0)
project(FaceRegister)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)
find_package(OpenCV REQUIRED)
set(DEPENDENCIES realsense2 ${OpenCV_LIBS})

#message( STATUS "OPENCV VERSION ${OpenCV_VERSION}")

add_executable(FaceRegister src/main.cpp)
set_property(TARGET FaceRegister PROPERTY CXX_STANDARD 14)
target_link_libraries(FaceRegister ${DEPENDENCIES} Threads::Threads)  
