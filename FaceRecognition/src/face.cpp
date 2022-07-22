#include "main.h"
#include "face.h"

//drawLandmark

//===<landmark °ËÃâÇÏ±â>=======
vector<cv::Point2f> imgFaceDetection(cv::Mat frame, cv::CascadeClassifier faceCascade, cv::Ptr<Facemark> facemark)
{
    cv::Mat frame_gray;
    vector<cv::Rect> faces;
    vector<vector<cv::Point2f>> landmarks;
    vector<cv::Point2f> points;

    cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY); // gray image º¯È¯

    equalizeHist(frame_gray, frame_gray); // È÷½ºÅä±×·¥ ÆòÈ°È­

    faceCascade.detectMultiScale(frame_gray, faces); // ¾ó±¼ °ËÃâ

    bool success = facemark->fit(frame, faces, landmarks);  // °ËÃâµÈ ¾ó±¼¿¡¼­ Æ¯Â¡Á¡ Ã£±â

    if (success)
    {
      /*drawLandmarksPoints(frame, landmarks[0]);
        drawLandmarks(frame, landmarks[0]);*/

        for (int j = 0; j < landmarks[0].size(); j++)
            points.push_back(cv::Point2f(landmarks[0][j].x, landmarks[0][j].y)); //Æ¯Â¡Á¡ ÁÂÇ¥¸¦ vector¿¡ ÀúÀå
    }
    return points;
}


//===< landmark 에 점으로 ㅍ시하기 >===
void drawLandmarksPoints(cv::Mat& frame, vector<cv::Point2f>& landmarks)
{
    //점 그리기
    for (int i = 0; i < 68; i++)
        line(frame, landmarks[i], landmarks[i], cv::Scalar(0, 0, 255), 3);
}

//===< point잇기>=============
void drawPolyline(cv::Mat& frame, const vector<cv::Point2f>& landmarks, const int start, const int end, bool isClosed)
{
    // 시작점과 끝점들을  points에 넣음
    vector <cv::Point> points;
    for (int i = start; i <= end; i++)
    {
        points.push_back(cv::Point(landmarks[i].x, landmarks[i].y));
    }
    // 폴리라인 그리기
    polylines(frame, points, isClosed, cv::Scalar(0, 100, 255), 2, 16);
}

//===<landkmark끼리 잇기 >=======
void drawLandmarks(cv::Mat& frame, vector<cv::Point2f>& landmarks)
{
    // 68 점을 통해 얼굴을 그림
    if (landmarks.size() == 68)
    {
        drawPolyline(frame, landmarks, 0, 16);     // 턱라인
        drawPolyline(frame, landmarks, 17, 21);    // 왼쪽 눈썹
        drawPolyline(frame, landmarks, 22, 26);    // 오른쪽 눈썹
        drawPolyline(frame, landmarks, 27, 30);    // 코 가운데 라인 
        drawPolyline(frame, landmarks, 30, 35);    // 코 사이드
        drawPolyline(frame, landmarks, 36, 41);    // 왼쪽 눈
        drawPolyline(frame, landmarks, 42, 47);    // 오른쪽 눈
        drawPolyline(frame, landmarks, 48, 59);    // 입술 바깥
        drawPolyline(frame, landmarks, 60, 67);    // 입술 안
    }
    else
    {   // 점의 수가 68개가 아닌 경우, 어떤 점이 얼굴 특징에 ㅐ당하는 지 알 수 없기에
        // landmark 당 하나의 점을 그림
        for (int i = 0; i < landmarks.size(); i++)
            circle(frame, landmarks[i], 3, cv::Scalar(255, 100, 0), cv::FILLED);
    }
}




//==========================================================================================================================
//===< DelaunayTriangles ¢¯¢¯¢¯¢¯  ¢¯¢¯¢¯¢¯ > =======
static void calcDelaunayTriangles(cv::Rect rect, vector<cv::Point2f>& points, vector< vector<int> >& delaunayTri, cv::Mat img)
{
    cv::Mat img_tmp = img.clone();
    cv::Subdiv2D subdiv(rect);

    for (vector<cv::Point2f>::iterator it = points.begin(); it != points.end(); it++)
    {
        if ((*it).x <= 0)(*it).x = 1;
        if ((*it).y <= 0)(*it).y = 1;
        if ((*it).x >= CAM_WIDTH)(*it).x = CAM_WIDTH-1;
        if ((*it).y >= CAM_HEIGHT)(*it).y = CAM_HEIGHT-1;
        subdiv.insert(*it); // getTriangledList ¢¯ ¢¯¢¯ suvdiv¢¯ insert
    }

    vector<cv::Vec6f> triangleList;
    subdiv.getTriangleList(triangleList); // Delaunay Triangulation ¢¯¢¯¢¯¢¯ ¢¯¢¯ ¢¯¢¯(x1,y1)(x2,y2)(x3,y3)
    vector<cv::Point2f> pt(3);
    vector<int> ind(3);

    for (size_t i = 0; i < triangleList.size(); i++)
    {
        cv::Vec6f t = triangleList[i];
        pt[0] = cv::Point2f(t[0], t[1]); // ¢¯¢¯¢¯ ¢¯¢¯¢¯¢¯ ¢¯¢¯ (x1, y1) ¢¯¢¯
        pt[1] = cv::Point2f(t[2], t[3]);
        pt[2] = cv::Point2f(t[4], t[5]);

        if (rect.contains(pt[0]) && rect.contains(pt[1]) && rect.contains(pt[2]))
        {
            for (int j = 0; j < 3; j++)
                for (size_t k = 0; k < points.size(); k++)
                    if (abs(pt[j].x - points[k].x) < 1 && abs(pt[j].y - points[k].y) < 1)
                        ind[j] = k;

            line(img_tmp, pt[0], pt[1], (255, 255, 255), 1, 16, 0);  // line ¢¯¢¯¢¯
            line(img_tmp, pt[1], pt[2], (255, 255, 255), 1, 16, 0);
            line(img_tmp, pt[2], pt[0], (255, 255, 255), 1, 16, 0);

            delaunayTri.push_back(ind);
        }
    }
}

//===< 들로네 삼각형을 채우기/ 데려올 사진의 마스크 추출하기>===
void warpTriangle(cv::Mat& frame_1, cv::Mat& result, vector<cv::Point2f>& t1, vector<cv::Point2f>& t2)
{
    cv::Rect r1 = boundingRect(t1); // 외곽선에 외접하고 똑바로 세워진 직사각형의 좌상단 좌표와 가로 세로 리턴
    cv::Rect r2 = boundingRect(t2);

    if (r1.x <= 0) r1.x = 0;
    if (r1.y <= 0) r1.y = 0;
    if (r1.x >= CAM_WIDTH) r1.x = CAM_WIDTH;
    if (r1.y >= CAM_HEIGHT) r1.y = CAM_HEIGHT;

    if (r2.x <= 0) r2.x = 0;
    if (r2.y <= 0) r2.y = 0;
    if (r2.x >= CAM_WIDTH) r2.x = CAM_WIDTH;
    if (r2.y >= CAM_HEIGHT) r2.y = CAM_HEIGHT;

    // 각 사각형의 왼쪽 상단 모서리를 기준으로 오프셋 지점
    vector<cv::Point2f> t1Rect, t2Rect;
    vector<cv::Point> t2RectInt;

    for (int i = 0; i < 3; i++)
    {
        t1Rect.push_back(cv::Point2f(t1[i].x - r1.x, t1[i].y - r1.y));
        t2Rect.push_back(cv::Point2f(t2[i].x - r2.x, t2[i].y - r2.y));

        t2RectInt.push_back(cv::Point(t2[i].x - r2.x, t2[i].y - r2.y)); // for fillConvexPoly
    }

    // 삼각형을 채워  mask 가져오기
    cv::Mat mask = cv::Mat::zeros(r2.height, r2.width, CV_32FC3); // r2사각형 생성
    fillConvexPoly(mask, t2RectInt, cv::Scalar(1.0, 1.0, 1.0), 16, 0); // 삼각형 부분만 흰색으로

    // 작은 직사각형 패치들에  warpimage 적용
    cv::Mat img1Rect;
    frame_1(r1).copyTo(img1Rect);

    cv::Mat img2Rect = cv::Mat::zeros(r2.height, r2.width, img1Rect.type());

    //srcTri 와  dstTri¢를 사용하여 계산된 어파인 변환을 src에 적용
    cv::Mat warpMat = getAffineTransform(t1Rect, t2Rect);
    warpAffine(img1Rect, img2Rect, warpMat, img2Rect.size(), cv::INTER_LINEAR, cv::BORDER_REFLECT_101);

    multiply(img2Rect, mask, img2Rect);

    multiply(result(r2), cv::Scalar(1.0, 1.0, 1.0) - mask, result(r2));
    result(r2) = result(r2) + img2Rect;
}

//===< 얼굴 바꾸기 > ========
void processFaceSwap(cv::Mat& frame_1, cv::Mat& frame_2, cv::Mat& result, cv::Ptr<Facemark> facemark, cv::CascadeClassifier face_cascade)
{
	cv::Mat ori_frame_1 = frame_1.clone(); // original image
	cv::Mat ori_frame_2 = frame_2.clone();

	cv::Mat cpy_frame_1 = frame_1.clone(); // copy image
	cv::Mat cpy_frame_2 = frame_2.clone();

	//// 1. Face Alignment
	//// 1-1. Facial Landmark Detection

	vector<cv::Point2f> face_points1 = imgFaceDetection(frame_1, face_cascade, facemark); // face points 얻기
	vector<cv::Point2f> face_points2 = imgFaceDetection(frame_2, face_cascade, facemark);

	bool flag = true;

	if (face_points1.size() == 0 || face_points2.size() == 0)  flag = false; // 얼굴 인식x 일때

	if (flag)
	{
		frame_1.convertTo(frame_1, CV_32F); // CV_32F로 변환
		result.convertTo(result, CV_32F);

		vector<cv::Point2f> hull1;
		vector<cv::Point2f> hull2;
		vector<int> hullIndex;

		convexHull(face_points2, hullIndex, false, false); // face_point의 외곽선을 찾아 hullIndex(방향)에 저장

		for (int i = 0; i < hullIndex.size(); i++)
		{
			if (face_points1[hullIndex[i]].x <= 0) face_points1[hullIndex[i]].x = 0;					// 예
			if (face_points1[hullIndex[i]].y <= 0) face_points1[hullIndex[i]].y = 0;					// 외
			if (face_points1[hullIndex[i]].x >= CAM_WIDTH) face_points1[hullIndex[i]].x = CAM_WIDTH;	// 처
			if (face_points1[hullIndex[i]].y >= CAM_HEIGHT) face_points1[hullIndex[i]].y = CAM_HEIGHT;	// 리

			if (face_points2[hullIndex[i]].x <= 0) face_points2[hullIndex[i]].x = 0;					// 예
			if (face_points2[hullIndex[i]].y <= 0) face_points2[hullIndex[i]].y = 0;					// 외
			if (face_points2[hullIndex[i]].x >= CAM_WIDTH) face_points2[hullIndex[i]].x = CAM_WIDTH;	// 처
			if (face_points2[hullIndex[i]].y >= CAM_HEIGHT) face_points2[hullIndex[i]].y = CAM_HEIGHT;	// 리

			hull1.push_back(face_points1[hullIndex[i]]);
			hull2.push_back(face_points2[hullIndex[i]]);  // 외곽선을 순서대로 hull2에 저장
		}

		//// 1-3. Delaunay Triangulation
		vector<vector<int>> dt;
		cv::Rect rect(0, 0, result.cols, result.rows); // 결과를 저장할 사각형

		calcDelaunayTriangles(rect, hull2, dt, frame_2); // delaunay triangulation 

		for (size_t i = 0; i < dt.size(); i++)
		{
			vector<cv::Point2f> t1, t2;
			// 삼격형들에 해당하는 img1, img2에 대한 점을 구함
			for (size_t j = 0; j < 3; j++)
			{
				t1.push_back(hull1[dt[i][j]]);
				t2.push_back(hull2[dt[i][j]]);
			}

			warpTriangle(frame_1, result, t1, t2); // 삼각형 마스크 계산

			line(ori_frame_1, hull1[dt[i][0]], hull1[dt[i][1]], (255, 255, 255), 1, 16, 0);
			line(ori_frame_1, hull1[dt[i][1]], hull1[dt[i][2]], (255, 255, 255), 1, 16, 0);
			line(ori_frame_1, hull1[dt[i][2]], hull1[dt[i][0]], (255, 255, 255), 1, 16, 0);
		}
		// mask 계산

		vector<cv::Point> hull8U;
		for (int i = 0; i < hull2.size(); i++)
		{
			cv::Point pt(hull2[i].x, hull2[i].y);
			hull8U.push_back(pt);
		}

		cv::Mat mask = cv::Mat::zeros(frame_2.rows, frame_2.cols, frame_2.depth());
		fillConvexPoly(mask, &hull8U[0], hull8U.size(), cv::Scalar(255, 255, 255)); // 마스크그리기

		//// 2. Seamless Cloning
		cv::Rect r = boundingRect(hull2);  // img2에 대한 convex hull을 사각형으로
		cv::Point center = (r.tl() + r.br()) / 2;  // img2에 대한 convex hull의 중심점을 찾음

		cv::Mat img_seamlessCloned;

		result.convertTo(result, CV_8UC3); 

		seamlessClone(result, frame_2, mask, center, img_seamlessCloned, cv::NORMAL_CLONE); // 피부색 맞추기
		result = img_seamlessCloned;
	}
	else
		result = ori_frame_2;

	frame_1 = cpy_frame_1;
	frame_2 = cpy_frame_2;
}

/*
void pengsoo(cv::Mat frame, cv::Mat &result, cv::CascadeClassifier face_cascade)
{
    cv::Mat peng = cv::imread("images/peng.png");
    vector<cv::Rect> faces;
    cv::Mat frame_gray;
    cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
    cv::equalizeHist(frame_gray, frame_gray);
    face_cascade.detectMultiScale(frame_gray, faces);
    if (faces.size() > 0)
    {
        for (int i = 0; i < faces.size(); i++)
        {
            cv::resize(peng, peng, faces[i].size());
            cv::Mat roi(frame, cv::Rect(faces[i].x, faces[i].y, peng.cols, peng.rows));

            cv::Mat peng_gray, mask, mask_inv;
            cv::threshold(peng, mask, 1, 255, cv::THRESH_BINARY);
            cv::bitwise_not(mask, mask_inv);

            cv::Mat fg = peng.clone();
            cv::Mat bg = roi.clone();
            cv::bitwise_and(roi, mask_inv, bg);
            cv::bitwise_and(peng, mask, fg);
            cv::Mat dst;
            cv::add(fg, bg, dst);
            dst.copyTo(roi);
            result = frame;
        }
    }
}
*/
