#include "face_buffer.h"

#include <QString>

#include "global.h"

FaceBuffer::FaceBuffer() {}

FaceBuffer::~FaceBuffer() {}

/*
@��ʼ��FaceBuffer��.
*/
void FaceBuffer::Init()
{
    frames_.clear();

    Mat f(xSize, ySize, CV_8UC1, Scalar(0));
    for (unsigned int i = 0;i < BUFFER_MAX_SIZE;++i)
    {
        frames_.push_back(f);
    }
    buffer_size_ = BUFFER_MAX_SIZE;
}

/*
@�����µ�����ͼ��֡��ջ����ͼ�񻺳���.
@���: frame, ���µ�һ֡����ͼ��.
@����: frames_, ����ͼ�񻺳����.
@����ֵ: ��.
*/
void FaceBuffer::push_back(Mat &frame)
{
    frames_.erase(frames_.begin());
    frames_.push_back(frame);
}

/*
@��ȡ����ͼ�񻺳����.
*/
vector<Mat> FaceBuffer::peep() const
{
    /*@TEST@
    for (unsigned int i = 0;i < buffer_size_;++i)
    {
        string name = QString::number(i).toStdString();
        imshow(name, frames_.at(i));
    }
    */
    return frames_;
}

/*
@�ͷŻ�����.
*/
void FaceBuffer::clear()
{
    frames_.clear();
}