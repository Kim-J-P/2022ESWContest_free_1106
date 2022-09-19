cmake_minimum_required(VERSION 3.1.0)
project(FaceRecognition)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)
set(DEPENDENCIES realsense2 ${OpenCV_LIBS})


#message( STATUS "OPENCV VERSION ${OpenCV_VERSION}")

add_executable(FaceRecognition src/main.cpp src/face.cpp) 
set_property(TARGET FaceRecognition PROPERTY CXX_STANDARD 14)
target_link_libraries(FaceRecognition ${DEPENDENCIES} Threads::Threads)

