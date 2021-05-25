/*
@����ͼ�񻺳���.
*/
#ifndef FACE_BUFFER_H
#define FACE_BUFFER_H
#include <vector>

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class FaceBuffer
{
public:
    FaceBuffer();
    virtual ~FaceBuffer();

    void Init();
    void push_back(Mat &frame); /*�����µ�����ͼ��֡��ջ����ͼ�񻺳���.*/
    vector<Mat> peep() const;

private:
    void clear();               /*�ͷ�����ͼ�񻺳���.*/

    int buffer_size_;           /*����ͼ�񻺳�����С.*/
    vector<Mat>frames_;         /*����ͼ�񻺳�������.*/
};

#endif /*FACE_BUFFER_H*/