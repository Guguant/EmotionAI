/*
@����ͼ�����е�LBP-TOP����.
*/
#ifndef LBPTOP_H
#define LBPTOP_H

#include <vector>

#include <opencv2/opencv.hpp>

#include "uniform_pattern.h"

using namespace cv;
using namespace std;

class LbpTop
{
public:
    LbpTop();
    virtual ~LbpTop();

    void Init();

    Mat GenerateSampleData(const vector<Mat> &volume);  /*��ȡ����LBP-TOP����.*/
    Mat GetHistImage(const cv::Mat &hist);              /*�Ŵ�Histͼ��ֵ, �Կ��ӻ�.*/
    void GetFullFrame(Mat &xy, Mat &xt, Mat &yt) const; /*��ȡ������LBP-TOP֡ͼ��.*/

private:
    Mat GetLBPtoHist(Mat &frame, const int P, const int r1, const int r2);
                                                        /*��LBP-TOP����ͼת��ΪHist��״ͼ.*/
    void NormalizeHist(Mat &matG);                      /*��״ͼ���ݹ淶����[0, 1)�ռ�.*/

    UniformPattern p_;
    Point *nbr_table_;
    Mat full_frame_xy;                                  /*X-Yάƽ������ͼ��.*/
    Mat full_frame_xt;                                  /*X-Tάƽ������ͼ��.*/
    Mat full_frame_yt;                                  /*Y-Tάƽ������ͼ��.*/
};

#endif /*LBPTOP_H*/