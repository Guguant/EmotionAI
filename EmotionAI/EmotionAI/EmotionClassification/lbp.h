#ifndef LBP_H
#define LBP_H
#include "opencv2/opencv.hpp"

using namespace cv;

/*
@��ȡͼ���LBP����.
*/
Mat GetLBP(const Mat& src_image, Mat& dst_lbp_image);

#endif /*LBP_H*/