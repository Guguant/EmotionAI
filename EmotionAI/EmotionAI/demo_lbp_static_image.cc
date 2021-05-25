#include "demo_lbp_static_image.h"

#include <QDebug>
#include <QFileDialog>

#include "ec_global.h"
#include "global.h"

DemoLBPStaticImage::DemoLBPStaticImage(QWidget *parent)
    : QDialog(parent),
    dsi_ui_(new Ui::DemoLBPStaticImage)
{
    dsi_ui_->setupUi(this);
    this->setWindowFlags(Qt::WindowMinimizeButtonHint |
        Qt::WindowMaximizeButtonHint |
        Qt::WindowCloseButtonHint);
    InitMatImage();
    is_valid_file_path_ = false;
    kScaleWidth_ = SHOW_IMAGE_LABEL_SIZE;
    kScaleHeight_ = SHOW_IMAGE_LABEL_SIZE;

    connect(dsi_ui_->loadImageBtn, SIGNAL(clicked()), this, SLOT(OnLoadImageButtonClicked()));
    connect(dsi_ui_->runImageBtn, SIGNAL(clicked()), this, SLOT(OnSingleFaceRecognitionButtonClicked()));
    connect(dsi_ui_->runImageBtn_2, SIGNAL(clicked()), this, SLOT(OnMultiFaceRecognitionButtonClicked()));

    connect(dsi_ui_->viewOriginalImageBtn, SIGNAL(clicked()), this, SLOT(ViewOriginalImageClicked()));
    connect(dsi_ui_->viewGrayImageBtn, SIGNAL(clicked()), this, SLOT(ViewGrayImageClicked()));
    connect(dsi_ui_->viewGrayequalImageBtn, SIGNAL(clicked()), this, SLOT(ViewGrayEqualImageClicked()));
    connect(dsi_ui_->viewFacedetectionImageBtn, SIGNAL(clicked()), this, SLOT(ViewFaceDetectionImageClicked()));
    connect(dsi_ui_->viewFacecutImageBtn, SIGNAL(clicked()), this, SLOT(ViewFaceCutImageClicked()));
    connect(dsi_ui_->viewNormalImageBtn, SIGNAL(clicked()), this, SLOT(ViewNormalImageClicked()));
}

DemoLBPStaticImage::~DemoLBPStaticImage()
{
    delete dsi_ui_;
}

/*
@1. ��ʼ���洢ͼ�����ݵ�Mat�ͱ���.
@2. ��ʼ�����ڻ���ͼ��ı�ǩ.
*/
void DemoLBPStaticImage::InitMatImage()
{
    image_original_.release();
    image_gray_.release();
    image_equalize_hist_.release();
    image_face_detection_.release();
    image_face_cut_.release();
    image_normal_.release();
    image_multi_face_result_.release();

    dsi_ui_->label_original->clear();
    dsi_ui_->label_gray->clear();
    dsi_ui_->label_gray_equal->clear();
    dsi_ui_->label_facedetection->clear();
    dsi_ui_->label_facecut->clear();
    dsi_ui_->label_normal->clear();
    dsi_ui_->label_result->clear();
}

/*
@����ͼ���ļ�.
@���: ��.
@����: image_file_path_, ͼ���ļ�·��.
@����ֵ: ��.
*/
void DemoLBPStaticImage::OnLoadImageButtonClicked()
{
    image_file_path_ = QFileDialog::getOpenFileName(this,
        tr("select a image"),
        QString::fromStdString(TEST_IMAGE_PATH),
        tr("File(*.jpg *.png *.jpeg *.tiff)"));
    if (image_file_path_.isEmpty())
    {
        return;
    }

    InitMatImage();
    image_original_ = imread(image_file_path_.toLocal8Bit().toStdString());
    if (image_original_.empty())
    {
        return;
    }
    is_valid_file_path_ = true;
    CalcScaleNum(image_original_.cols, image_original_.rows);
    QImage qimg = QImage((const unsigned char*)image_original_.data, image_original_.cols,
                                               image_original_.rows, image_original_.step,
                                               QImage::Format_RGB888).rgbSwapped();
    qimg = qimg.scaled(kScaleWidth_, kScaleHeight_, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    dsi_ui_->label_original->setPixmap(QPixmap::fromImage(qimg));
}

/*
@����׵ı���ʶ��.
@���: ��.
@����: ��.
@����ֵ: ��.
*/
void DemoLBPStaticImage::OnSingleFaceRecognitionButtonClicked()
{
    if (false == is_valid_file_path_)
    {
        return;
    }
    if (image_original_.empty())
    {
        return;
    }

    QImage fimg;
    /*ͼ��ҶȻ�.*/
    cvtColor(image_original_, image_gray_, CV_BGR2GRAY);
    fimg = QImage((const unsigned char*)image_gray_.data, image_gray_.cols,
                                        image_gray_.rows, image_gray_.step,
                                        QImage::Format_Grayscale8).rgbSwapped();
    fimg = fimg.scaled(kScaleWidth_, kScaleHeight_, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    dsi_ui_->label_gray->setPixmap(QPixmap::fromImage(fimg));

    /*�Ҷ�ֱ��ͼ���⻯.*/
    equalizeHist(image_gray_, image_equalize_hist_);
    fimg = QImage((const unsigned char*)image_equalize_hist_.data, image_equalize_hist_.cols,
                                        image_equalize_hist_.rows, image_equalize_hist_.step,
                                        QImage::Format_Grayscale8).rgbSwapped();
    fimg = fimg.scaled(kScaleWidth_, kScaleHeight_, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    dsi_ui_->label_gray_equal->setPixmap(QPixmap::fromImage(fimg));

    vector<Rect>faces;

    /*��ȡ��������.*/
    ec.GetFaceArea(image_equalize_hist_, faces);
    if (faces.empty()) 
    {
        dsi_ui_->label_result->setText("0 SUM.");
        return;
    }
    image_face_detection_ = image_original_.clone();
    ec.DrawFaceRect(faces, image_face_detection_);
    fimg = QImage((const unsigned char*)image_face_detection_.data, image_face_detection_.cols,
                                        image_face_detection_.rows, image_face_detection_.step,
                                        QImage::Format_RGB888).rgbSwapped();
    fimg = fimg.scaled(kScaleWidth_, kScaleHeight_, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    dsi_ui_->label_facedetection->setPixmap(QPixmap::fromImage(fimg));

    /*�ü�������ͼ��.*/
    image_face_cut_ = image_equalize_hist_.clone();
    ec.GetFaceImage(image_equalize_hist_, faces, image_face_cut_);
    fimg = QImage((const unsigned char*)image_face_cut_.data, image_face_cut_.cols,
                                        image_face_cut_.rows, image_face_cut_.step,
                                        QImage::Format_Grayscale8).rgbSwapped();
    CalcScaleNum(image_face_cut_.cols, image_face_cut_.rows);
    fimg = fimg.scaled(kScaleWidth_, kScaleHeight_, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    dsi_ui_->label_facecut->setPixmap(QPixmap::fromImage(fimg));

    /*ͼ��ߴ��һ��.*/
    image_normal_ = image_face_cut_.clone();
    cv::resize(image_normal_, image_normal_,
               Size(SAVE_IMAGE_SIZE_HEIGHT + 2, SAVE_IMAGE_SIZE_WIDTH + 2),
               (0, 0), (0, 0), INTER_LINEAR);
    fimg = QImage((const unsigned char*)image_normal_.data, image_normal_.cols,
                                        image_normal_.rows, image_normal_.step,
                                        QImage::Format_Grayscale8).rgbSwapped();
    dsi_ui_->label_normal->setPixmap(QPixmap::fromImage(fimg));

    Mat lbp_face, lbp_hist;
    GetLBP(image_normal_, lbp_face);
    GetHistValue(lbp_face, lbp_hist);

    /*������ࣺSVM*/
    double t = 0;
    EmotionType et = UNKNOW;
    t = ec.SvmLbpEmotion(lbp_hist);
    et = EmotionType(int(t - 0));
    GetEmotionResult(et);
    /*@OUTPUT: all probability of seven emotion.@
    for (int i =0;i < 7;++i)
        qDebug() << ec.lbp_emotion_probability_[i];
    */
    dsi_ui_->label_result->setText(expression_recognition_result_);
}

/*
@����׵ı���ʶ��.
@���: ��.
@����: ��.
@����ֵ: ��.
*/
void DemoLBPStaticImage::OnMultiFaceRecognitionButtonClicked()
{
    if (false == is_valid_file_path_)
    {
        return;
    }

    if (image_original_.empty())
    {
        return;
    }

    /*�����������.*/
    EmotionType et;
    image_multi_face_result_ = image_original_.clone();
    int sum = 0;
    sum = ec.GetFaceEmotion(image_original_, image_multi_face_result_, et);

    if (!image_multi_face_result_.empty())
    {
        imshow("Multiple Emotion Result.", image_multi_face_result_);
    }
    dsi_ui_->label_result->setText(QString::number(sum) + " SUM.");
}

/*
@չʾ����ʶ����.
*/
void DemoLBPStaticImage::GetEmotionResult(const EmotionType em)
{
    switch (em)
    {
    case ANGER:
        expression_recognition_result_ = "Anger";
        break;
    case DISGUST:
        expression_recognition_result_ = "Disgust";
        break;
    case FEAR:
        expression_recognition_result_ = "Fear";
        break;
    case HAPPY:
        expression_recognition_result_ = "Happy";
        break;
    case NEUTRAL:
        expression_recognition_result_ = "Neutral";
        break;
    case SADNESS:
        expression_recognition_result_ = "Sadness";
        break;
    case SURPRISE:
        expression_recognition_result_ = "Surprise";
        break;
    case UNKNOW:
        expression_recognition_result_ = "UNKNOW";
        break;
    default:
        expression_recognition_result_ = "UNKNOW";
        break;
    }
}

/*
@Ԥ��ԭͼ.
*/
void DemoLBPStaticImage::ViewOriginalImageClicked()
{
    if (!image_original_.empty())
    {
        imshow("ԭͼ", image_original_);
    }
}

/*
@Ԥ���Ҷ�ͼ.
*/
void DemoLBPStaticImage::ViewGrayImageClicked()
{
    if (!image_gray_.empty())
    {
        imshow("�Ҷ�ͼ", image_gray_);
    }
}

/*
@Ԥ��ֱ��ͼ���⻯���ͼ��.
*/
void DemoLBPStaticImage::ViewGrayEqualImageClicked()
{
    if (!image_equalize_hist_.empty())
    {
        imshow("ֱ��ͼ���⻯", image_equalize_hist_);
    }
}

/*
@Ԥ����⵽������ͼ��.
*/
void DemoLBPStaticImage::ViewFaceDetectionImageClicked()
{
    if (!image_face_detection_.empty())
    {
        imshow("�����������", image_face_detection_);
    }
}

/*
@Ԥ������������ú��ͼ��.
*/
void DemoLBPStaticImage::ViewFaceCutImageClicked()
{
    if (!image_face_cut_.empty())
    {
        imshow("�����������", image_face_cut_);
    }
}

/*
@Ԥ���ߴ��һ�����ͼ��.
*/
void DemoLBPStaticImage::ViewNormalImageClicked()
{
    if (!image_normal_.empty())
    {
        imshow("�ߴ��һ��", image_normal_);
    }
}

/*
@����ͼ�����������.
@���: width, ԭʼͼ����; height, ԭʼͼ��߶�.
@����: ScaleWidht, չʾ��ͼ����; kScaleHeight_, չʾ��ͼ��߶�.
@����ֵ: ��.
*/
void DemoLBPStaticImage::CalcScaleNum(const int width, const int height)
{
    double a = width;
    double b = height;
    int scaleNum = 1;
    if ((a / SHOW_IMAGE_LABEL_SIZE) > (b / SHOW_IMAGE_LABEL_SIZE))
    {
        scaleNum = a / SHOW_IMAGE_LABEL_SIZE;
    }
    else
    {
        scaleNum = b / SHOW_IMAGE_LABEL_SIZE;
    }
    scaleNum = (scaleNum < 1) ? 1 : scaleNum;
    kScaleWidth_ = width / scaleNum;
    kScaleHeight_ = height / scaleNum;
}