/*
@Uniform Pattern, ԭʼ�������ݽ�ά.
*/
#ifndef UNIFORM_PATTERN_H
#define UNIFORM_PATTERN_H

#include <opencv2/opencv.hpp>

using namespace cv;

class UniformPattern
{
public:
    UniformPattern();
    virtual ~UniformPattern();

    void LoadUniformPattern();      /*����Uniform Pattern.*/
    int IndexOf(const unsigned V);  /*Uniform Pattern ��ά.*/

private:
    Mat uniform_pattern_;           /*�ȼ�ģʽ����, ���ݾ�Ϊ�ȼ�ģʽ��.*/
    int bits_;                      /*����������.*/
};

#endif /*UNIFORM_PATTERN_H*/