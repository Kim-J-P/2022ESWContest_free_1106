#pragma once
#include <iostream>
#include <cmath>
#include "opencv2/opencv.hpp"
#include "opencv2/face.hpp"

#ifdef _DEBUG
#pragma comment(lib, "opencv_world454d.lib")

#else
#pragma comment(lib, "opencv_world454.lib")
#endif // _DEBUG

#define CAM_WIDTH 640 
#define CAM_HEIGHT 480

#define max 100

using namespace std;
using namespace cv;
