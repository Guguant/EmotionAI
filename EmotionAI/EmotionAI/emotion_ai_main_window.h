/*
@ϵͳ������.
*/
#ifndef EMOTION_H
#define EMOTION_H
#include "ui_emotion_ai_main_window.h"

#include <QDir>
#include <QFileInfoList>
#include <QGraphicsScene>
#include <QLabel>
#include <QTimer>
#include <QtWidgets/QMainWindow>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include "demo_lbp_static_image.h"
#include "demo_lbp_static_video.h"
#include "lbptopsettings.h"
#include "storage_settings.h"

using namespace cv;

namespace Ui {
    class EmotionAiMainWindow;
};

class EmotionAiMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit EmotionAiMainWindow(QWidget *parent = Q_NULLPTR);
    virtual ~EmotionAiMainWindow();

private slots:
    void OnActionDemoImageTriggered();      /*��ʾ��̬ͼƬ��ʾ����.*/
    void OnActionDemoVideoTriggered();      /*��ʾ��̬��Ƶ��ʾ����.*/
    void OnActionLbptopSettingsTriggered(); /*LBP-TOP�������ô���.*/
    void OnActionStorageSettingsTriggered();/*�洢·�����洢ʱ��������.*/
    void OnActionViewLbptopHistTriggered(); /*�鿴��״ͼ.*/
    void OnActionAboutEmotionAiTriggered(); /*����ϵͳ.*/

    void OnOpenCamera();                    /*������ͷ.*/
    void GetCameraFrame();                  /*��������ͷ֡.*/
    void OnOpenVideo();                     /*����Ƶ�ļ�.*/
    void GetVideoFrame();                   /*������Ƶ֡.*/
    void OnOpenImageSeq();                  /*��ͼ������.*/
    void GetImageSeqFrame();                /*����ͼ������֡.*/

    void OnPlayPauseButtonPressed();        /*[����/��ͣ]��ť.*/
    void OnStopReplayButtonPressed();       /*[ֹͣ/�ز�]��ť.*/
    void OnSnipFrameButtonClicked();        /*ץͼ.*/

    void DelayChanged();                    /*ʱ�ӵ�ֵ������.*/
    void SaveEmotionFrame();                /*��ʱ�洢����֡.*/

    void OpenExtension();                   /*�򿪻�ͼչʾ����.*/
    void CloseExtension();                  /*�رջ�ͼչʾ����.*/

private:
    void ShowBufferFrames(const std::vector<Mat> &volume);  /*չʾͼ�񻺳�������.*/
    void ShowLbpTopFrames();                                /*չʾLBP-TOP�ϳ�ͼ��.*/
    void RelaxMusic(const int emotion_label);               /*����.*/

    Ui::EmotionAI *ui_;
    DemoLBPStaticImage *demo_static_image_window_;
    DemoLBPStaticVideo *demo_static_video_window_;
    LbpTopSettings *lbptop_settings_window_;
    StorageSettings *storage_settings_window_;

    bool paused_status_;                    /*[��ͣ/����]��ť״̬.*/
    bool stop_replay_status_;               /*[ֹͣ/�ز�]��ť״̬.*/

    VideoCapture camera_cap_;               /*���ʵ��.*/
    VideoCapture video_cap_;                /*��Ƶʵ��.*/
    Mat frame_;                             /*��������ͷץȡ��֡.*/
    Mat face_;                              /*����Ƶ֡����ȡ������ͼ��.*/
    Mat result_frame_;                      /*���������ɵ�ͼ��.*/
    Mat sample_feature_data_;               /*������������.*/
    Mat xy_;
    Mat xt_;
    Mat yt_;
    Mat hist_image_;

    QTimer *camera_timer_;                  /*�����ʱ��.*/
    QTimer *video_timer_;                   /*��Ƶ��ʱ��.*/
    QTimer *image_timer_;                   /*ͼ�����ж�ʱ��.*/
    QTimer *save_image_timer_;              /*����ͼ��ʱ�洢.*/

    QImage q_image_;
    QImage frame_xy_image_;
    QImage frame_xt_image_;
    QImage frame_yt_image_;
    QImage q_hist_image_;

    QGraphicsScene *main_view_scene_;
    QGraphicsScene *histogram_view_scene_;
    QString file_path_;
    QString directory_path_;                /*�򿪵��ļ��е�·��.*/
    QFileInfoList file_info_list_;          /*directory_path_�ļ������ļ���Ϣ�б�.*/

    int file_index_;                        /*�򿪵�ͼ���������.*/
    int fps_;                               /*���֡��.*/
    int delay_;                             /*ʱ��.*/
    char media_type_;                       /*ý������ 
                                                c: camera;
                                                v: video;
                                                i: image sequence.
                                            */
};

#endif /*EMOTION_H*/