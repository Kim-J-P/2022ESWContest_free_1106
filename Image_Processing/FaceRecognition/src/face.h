#pragma once
#include <iostream>
#include <cmath>
#include <librealsense2/rs.hpp>
#include "opencv2/opencv.hpp"
#include "opencv2/face.hpp"
#include <thread>
#include <queue>
#include <vector>
#include <fstream>
/*
#ifdef _DEBUG
#pragma comment(lib, "opencv_world454d.lib")

#else
#pragma comment(lib, "opencv_world454.lib")
#endif
*/
using namespace std;
using namespace cv;
using namespace cv::face;

Mat Video_streaming();
Mat Image_preprocessing(Mat src);
Mat Face_extract(CascadeClassifier face_cascade, Mat src, Mat src_gray);
vector<Point2f> Landmark_extract(Mat face, vector<Rect> rect);
void drawLandmarksPoints(cv::Mat& frame, vector<cv::Point2f>& landmarks);
vector<double> cal_ratio(vector<Point2f>& point);
double get_ratio(double distance1, double distance2);
double pixels_distance(vector<Point2f>& point, int point1, int point2);
String Compare_result(vector<double> value1, vector<double> value2);
void Save_result(string com_result);
