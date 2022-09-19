#include "face.h"

Mat Video_streaming()
{
    Mat src,src_gray,src_fil;

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
		src = frame;
    
		if(waitKey(1) == 27)
            break;
    }
    destroyAllWindows();

    return src;
}


Mat Image_preprocessing(Mat src)
{
    Mat src_gray, src_fil;

    cvtColor(src, src_gray, COLOR_BGR2GRAY);
    bilateralFilter(src_gray, src_fil,-1,10,5);
    equalizeHist(src_fil, src_gray);

    return src_gray;
}


Mat Face_extract(CascadeClassifier face_cascade,Mat src, Mat src_gray)
{
    vector<Rect> faces;
    Mat face;

    face_cascade.detectMultiScale(src_gray, faces);
    face = src(faces[0]);

    return face;
}


vector<Point2f> Landmark_extract(Mat face, vector<Rect> rect)
{
    vector< vector<Point2f>> landmarks;
    Ptr<Facemark> facemark = FacemarkLBF::create();
    facemark->loadModel("data/lbfmodel.yaml");
    vector<Point2f> points;

    bool success = facemark->fit(face, rect, landmarks);

    if(success)
    {
        for(int i = 0; i<landmarks[0].size() ; i++)
        {
            points.push_back(Point2f(landmarks[0][i].x, landmarks[0][i].y));
        }
    }
    drawLandmarksPoints(face,points);

    return points;
}


void drawLandmarksPoints(cv::Mat& frame, vector<cv::Point2f>& landmarks)
{
    //점 그리기
    for (int i = 0; i < 68; i++)
        line(frame, landmarks[i], landmarks[i], cv::Scalar(0, 0, 255), 3);
}


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


string Compare_result(vector<double> value1, vector<double> value2)
{
    double result = value1[0]/value2[0];

    string com_result;
    if(result > 0.99)
    {
        cout << "Correct" << endl;
        com_result = "Correct";
    }
    else
    {
        cout << "Wrong" <<endl;
        com_result = "Worng";
    }

    return com_result;
}


void Save_result(string com_result)
{
    ofstream file("../../data/result.txt");
    if(file.is_open()){
        file << com_result;
        file.close();
    }else cout <<"TXT open failed"<< endl;
}

