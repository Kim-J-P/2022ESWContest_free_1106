#include "main.h"
#include "face.h"
#include "setting.h"

using namespace cv;
using namespace std;
double cal_ratio( vector<Point2f>& point1);
String filename1 = "data/KJP1.json";
String filename2 = "data/KJP2.json";
int main()
{
	//<등록사진 가져오기> =====================================
	Mat src1,src1_gray,src2,src2_gray;
	
	FileStorage fr(filename1, FileStorage::READ);
	if(!fr.isOpened()){
		cerr<<"File open failed!"<<endl;
		return 0;
	}
	fr["data"] >> src1;
	fr.release();
	FileStorage fr1(filename2, FileStorage::READ);
	if(!fr1.isOpened()){
		cerr<<"File open failed!"<<endl;
		return 0;
	}
	fr1["data"] >> src2;
	fr1.release();

	imshow("readfile1", src1);
	imshow("readfile2", src2);
	waitKey();
	destroyAllWindows();

	//Mat src1_gray, src2_gray;
	//Mat src1 = imread(filename1, IMREAD_COLOR);
	//Mat src2 = imread(filename2, IMREAD_COLOR);
	//if(src1.empty()||src2.empty()){
	//	cerr << "Image load failed!" << endl;
	//	return -1;
	//}

	CascadeClassifier face_cascade;
	if(!face_cascade.load("data/harr_casecade/haarcascade_frontalface_default.xml"))
	{
        cout << "\n\nError loading face cascade\n\n" << endl;
        return -1;
    }

	Ptr<Facemark> facemark = FacemarkLBF::create();
	facemark->loadModel("data/lbfmodel.yaml");


	//< 비교할 사진 캡쳐하기 > ==================================
	//VideoCapture cam(2);
	//if(!cam_setting(cam)) return -1;
		
	
	//============================================================
	//<얼굴 검출>=========
	vector<Rect> faces1;
	vector<Rect> faces2; // 왼쪽상단 좌표, width와 height 값
	cvtColor(src1, src1_gray, COLOR_BGR2GRAY);
	cvtColor(src2, src2_gray, COLOR_BGR2GRAY);
	equalizeHist(src1_gray,src1_gray);
	equalizeHist(src2_gray,src2_gray);


	face_cascade.detectMultiScale(src1_gray, faces1);
	cout<< "x : " << faces1[0].x << " y : " << faces1[0].y << " width : " << faces1[0].width << " height : " << faces1[0].height << endl;
	Mat face1 = src1(faces1[0]);
	imshow("face1", face1);
	vector<Rect> rect1 = {{0,0,face1.cols,face1.rows},};


	face_cascade.detectMultiScale(src2_gray, faces2);
	cout<< "x : " << faces2[0].x << " y : " << faces2[0].y << " width : " << faces2[0].width << " height : " << faces2[0].height << endl;
	Mat face2 = src2(faces2[0]);
	resize(face2,face2,faces1[0].size());
	imshow("face2", face2);
	vector<Rect> rect2 = {{0,0,face2.cols,face2.rows},};
	//============================================================
	//<특징점 검출>========
	vector< vector<Point2f> > landmarks1, landmarks2;
	vector<Point2f> points1, points2;

	bool success1 = facemark->fit(face1 , rect1, landmarks1);
	bool success2 = facemark->fit(face2 , rect2, landmarks2);
	
	if(success1)
	{
		for(int i = 0 ; i<landmarks1[0].size() ; i++)
		{
			points1.push_back(Point2f(landmarks1[0][i].x, landmarks1[0][i].y));
		}
	}
	drawLandmarksPoints(face1,points1);
	if(success2)
	{
		for(int i = 0; i < landmarks2[0].size(); i++)
        {
			points2.push_back(Point2f(landmarks2[0][i].x, landmarks2[0][i].y));
        }
    }
	drawLandmarksPoints(face2,points2);

	//================================================================
	line(face1, points1[36], points1[57], Scalar(0,0,255),3);//왼쪽 눈 ~ 아랫입술 중앙
	line(face1, points1[57], points1[45], Scalar(0,0,255),3);//오른쪽 눈 ~ 아랫입술 중앙
	line(face1, points1[36], points1[45], Scalar(0,0,255),3);//눈 간격
	line(face1, points1[39], points1[42], Scalar(0,0,255),3);// 미간 거리
	line(face1, points1[33], points1[57], Scalar(0,0,255),3);// 인중~아랫입술
	line(face1, points1[48], points1[54], Scalar(0,0,255),3);// 입술 가로길이
	

	double ratio1 = cal_ratio(points1);
	double ratio2 = cal_ratio(points2);
	
    imshow("Facial Landmark1 Detection", face1);
	imshow("Facial Landmark2 Detection", face2);
    waitKey();
}


double cal_ratio( vector<Point2f>& point1)
{
	
	//< 왼쪽 눈~아랫입술 중앙+ 오른쪽 눈~아랫입술 중앙 / 입술가로길이 >
	double q1 = sqrt( pow((point1[36].x-point1[57].x),2) +  pow((point1[36].y-point1[57].y),2) );
	double q2 = sqrt( pow((point1[57].x-point1[45].x),2) +  pow((point1[57].y-point1[45].y),2) );
	double distance1 = q1+q2;
	double distance2 = sqrt( pow((point1[39].x-point1[42].x),2) +  pow((point1[39].y-point1[42].y),2) );
			
	//<  인중~아랫입술 중앙 / 입술가로길이 >
	//double distance1 = sqrt( pow((point1[33].x-point1[57].x),2) +  pow((point1[33].y-point1[57].y),2) );
	//double distance2 = sqrt( pow((point1[48].x-point1[54].x),2) +  pow((point1[48].y-point1[54].y),2) );

	//< 인중~아랫입술 중앙 / 입술가로길이 >
	//double distance1 = sqrt( pow((point1[36].x-point1[45].x),2) +  pow((point1[36].y-point1[45].y),2) );
	//double distance2 = sqrt( pow((point1[39].x-point1[42].x),2) +  pow((point1[39].y-point1[42].y),2) );
	double ratio;
	if( distance1 > distance2 )
		ratio = distance1 / distance2;
	else ratio = distance2 / distance1;
	cout << " distance 1 : " << distance1 << " / distance 2 : " << distance2 << " / 비율 : " << ratio << endl ;

	return ratio;
}
