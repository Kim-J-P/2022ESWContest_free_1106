#pragma once
#include "main.h"

using namespace cv::face;

void drawPolyline(cv::Mat& frame, const vector<cv::Point2f>& landmarks, const int start, const int end, bool isClosed = false);
void drawLandmarks(cv::Mat& frame, vector<cv::Point2f>& landmarks);
static void calcDelaunayTriangles(cv::Rect rect, vector<cv::Point2f>& points, vector< vector<int> >& delaunayTri, cv::Mat img);
void drawLandmarksPoints(cv::Mat& frame, vector<cv::Point2f>& landmarks);
void warpTriangle(cv::Mat& frame_1, cv::Mat& result, vector<cv::Point2f>& t1, vector<cv::Point2f>& t2);
vector<cv::Point2f> imgFaceDetection(cv::Mat frame, cv::CascadeClassifier faceCascade, cv::Ptr<Facemark> facemark);
void processFaceSwap(cv::Mat& frame_1, cv::Mat& frame_2, cv::Mat& result, cv::Ptr<Facemark> facemark, cv::CascadeClassifier face_cascade);
void pengsoo(cv::Mat frame, cv::Mat &result, cv::CascadeClassifier face_cascade);
