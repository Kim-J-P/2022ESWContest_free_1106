#include "setting.h"

bool cam_setting(cv::VideoCapture cam)
{
	bool open = true;
	static int cam_num = 1;
	cam.set(cv::CAP_PROP_FRAME_WIDTH, CAM_WIDTH);
	cam.set(cv::CAP_PROP_FRAME_HEIGHT, CAM_HEIGHT);
	if (!cam.isOpened())
	{
		cout << "\n\nError opening video cam_" << cam_num << "\n\n" << endl;
		open = false;
	}
	cam_num++;
	return open;
}
