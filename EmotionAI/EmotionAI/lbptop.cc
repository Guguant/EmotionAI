#include "lbptop.h"

#include "global.h"

#define ELEM(start,step,xpos,ypos) (*((uchar*)(start+step*(ypos)+(xpos)*sizeof(uchar))))

LbpTop::LbpTop()
{
    nbr_table_ = new Point[g_Pxy];
}

LbpTop::~LbpTop()
{
    delete[] nbr_table_;
}

void LbpTop::Init()
{
    /*Ĭ��Ϊ8����.*/
    nbr_table_[0] = Point(1, 0);
    nbr_table_[1] = Point(1, -1);
    nbr_table_[2] = Point(0, -1);
    nbr_table_[3] = Point(-1, -1);
    nbr_table_[4] = Point(-1, 0);
    nbr_table_[5] = Point(-1, 1);
    nbr_table_[6] = Point(0, 1);
    nbr_table_[7] = Point(1, 1);
    p_.LoadUniformPattern();
}

/*
@��ȡLBP-TOPͼ���Hist��״ͼ.
@���: P, ����������; r1, X�᷽�������뾶; r2, Y�᷽�������뾶.
@����: ��.
@����ֵ:lbptop_to_hist, LBP-TOPͼ���Hist��״ͼ.
*/
Mat LbpTop::GetLBPtoHist(Mat &frame, const int P, const int r1, const int r2)
{
    unsigned int bins = unsigned int(P * (P - 1) + 3);
    Mat lbptop_to_hist(1, bins, CV_32FC1, Scalar(0));

    unsigned char thresh = 0;
    unsigned int V = 0;
    int uniform_V = 0;
    int xp, yp = 0;
    bool vp = false;
    for (unsigned int yc = r2;yc < frame.rows - r2;++yc)
    {
        for (unsigned int xc = r1;xc < frame.cols - r1;++xc)
        {
            thresh = ELEM(frame.data, frame.step, xc, yc);
            V = 0;
            for (int i = P - 1; i >= 0;--i)
            {
                xp = xc + int(r1 * nbr_table_[i].x);
                yp = yc + int(r2 * nbr_table_[i].y);
                vp = (ELEM(frame.data, frame.step, xp, yp) >= thresh ? 1 : 0);
                V = (V << 1) | int(vp);
            }
            /*Uniform Pattern��ά.*/
            uniform_V = p_.IndexOf(V);
            /*�ǵȼ���ģʽ, lbptop_to_hist���������һλ�洢�ǵȼ�ģʽ������.*/
            if (uniform_V == -1)
            {
                (*((float*)(lbptop_to_hist.data + (bins - 1) * sizeof(float))))++;
            }
            else /*�ȼ���ģʽ.*/
            {
                (*((float*)(lbptop_to_hist.data + (uniform_V) * sizeof(float))))++;
            }
        }
    }
    return lbptop_to_hist;
}

/*
@Histͼ�����ݹ淶��������ֵ�ռ�Ϊ[0, 1).
*/
void LbpTop::NormalizeHist(Mat &matG)
{

    float sum = 0;
    for (int i = 0;i < matG.cols;++i)
    {
        sum += (*((float*)(matG.data + (i) * sizeof(float))));
    }
    if (sum == 0)
    {
        return;
    }
    for (int i = 0;i < matG.cols;++i)
    {
        (*((float*)(matG.data + (i) * sizeof(float)))) = (*((float*)(matG.data + (i) * sizeof(float)))) * 1.0 / sum;
    }
}

/*
@����LBP-TOP������״ͼ.
@���: volume, ����ͼ������.
@����: histTo, LBP-TOP������״ͼ.
*/
Mat LbpTop::GenerateSampleData(const vector<Mat> &volume)
{
    /*uniform pattern code.*/
    int binsXY = (g_Pxy * (g_Pxy - 1) + 3);
    int binsXT = (g_Pxt * (g_Pxt - 1) + 3);
    int binsYT = (g_Pyt * (g_Pyt - 1) + 3);
    int bins = binsXY + binsXT + binsYT;

    /*ͼ�񻮷ֵ��ܿ���.*/
    int blockCount = xBlocks * yBlocks;

    /*histV, ͼ���ӿ��LBP-TOP����; histTo, ����ͼ������LBP-TOP����.*/
    Mat histV(1, bins, CV_32FC1, cv::Scalar(0));
    Mat histTo(1, bins * blockCount, CV_32FC1, cv::Scalar(0));
    Mat matXY_G(1, binsXY, CV_32FC1, cv::Scalar(0));
    Mat matXT_G(1, binsXT, CV_32FC1, cv::Scalar(0));
    Mat matYT_G(1, binsYT, CV_32FC1, cv::Scalar(0));

    /*ͼ�����е�����.*/
    int size = 0;
    size = volume.size();

    /*xy, xt, yt����ά�ȵ�����ͼ��.*/
    full_frame_xy = volume.at(size - 1);  /*�����������һ��ͼ��*/
    full_frame_xt = Mat(size * yBlocks, xSize,  CV_8UC1, Scalar(255));
    full_frame_yt = Mat(ySize, size * xBlocks, CV_8UC1, Scalar(255));

    int xWidth = xSize / xBlocks;
    int yWidth = ySize / yBlocks;

    /*xy, xt, yt����ά�ȵ��ӿ�����ͼ��*/
    cv::Mat frame_xy(yWidth, xWidth, CV_8UC1);
    cv::Mat frame_xt(size, xWidth, CV_8UC1);
    cv::Mat frame_yt(yWidth, size, CV_8UC1);
 
    unsigned int i, j, m, n = 0;
    for (unsigned k = 0;k < blockCount;++k)
    {
        i = k % xBlocks;
        j = k / xBlocks;
        m = xWidth * i;    /*��ǰ�ӿ����Ͻ����ص�ĺ�����.*/
        n = yWidth * j;    /*��ǰ�ӿ����Ͻ����ص��������.*/
        Rect r(m, n, xWidth, yWidth);   /*m, n, ���Ͻǵ�����; xWidth, yWidth,���εĳߴ�.*/

        for (unsigned int l = 0;l < size;++l)
        {
            Mat f = volume.at(l)(r);
            f.col(f.cols / 2).copyTo(frame_yt.col(l));
            f.row(f.rows / 2).copyTo(frame_xt.row(l));
        }
        volume.at(size - 1)(r).copyTo(frame_xy);

        /*���ֲ�������Ƶ�ȫ������.*/
        r = Rect(m, size * j, xWidth, size);
        frame_xt.copyTo(full_frame_xt(r));
        r = Rect(size * i, n, size, yWidth);
        frame_yt.copyTo(full_frame_yt(r));

        /*matXY_G: X-Yά�ֲ�������״ͼ.*/
        matXY_G = GetLBPtoHist(frame_xy, g_Pxy, g_Rx, g_Ry);
        NormalizeHist(matXY_G);
        matXY_G.colRange(0, binsXY).copyTo(histV.colRange(0, binsXY));
        /*matXT_G: X-Tά�ֲ�������״ͼ.*/
        matXT_G = GetLBPtoHist(frame_xt, g_Pxt, g_Rx, g_Rt);
        NormalizeHist(matXT_G);
        matXT_G.colRange(0, binsXT).copyTo(histV.colRange(binsXY, (binsXY + binsXT)));
        /*matYT_G: Y-Tά�ֲ�������״ͼ.*/
        matYT_G = GetLBPtoHist(frame_yt, g_Pyt, g_Rt, g_Ry);
        NormalizeHist(matYT_G);
        matYT_G.colRange(0, binsYT).copyTo(histV.colRange((binsXY + binsXT), (binsXY + binsXT + binsYT)));

        /*bins = 59 * 3��k <= blockCount * 14 * 14.*/
        histV.colRange(0, bins).copyTo(histTo.colRange(bins*k, bins*(k + 1)));
    }

    frame_xy.release();
    frame_xt.release();
    frame_yt.release();
    return histTo;
}

/*
@�Ŵ�Histͼ��ֵ, �Կ��ӻ�.
*/
Mat LbpTop::GetHistImage(const Mat &hist)
{
    cv::Mat hist_image(256, hist.cols, CV_8UC1, cv::Scalar(255));
    float binVal = 0.0;
    int intensity = 0;
    for (int i = 0; i< hist.cols; i++)
    {
        binVal = hist.at<float>(i);
        intensity = static_cast<int>(binVal * 300);
        cv::line(hist_image, cv::Point(i, 255), cv::Point(i, 255-intensity), cv::Scalar(0,0,255), 3);
    }
    return hist_image;
}

/*
@��ȡLBP-TOP֡.
@���: .
@����: xy, X-Yά����ͼ��; xt, X-Tά����ͼ��; yt, Y-Tά����ͼ��.
@����ֵ: ��.
*/
void LbpTop::GetFullFrame(Mat &xy, Mat &xt, Mat &yt) const
{
    xy = full_frame_xy;
    xt = full_frame_xt;
    yt = full_frame_yt;
}
