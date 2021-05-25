/*
@��̬ͼ�������ʾ.
@����������һ�Ŵ���������ͼ��չʾ����ʶ��Ĺ���.
*/
#ifndef DEMO_LBP_STATIC_IMAGE_H
#define DEMO_LBP_STATIC_IMAGE_H
#include "ui_demo_lbp_static_image.h"

#include <QLabel>
#include <QTimer>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include "emotion_classification.h"

/*ͼ��չʾLabel�ĳߴ�.*/
const int SHOW_IMAGE_LABEL_SIZE = 200;

namespace Ui {
    class DemoLBPStaticImage;
};

class DemoLBPStaticImage : public QDialog
{
    Q_OBJECT

public:
    explicit DemoLBPStaticImage(QWidget *parent = Q_NULLPTR);
    ~DemoLBPStaticImage();

private slots:
    void OnLoadImageButtonClicked();                        /*����ͼƬ�ļ�.*/
    void OnSingleFaceRecognitionButtonClicked();            /*����ʶ��(�����).*/
    void OnMultiFaceRecognitionButtonClicked();             /*����ʶ��(�����).*/

    void ViewOriginalImageClicked();                        /*ԭͼ.*/
    void ViewGrayImageClicked();                            /*�Ҷ�ͼ.*/
    void ViewGrayEqualImageClicked();                       /*ֱ��ͼ���⻯.*/
    void ViewFaceDetectionImageClicked();                   /*����ͼ��.*/
    void ViewFaceCutImageClicked();                         /*��������ü�.*/
    void ViewNormalImageClicked();                          /*�ߴ��һ��.*/

private:
    void InitMatImage();                                    /*��ʼ���洢ͼƬ��Mat���ͱ���.*/
    void GetEmotionResult(const EmotionType em);            /*չʾ����ʶ����.*/
    void CalcScaleNum(const int width, const int height);   /*����ͼ�����������.*/

    Ui::DemoLBPStaticImage *dsi_ui_;

    QString image_file_path_;                               /*ͼƬ�ļ�·��.*/
    bool is_valid_file_path_;                               /*ͼƬ·���ļ����Ƿ���Ч.*/

    Mat image_original_;                                    /*���صı���ͼ��.*/
    Mat image_gray_;                                        /*�Ҷ�ͼ.*/
    Mat image_equalize_hist_;                               /*ֱ��ͼ���⻯.*/
    Mat image_face_detection_;                              /*��⵽����������.*/
    Mat image_face_cut_;                                    /*�ü���������.*/
    Mat image_normal_;                                      /*���������һ��.*/

    Mat image_multi_face_result_;                           /*�����ʶ��Ľ��.*/
    QString expression_recognition_result_;                 /*�������͵Ľ��.*/

    int kScaleWidth_;                                       /*չʾͼ���ǩ�����ź�ͼ��Ŀ�.*/
    int kScaleHeight_;                                      /*չʾͼ���ǩ�����ź�ͼ��ĸ�.*/
};

#endif /*DEMO_LBP_STATIC_IMAGE_H*/