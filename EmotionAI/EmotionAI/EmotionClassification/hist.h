#ifndef HIST_H
#define HIST_H
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

/*
@��ȡ�Ҷ�ֱ��ͼ�ֲ�.
*/
void GetHistValue(const Mat & gray_image, Mat & hist_image);

#endif /*HIST_H*/