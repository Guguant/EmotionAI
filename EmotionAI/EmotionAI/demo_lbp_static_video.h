/*
@��̬��Ƶ����.
@����: ����һ����Ƶ����, ����ÿһ֡�е���������, ͳ�Ʋ�ͬ���������.
*/
#ifndef DEMO_LBP_STATIC_VIDEO_H
#define DEMO_LBP_STATIC_VIDEO_H
#include "ui_demo_lbp_static_video.h"

#include <QLabel>
#include <QTimer>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include "ec_global.h"

using namespace cv;

/*
@��Ƶ�ļ���Ϣ.
*/
struct VideoInfo
{
    double rate;           /*֡��.*/
    long int tatal_frame;  /*��֡��.*/
    int frame_width;       /*֡��.*/
    int frame_height;      /*֡��.*/
};

/*
@��ͬ���������.
*/
struct EmotionNum
{
    unsigned int anger_num;
    unsigned int disgust_num;
    unsigned int fear_num;
    unsigned int happy_num;
    unsigned int neutral_num;
    unsigned int sadness_num;
    unsigned int surprise_num;
};

namespace Ui
{
    class DemoLBPStaticVideo;
};

class DemoLBPStaticVideo : public QDialog
{
    Q_OBJECT

public:
    explicit DemoLBPStaticVideo(QWidget *parent = Q_NULLPTR);
    virtual ~DemoLBPStaticVideo();

private slots:
    void OnLoadVideoButtonClicked();            /*���ؾ�̬��Ƶ�ļ�·��.*/
    void OnPlayVideoButtonClicked();            /*��ʼ������Ƶ�������б���ʶ��.*/
    void UpdateEmotionLabel();                  /*���±����ǩ���֣�չʾÿһ�ֱ��������.*/
private:
    void InitVideoInfo();                       /*��ʼ����Ƶ��Ϣ.*/
    void GetVideoInfo();                        /*��ȡ��Ƶ��Ϣ.*/
    void ShowVideoInfo();                       /*չʾ��Ƶ��Ϣ.*/

    void InitEmotionNum();                      /*��ʼ�����ֱ�������.*/
    void CountEmotionNum(const EmotionType em); /*���㲻ͬ���������.*/

    Ui::DemoLBPStaticVideo *dsv_ui_;

    QString video_file_path_;                   /*��Ƶ�ļ�·��.*/
    bool is_valid_video_file_path_;             /*��Ƶ·���ļ����Ƿ���Ч.*/
    VideoCapture video_cam_;                    /*����ͷʵ��.*/
    Mat input_frame_;                           /*������Ƶ��ÿһ֡.*/
    QTimer *expression_label_num_timer_;        /*��ʱ�����ڶ�ʱ���±���ͳ������.*/

    struct VideoInfo video_info_;               /*�����ص���Ƶ�ļ���Ϣ.*/
    struct EmotionNum emotion_num_;             /*��ͬ���������.*/
};

#endif /*DEMO_LBP_STATIC_VIDEO_H*/