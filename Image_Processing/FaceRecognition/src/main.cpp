#include "face.h"

string filename1 = "../../data/Register.json";

int main()
{
	Mat src1,src1_gray,src2,src2_gray;
	vector<Rect> faces;
    CascadeClassifier face_cascade("data/harr_casecade/haarcascade_frontalface_default.xml");
    if(face_cascade.empty())
    {   
        cerr << "XML load failed!" << endl;
        return 0; 
    }
	
	//< 1. 스트리밍 및 사진 캡처>================================
	src2 = Video_streaming();
	cvtColor(src2, src2_gray, COLOR_BGR2GRAY);
	equalizeHist(src2_gray,src2_gray);
	face_cascade.detectMultiScale(src2_gray, faces);
	if((faces.size()==0) || (faces.size() >=2)){
		cerr << "Can't find face, try again!!" <<endl;
		return 0;
	}


	//< 2. 등록사진 가져오기> =====================================
	FileStorage fr1(filename1, FileStorage::READ);
	if(!fr1.isOpened()){
		cerr<<"File open failed!"<<endl;
		return 0;
	}
	fr1["data"] >> src1;
	fr1.release();
	//imshow("readfile1", src1);
	//waitKey();
	//destroyAllWindows();


	//< 3. 이미지 전처리 과정  >=====================================
	src1_gray = Image_preprocessing(src1);
	src2_gray = Image_preprocessing(src2);
	
	
	// < 4-1. 얼굴 검출 > ============================================
	Mat face1 = Face_extract(face_cascade,src1, src1_gray); 
	vector<Rect> rect1 = {{0,0,face1.cols,face1.rows},};

	Mat face2 = Face_extract(face_cascade, src2, src2_gray); 
	resize(face2,face2,face1.size());
	vector<Rect> rect2 = {{0,0,face2.cols,face2.rows},};
	
	
	//< 4-2. 특징점 검출> ============================================
	vector<Point2f> points1,points2;
	points1 = Landmark_extract(face1, rect1);
	points2 = Landmark_extract(face2, rect2);


	// < 4-3. 특징점 계산 >=========================================
	vector<double> value1 = cal_ratio(points1);
	vector<double> value2 = cal_ratio(points2);
	

	// < 4-4. 비교 및 판단 >=========================================
	string com_result = Compare_result(value1,value2); 


    // < 5. Save result in TEXT file >=================================
	Save_result(com_result);

	return 0;
}
