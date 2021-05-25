/*
@�沿����׷����.
*/
#ifndef FACE_TRACKER_H
#define FACE_TRACKER_H
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class FaceTracker
{
public:
    FaceTracker();
    virtual ~FaceTracker();

    void Init();
    Rect FaceDetect(const Mat &frame);   /*��ȡ��������.*/

private:
    CascadeClassifier ccf_;              /*������.*/
    Mat frame_gray_;                     /*�Ҷ�ͼ��.*/
    vector<cv::Rect> face_regions_;      /*����������������.*/
};

#endif /*FACE_TRACKER_H*/