#include "opencv2/opencv.hpp"
#include <librealsense2/rs.hpp>
#include <iostream>

using namespace cv;
using namespace std;
int main()
{
	//VideoCapture cap(2);
	//if(!cap.isOpened()){
	//	cerr << "Camera open filed!" << endl;
	//	return 0;
	//}
	
	rs2::pipeline pipe;
	rs2::config cfg;
	
	cfg.enable_stream(RS2_STREAM_COLOR, 848, 480, RS2_FORMAT_BGR8, 30);
	pipe.start(cfg);
	
	
	CascadeClassifier classifier("haarcascade_frontalface_default.xml");
	if(classifier.empty()){
		cerr << "XML load failed!" << endl;
		return 0;
	}

    //cout << "Frame width: " << cvRound(cap.get(CAP_PROP_FRAME_WIDTH)) << endl;
    //cout << "Frame height: " << cvRound(cap.get(CAP_PROP_FRAME_HEIGHT)) << endl;
	//cout << "Frame count: " << cvRound(cap.get(CAP_PROP_FRAME_COUNT)) << endl;

	//double fps = cap.get(CAP_PROP_FPS);
	//cout << "FPS: : : " << fps << endl;

	//int delay = cvRound(1000/fps);

	Mat frame, inversed;
	while(true){
		cap >> frame;
		if(frame.empty())
			break;
		
		vector<Rect> faces;
		classifier.detectMultiScale(frame, faces); 

		for(Rect rc : faces) {
			rectangle(frame, rc, Scalar(255,0,255), 2);
		}
		
		imshow("frame", frame);

		if (waitKey(delay) == 27)
			break;
	}

	destroyAllWindows();
	return 0;
}
