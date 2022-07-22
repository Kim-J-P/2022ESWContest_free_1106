cmake_minimum_required(VERSION 3.1.0)
project(FaceRecognition)

find_package(OpenCV REQUIRED)
set(DEPENDENCIES realsense2 ${OpenCV_LIBS})
#set(HEADER_FILES src/face.h src/setting.h src/IDPef.h)

message( STATUS "OPENCV VERSION ${OpenCV_VERSION}")

add_executable(FaceRecognition src/main.cpp src/face.cpp src/setting.cpp) 
set_property(TARGET FaceRecognition PROPERTY CXX_STANDARD 11)
target_link_libraries(FaceRecognition ${DEPENDENCIES})
