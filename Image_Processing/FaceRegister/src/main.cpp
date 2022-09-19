#include "opencv2/opencv.hpp"
#include <librealsense2/rs.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <queue>

using namespace cv;
using namespace std;

String filename = "../../data/Register.json";

void video_streaming();
void face_register();

int main()
{
	thread t1(video_streaming);
	t1.join();
	
	face_register();

	return 0;
}

void video_streaming()
{	
    rs2::pipeline pipe;
    rs2::config cfg;

    cfg.enable_stream(RS2_STREAM_COLOR, 848, 480, RS2_FORMAT_BGR8, 30);
    pipe.start(cfg);

    const auto window_name = "Display Image";
    namedWindow(window_name, WINDOW_AUTOSIZE);

    while(1)
	{
        rs2::frameset data = pipe.wait_for_frames();
        rs2::frame color_frame =data.get_color_frame();

        Mat frame(Size(848,480), CV_8UC3, (void*)color_frame.get_data(), Mat::AUTO_STEP);

        imshow(window_name, frame);

		if(waitKey(1) == 27)
			break;
    }
	destroyAllWindows();
}


void face_register()
{
	rs2::pipeline pipe;
	rs2::config cfg;
	cfg.enable_stream(RS2_STREAM_COLOR, 848,480, RS2_FORMAT_BGR8, 30);
	pipe.start(cfg);

	rs2::frameset data = pipe.wait_for_frames();
    rs2::frame color_frame =data.get_color_frame();

	CascadeClassifier classifier("data/haarcascade_frontalface_default.xml");
    if(classifier.empty())
	{
        cerr << "XML load failed!" << endl;
        return ;
    }
	
	//확인용========
	//Mat frame = imread("data/Son2.jpg",IMREAD_GRAYSCALE);
    //	if(frame.empty()){
	//	cerr << "Image load failed!"<<endl;
	//return ;
	//}
	//imshow("a", frame);
	//waitKey();
	//destroyAllWindows();
	//==============

		vector<Rect> faces;
		Mat frame_gray;
		Mat frame(Size(848,480), CV_8UC3, (void*)color_frame.get_data(), Mat::AUTO_STEP);
		cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
		equalizeHist(frame_gray,frame_gray);
    	classifier.detectMultiScale(frame_gray, faces);		// 얼굴 검출
		
		if( (faces.size()==0) || (faces.size()>=2) ){		// 너무 많이 검출되거나 검출안되면 다시
			cerr << "Cant find face, try again !!" << endl;
			return;
		}

	Mat frame_temp = frame;
	rectangle(frame_temp, faces.front(),Scalar(255,0,255),2);
	imshow("a", frame_temp);
	waitKey();

	FileStorage fw(filename, FileStorage::WRITE);
	if(!fw.isOpened()){
		cerr<<"File open failed!"<<endl;
		return ;
	}else cerr << "Storage Success!" << endl;

	fw << "data" << frame;	
	fw.release();

	// 저장파일 확인용
	FileStorage fr(filename, FileStorage::READ);
    if(!fr.isOpened()){
        cerr<<"File open failed!"<<endl;
        return ;
    }
	Mat src;
	fr["data"]>>src;
	fr.release();
	imshow("readfile",src);
	waitKey();
	destroyAllWindows();
}
