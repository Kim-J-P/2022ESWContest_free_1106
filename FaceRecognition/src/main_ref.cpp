#include "main.h"
#include "face.h"
#include "setting.h"

using namespace cv;
using namespace std;

int processCamFaceEyesDetect();
int processCamFacemarkDetect();

int main()
{
	processCamFaceEyesDetect();
	processCamFacemarkDetect();
	return 0;
}

int processCamFaceEyesDetect() {

	CascadeClassifier face_cascade;
	CascadeClassifier eyes_cascade;

	face_cascade.load("data/harr_casecade/haarcascade_frontalface_default.xml");
	eyes_cascade.load("data/harr_casecade/haarcascade_eye.xml");

	// 웹캠 스트림 읽기
	VideoCapture cam(2);
	if (!cam_setting(cam)) return -1;
	
	Mat frame;
	
	while (1) {
		cam.read(frame);
		if (frame.empty()) {
			cout << "<ERROR> No captured frame!" << endl;
			break;
		}
		// 얼굴 감지 및 출력
		Mat frame_gray;
		cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
		equalizeHist(frame_gray, frame_gray);
		std::vector<Rect> faces;
		face_cascade.detectMultiScale(frame_gray, faces);

		for (size_t i = 0; i < faces.size(); i++) {
			Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
			ellipse(frame, center, Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360, Scalar(255, 0, 255), 4);
			Mat faceROI = frame_gray(faces[i]);

			// 눈 감지
			std::vector<Rect> eyes;
			eyes_cascade.detectMultiScale(faceROI, eyes);
			for (size_t j = 0; j < eyes.size(); j++)
			{
				Point eye_center(faces[i].x + eyes[j].x + eyes[j].width / 2, faces[i].y + eyes[j].y + eyes[j].height / 2);
				int radius = cvRound((eyes[j].width + eyes[j].height) * 0.25);
				circle(frame, eye_center, radius, Scalar(255, 0, 0), 4);
			}

		}

		imshow("캡쳐 - face_cascade and eyes_cascade", frame);

		// ESC Detect
		if (waitKey(10) == 27) {
			break; // escape
		}
	}
	destroyAllWindows();
	return 0;
}

// good work
int processCamFacemarkDetect() {

	CascadeClassifier face_cascade;
	CascadeClassifier eyes_cascade;
	// Harr cascade 로드
	if (!face_cascade.load("data/harr_casecade/haarcascade_frontalface_alt2.xml")) //얼굴 검출 알고리즘 로드
	{
		cout << "\n\nError loading face cascade\n\n" << endl;
		return -1;
	}

	// Facemark의 인스턴스 생성
	Ptr<Facemark> facemark = FacemarkLBF::create();

	// landmark 디텍터 로드
	facemark->loadModel("data/lbfmodel.yaml");

	// 웹캠 스트림 읽기
	VideoCapture cam(2);
	if (!cam_setting(cam)) return -1;

	Mat frame, gray;

	// 프레임 읽기
	while (1) {
		cam.read(frame);
		if (frame.empty()) {
			cout << "<ERROR> No captured frame!" << endl;
			break;
		}

		vector<Rect> faces;
		// 얼굴 검출을 위해 그레이스케일 변환
		cvtColor(frame, gray, COLOR_BGR2GRAY);

		// 얼굴 검출
		face_cascade.detectMultiScale(gray, faces);

		vector< vector<Point2f> > landmarks;

		// 랜드마크 검출
		bool success = facemark->fit(frame, faces, landmarks);

		if (success)
		{
			// 성공시
			for (int i = 0; i < landmarks.size(); i++)
			{
				drawLandmarksPoints(frame, landmarks[i]);
			}
		}

		imshow("Facial Landmark Detection", frame);
		if (waitKey(1) == 27) break;
	}
	destroyAllWindows();
}
