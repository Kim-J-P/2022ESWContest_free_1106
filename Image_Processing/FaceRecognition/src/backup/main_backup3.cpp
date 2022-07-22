#include "opencv2/opencv.hpp"
#include <librealsense2/rs.hpp>
#include <iostream>

using namespace cv;
using namespace std;
int main()
{
	rs2::pipeline pipe;
	rs2::config cfg;
	
	cfg.enable_stream(RS2_STREAM_COLOR, 848, 480, RS2_FORMAT_BGR8, 30);
	pipe.start(cfg);
		
	CascadeClassifier classifier("haarcascade_frontalface_default.xml");
	if(classifier.empty()){
		cerr << "XML load failed!" << endl;
		return 0;
	}
	
	const auto window_name = "Display Image";
	namedWindow(window_name, WINDOW_AUTOSIZE);

	while(waitKey(1)< 0 && getWindowProperty(window_name, WND_PROP_AUTOSIZE) >= 0) // 
	{	
		rs2::frameset data = pipe.wait_for_frames();
		rs2::frame color_frame =data.get_color_frame();
		
		Mat frame(Size(848,480), CV_8UC3, (void*)color_frame.get_data(), Mat::AUTO_STEP);

		vector<Rect> faces;
		classifier.detectMultiScale(frame, faces); 

		for(Rect rc : faces) {
			rectangle(frame, rc, Scalar(255,0,255), 2);
		}
		
		imshow(window_name, frame);
	}

	destroyAllWindows();
	return 0;

}
