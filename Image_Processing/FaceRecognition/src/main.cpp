#include "main.h"
#include "face.h"
#include "setting.h"

using namespace cv;
using namespace std;

vector<double> cal_ratio( vector<Point2f>& point);
double get_ratio(double distance1, double distance2);
double pixels_distance(vector<Point2f>& point, int point1, int point2);

String filename1 = "data/KJP1.json";
String filename2 = "data/KJP2.json";


int main()
{
	//< 1. 등록사진 가져오기> =====================================
	Mat src1,src1_gray,src2,src2_gray,src1_fil,src2_fil;
	
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

	//imshow("readfile1", src1);
	//imshow("readfile2", src2);
	//waitKey();
	//destroyAllWindows();


	//< 2. 비교할 사진 캡쳐하기 > ==================================
	//Mat capture;
	//VideoCapture cam(2);
	//if(!cam_setting(cam)) return -1;
	//cam.read(capture);
		

	//< 3. 이미지 전처리 과정  >==============================================
	cvtColor(src1, src1_gray, COLOR_BGR2GRAY);
	cvtColor(src2, src2_gray, COLOR_BGR2GRAY);
	//bilateralFilter(src1_gray, src1_fil,-1, 10, 5);//양방향 필터 에지가 무뎌지는 것을 방지
	//bilateralFilter(src2_gray, src2_fil,-1, 10, 5);
	equalizeHist(src1_gray,src1_gray);//영상의 픽셀 값 분포가 그레이스케일 전체 영역에서 골고루 나타나도록 변경
	equalizeHist(src2_gray,src2_gray);
	
	
	// < 4-1. 얼굴 검출 >=======================================================
	CascadeClassifier face_cascade;
	if(!face_cascade.load("data/harr_casecade/haarcascade_frontalface_default.xml"))
	{
        cout << "\n\nError loading face cascade\n\n" << endl;
        return -1;
    }

	Ptr<Facemark> facemark = FacemarkLBF::create();
	facemark->loadModel("data/lbfmodel.yaml");

	vector<Rect> faces1;
	vector<Rect> faces2;

	face_cascade.detectMultiScale(src1_gray, faces1);
	cout<< "x : " << faces1[0].x << " y : " << faces1[0].y << " width : " << faces1[0].width << " height : " << faces1[0].height << endl;
	Mat face1 = src1(faces1[0]);
	//imshow("face1", face1);
	vector<Rect> rect1 = {{0,0,face1.cols,face1.rows},};

	face_cascade.detectMultiScale(src2_gray, faces2);
	cout<< "x : " << faces2[0].x << " y : " << faces2[0].y << " width : " << faces2[0].width << " height : " << faces2[0].height << endl;
	Mat face2 = src2(faces2[0]);
	resize(face2,face2,faces1[0].size());
	//imshow("face2", face2);
	vector<Rect> rect2 = {{0,0,face2.cols,face2.rows},};
	
	
	//< 4-2. 특징점 검출>=======================================================
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

	// < 4-3. 특징점 계산 >=====================================================	
	vector<double> value1 = cal_ratio(points1);
	vector<double> value2 = cal_ratio(points2);
	


	// < 4-4. 비교 및 판단 >====================================================
	/*double result = (ratio1/ratio2)*100;
	if( result > 99 )
		cout << " Correct " << endl;
	else
		cout << " Wrong " << endl;
*/
	imshow("Facial Landmark1 Detection", face1);
	imshow("Facial Landmark2 Detection", face2);
    waitKey();


}
//===============================================================================
double pixels_distance(vector<Point2f>& point, int point1, int point2)
{
	double result = sqrt( pow((point[point1].x-point[point2].x),2) +  pow((point[point1].y-point[point2].y),2) );

	return result;
}

double get_ratio(double distance1, double distance2)
{
	double ratio;
	if( distance1 > distance2 )
		ratio = distance1 / distance2;
	else ratio = distance2 / distance1;
	cout << " distance 1 : " << distance1 << " / distance 2 : " << distance2 << " / 비율 : " << ratio << endl ;
	
	return ratio;
}

vector<double> cal_ratio( vector<Point2f>& point)
{
	vector<double> result;
	//**< 왼쪽 눈~아랫입술 중앙+ 오른쪽 눈~아랫입술 중앙 / 미간길이 >
	double q1 = pixels_distance(point,36,57);
	double q2 = pixels_distance(point,45,57);
	double distance1 = q1+q2;
	double distance2 = pixels_distance(point,39,42);
	result.push_back(get_ratio(distance1, distance2));
	
	//**<왼쪽 콧볼~왼쪽 눈썹 + 오른쪽 콧볼~ 오른쪽 눈썹 / 코 가로 길이>
	q1 = pixels_distance(point,31,39);
	q2 = pixels_distance(point,35,42);
	distance1 = q1+q2;
	distance2 = pixels_distance(point,31,35);
	result.push_back(get_ratio(distance1, distance2));
	
	return result;
}


