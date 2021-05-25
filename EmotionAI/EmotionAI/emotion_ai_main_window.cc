#include "emotion_ai_main_window.h"

#include <QDateTime>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QPainter.h>
#include <QThread>
#include <QTreeView>

#include "ec_global.h"
#include "global.h"

EmotionAiMainWindow::EmotionAiMainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui_(new Ui::EmotionAI)
{
    ui_->setupUi(this);
    demo_static_image_window_ = new DemoLBPStaticImage();
    demo_static_video_window_ = new DemoLBPStaticVideo();
    lbptop_settings_window_ = new LbpTopSettings(this);
    storage_settings_window_ = new StorageSettings(this);

    main_view_scene_ = new QGraphicsScene;
    ui_->mainView->setScene(main_view_scene_);

    this->resize(700, 683);
    ui_->label_smile->setPixmap(QPixmap::fromImage(QImage(":/EmotionAI/Resources/boy2.png").
        scaled(51, 51, Qt::KeepAspectRatio)));
    ui_->closeDrawerBtn->hide();
    ui_->playPauseBtn->setDisabled(true);
    ui_->stopReplayBtn->setDisabled(true);
    ui_->delaySpinBox->setRange(0, 10000);
    ui_->delayLabel->setBuddy(ui_->delaySpinBox);
    ui_->delaySpinBox->setDisabled(true);
    ui_->statusBar->showMessage("No media.");

    paused_status_ = false;
    stop_replay_status_ = true;

    camera_timer_ = new QTimer(this);
    video_timer_ = new QTimer(this);
    image_timer_ = new QTimer(this);
    save_image_timer_ = new QTimer(this);

    /*��ʱ����ͼ��.*/
    save_image_timer_->setInterval(g_save_image_time_interval);
    save_image_timer_->start();
    connect(save_image_timer_, SIGNAL(timeout()), this, SLOT(SaveEmotionFrame()));

    /*Menu Bar.*/
    /*menu_1 �ļ�.*/
    connect(ui_->actionOpen_Camera, SIGNAL(triggered()), this, SLOT(OnOpenCamera()));
    connect(camera_timer_, SIGNAL(timeout()), this, SLOT(GetCameraFrame()));
    connect(ui_->actionOpen_Video_File, SIGNAL(triggered()), this, SLOT(OnOpenVideo()));
    connect(video_timer_, SIGNAL(timeout()), this, SLOT(GetVideoFrame()));
    connect(ui_->actionOpen_Image_Seq, SIGNAL(triggered()), this, SLOT(OnOpenImageSeq()));
    connect(image_timer_, SIGNAL(timeout()), this, SLOT(GetImageSeqFrame()));
    connect(ui_->actionExit, SIGNAL(triggered()), this, SLOT(close()));

    /*menu_2 ��ʾ.*/
    connect(ui_->actionDemoImage, SIGNAL(triggered()), this, SLOT(OnActionDemoImageTriggered()));
    connect(ui_->actionDemoVideo, SIGNAL(triggered()), this, SLOT(OnActionDemoVideoTriggered()));

    /*menu_3 ��ͼ.*/
    connect(ui_->actionViewLbptopHist, SIGNAL(triggered()), this, SLOT(OnActionViewLbptopHistTriggered()));

    /*menu_4 ����.*/
    connect(ui_->actionLBP_TOP, SIGNAL(triggered()), this, SLOT(OnActionLbptopSettingsTriggered()));
    connect(ui_->actionStorage, SIGNAL(triggered()), this, SLOT(OnActionStorageSettingsTriggered()));
    connect(ui_->action_EmotionAI, SIGNAL(triggered()), this, SLOT(OnActionAboutEmotionAiTriggered()));

    /*Tool Bar.*/
    /*File Bar.*/
    connect(ui_->action_open_camera, SIGNAL(triggered()), this, SLOT(OnOpenCamera()));
    connect(ui_->action_open_video_file, SIGNAL(triggered()), this, SLOT(OnOpenVideo()));
    connect(ui_->action_open_image_seq, SIGNAL(triggered()), this, SLOT(OnOpenImageSeq()));

    /*Demo Bar incling image and video.*/
    connect(ui_->action_demo_image, SIGNAL(triggered()), this, SLOT(OnActionDemoImageTriggered()));
    connect(ui_->action_demo_video, SIGNAL(triggered()), this, SLOT(OnActionDemoVideoTriggered()));

    /*��������.*/
    connect(ui_->openDrawerBtn, SIGNAL(clicked()), this, SLOT(OpenExtension()));
    connect(ui_->closeDrawerBtn, SIGNAL(clicked()), this, SLOT(CloseExtension()));

    /*���ư�ť.*/
    connect(ui_->playPauseBtn, SIGNAL(pressed()), this, SLOT(OnPlayPauseButtonPressed()));
    connect(ui_->stopReplayBtn, SIGNAL(pressed()), this, SLOT(OnStopReplayButtonPressed()));
    connect(ui_->CatchCameraBtn, SIGNAL(pressed()), this, SLOT(OnSnipFrameButtonClicked()));
    connect(ui_->delaySpinBox, SIGNAL(valueChanged(int)), this, SLOT(DelayChanged()));
}

EmotionAiMainWindow::~EmotionAiMainWindow()
{
    delete camera_timer_;
    delete video_timer_;
    delete image_timer_;
    delete save_image_timer_;

    delete demo_static_image_window_;
    delete demo_static_video_window_;
    delete lbptop_settings_window_;
    delete storage_settings_window_;
    delete ui_;
}

/*
@��ʾ1, LBP��̬ͼƬ����ʶ�����.
*/
void EmotionAiMainWindow::OnActionDemoImageTriggered()
{
    demo_static_image_window_->show();
}

/*
@��ʾ2, LBP��̬��Ƶ����ʶ��.
*/
void EmotionAiMainWindow::OnActionDemoVideoTriggered()
{
    demo_static_video_window_->show();
}

/*
@LBP-TOP�����趨.
@P, ��������; R, ���������뾶.
*/
void EmotionAiMainWindow::OnActionLbptopSettingsTriggered()
{
    lbptop_settings_window_->show();
}

/*
@�洢·�����洢ʱ��������.
*/
void EmotionAiMainWindow::OnActionStorageSettingsTriggered()
{
    storage_settings_window_->show();
}

/*
@����ϵͳ.
*/
void EmotionAiMainWindow::OnActionAboutEmotionAiTriggered()
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle(tr("About Emo - Emotion AI"));
    msgBox.setText(tr("Emotion AI"));
    msgBox.setInformativeText(tr("Version 1.0.0\n"
                                 "Emo is a system of expression recognition.\n\n"
                                 "Copyright (C) 2018 The Anaian Inc."));
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
}

/*
@�鿴������״ͼ.
*/
void EmotionAiMainWindow::OnActionViewLbptopHistTriggered()
{
    if (hist_image_.empty())
    {
        QMessageBox::warning(this, "warning", "No Feature Hist",
                             QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return;
    }
    /*CV_GUI_EXPANDED.*/
    namedWindow("histogram of Feature Image", CV_GUI_EXPANDED);
    imshow("histogram of Feature Image", hist_image_);
}

/*
@������ͷ.
*/
void EmotionAiMainWindow::OnOpenCamera()
{
    ui_->statusBar->showMessage("Camera is opening.");
    if (camera_cap_.isOpened())
    {
        camera_timer_->stop();
        save_image_timer_->stop();
        camera_cap_.release();
    }
    if (video_cap_.isOpened())
    {
        video_timer_->stop();
        save_image_timer_->stop();
        video_cap_.release();
    }
    camera_cap_.open(0);
    delay_ = 40;     /*Ĭ��ʱ��.*/

    int cols = camera_cap_.get(CV_CAP_PROP_FRAME_WIDTH);
    int rows = camera_cap_.get(CV_CAP_PROP_FRAME_HEIGHT);
    main_view_scene_->setSceneRect(0, 0, cols, rows);
    ui_->playPauseBtn->setEnabled(true);
    ui_->playPauseBtn->setText(tr("Pause"));
    ui_->stopReplayBtn->setEnabled(true);
    ui_->delaySpinBox->setValue(delay_);
    ui_->delaySpinBox->setEnabled(true);
    paused_status_ = false;
    stop_replay_status_ = true;
    media_type_ = 'c';

    ui_->statusBar->showMessage(tr("Camera Opened."));
    ui_->stopReplayBtn->setText(tr("Stop"));
    camera_timer_->setInterval(delay_);
    camera_timer_->start();
    save_image_timer_->setInterval(g_save_image_time_interval);
    save_image_timer_->start();
}

/*
@��������ͷ֡��Ϣ.
*/
void EmotionAiMainWindow::GetCameraFrame()
{
    camera_cap_ >> frame_;
    if (frame_.empty())
    {
        return;
    }

    /*��ȡ����Ȥ����, ����������.*/
    Rect roi = ft.FaceDetect(frame_);
    if (roi.area() > 0)
    {
        frame_(roi).copyTo(face_);
        cv::resize(face_, face_, Size(xSize, ySize));
        cvtColor(face_, face_, CV_BGR2GRAY);
        fb.push_back(face_);
        cv::rectangle(frame_, roi, cv::Scalar(0, 0, 255), 2);

        vector<Mat> volume = fb.peep();

        /*���������������ݣ���LBP-TOP����.*/
        sample_feature_data_ = lt.GenerateSampleData(volume);

        ShowBufferFrames(volume);
        lt.GetFullFrame(xy_, xt_, yt_);
        ShowLbpTopFrames();
    }
    q_image_ = QImage((const unsigned char*)frame_.data, frame_.cols,
                                            frame_.rows,frame_.step,
                                            QImage::Format_RGB888).rgbSwapped();
    main_view_scene_->clear();
    main_view_scene_->addPixmap(QPixmap::fromImage(q_image_));
    ui_->statusBar->showMessage(tr("Camera is running."));
}

/*
@����Ƶ�ļ�.
*/
void EmotionAiMainWindow::OnOpenVideo()
{
    file_path_ = QFileDialog::getOpenFileName(this,
        tr("Select Video"),
        QString::fromStdString(TEST_VIDEO_PATH),
        tr("Video File(*.avi *.wmv *.flv *.mp4 *.MOV *.webm *.mpg)"));

    if (file_path_.isEmpty())
    {
        return;
    }
    ui_->statusBar->showMessage(tr("Opening the video file."));
    if (camera_cap_.isOpened())
    {
        camera_timer_->stop();
        save_image_timer_->stop();
        camera_cap_.release();
    }
    if (video_cap_.isOpened())
    {
        video_timer_->stop();
        save_image_timer_->stop();
        video_cap_.release();
    }

    video_cap_.open(file_path_.toLocal8Bit().toStdString());
    fps_ = video_cap_.get(CV_CAP_PROP_FPS);
    delay_ = int(1000 * 1.0 / fps_);

    ui_->playPauseBtn->setEnabled(true);
    ui_->playPauseBtn->setText(tr("Pause"));
    ui_->stopReplayBtn->setEnabled(true);
    ui_->delaySpinBox->setValue(delay_);
    ui_->delaySpinBox->setEnabled(true);

    int cols = video_cap_.get(CV_CAP_PROP_FRAME_WIDTH);
    int rows = video_cap_.get(CV_CAP_PROP_FRAME_HEIGHT);
    main_view_scene_->setSceneRect(0, 0, cols, rows);
    paused_status_ = false;
    stop_replay_status_ = true;
    media_type_ = 'v';

    ui_->statusBar->showMessage(tr("The video file Opened."));
    ui_->stopReplayBtn->setText(tr("Stop"));

    video_timer_->setInterval(delay_);
    video_timer_->start();
    save_image_timer_->setInterval(g_save_image_time_interval);
    save_image_timer_->start();
}

/*
@������Ƶ֡.
*/
void EmotionAiMainWindow::GetVideoFrame()
{
    video_cap_ >> frame_;
    if (frame_.empty())
    {
        video_timer_->stop();
        save_image_timer_->stop();
        OnStopReplayButtonPressed();
        return;
    }

    /*����ʶ��ģ��.*/
    Rect roi = ft.FaceDetect(frame_);
    if (roi.area() > 0)
    {
        frame_(roi).copyTo(face_);
        cv::resize(face_, face_, Size(xSize, ySize));
        cvtColor(face_, face_, CV_BGR2GRAY);
        fb.push_back(face_);
        cv::rectangle(frame_, roi, cv::Scalar(0, 0, 255), 2);

        vector<Mat> volume = fb.peep();

        /*������������, ��LBP-TOP����.*/
        sample_feature_data_ = lt.GenerateSampleData(volume);

        ShowBufferFrames(volume);
        lt.GetFullFrame(xy_, xt_, yt_);
        ShowLbpTopFrames();
    }

    q_image_ = QImage((const uchar*)frame_.data, frame_.cols, frame_.rows,
        frame_.step, QImage::Format_RGB888).rgbSwapped();
    main_view_scene_->clear();
    main_view_scene_->addPixmap(QPixmap::fromImage(q_image_));
    ui_->statusBar->showMessage(tr("Video is playing."));
}

/*
@��ͼ������.
*/
void EmotionAiMainWindow::OnOpenImageSeq()
{
    QFileDialog *fd = new QFileDialog;
    QTreeView *tree = fd->findChild<QTreeView*>();
    fd->setFileMode(QFileDialog::Directory);
    fd->setWindowTitle(tr("Select Image Sequence."));
    fd->setOption(QFileDialog::ShowDirsOnly);
    fd->setViewMode(QFileDialog::Detail);

    int result = fd->exec();
    if (!result)  /*result = 0*/
    {
        return;
    }
    directory_path_ = fd->selectedFiles()[0];
    QDir dir(directory_path_);
    QStringList filters;
    filters << "*.png" << "*.jpg" << "*.bmp";
    file_info_list_ = dir.entryInfoList(filters, QDir::Files | QDir::NoDotAndDotDot);

    if (file_info_list_.empty())
    {
        QMessageBox::critical(this, "Error", "No Image File Found");
        return;
    }
    file_index_ = 0;

    if (camera_cap_.isOpened())
    {
        camera_timer_->stop();
        save_image_timer_->stop();
        camera_cap_.release();
    }
    if (video_cap_.isOpened())
    {
        video_timer_->stop();
        save_image_timer_->stop();
        video_cap_.release();
    }
    /*Ĭ����ʱ.*/
    delay_ = 250;

    ui_->delaySpinBox->setValue(delay_);
    ui_->playPauseBtn->setEnabled(true);
    ui_->playPauseBtn->setText(tr("Pause"));
    ui_->stopReplayBtn->setEnabled(true);
    paused_status_ = false;
    stop_replay_status_ = true;
    media_type_ = 'i';

    ui_->stopReplayBtn->setText(tr("Stop"));

    image_timer_->setInterval(delay_);
    image_timer_->start();

    ui_->statusBar->showMessage(tr("open the image seq."));
}

/*
@����ͼ������֡
*/
void EmotionAiMainWindow::GetImageSeqFrame()
{
    frame_ = imread(file_info_list_.at(file_index_).absoluteFilePath().toLocal8Bit().toStdString());
    main_view_scene_->setSceneRect(0, 0, frame_.cols, frame_.rows);

    Rect roi = ft.FaceDetect(frame_);
    if (roi.area() > 0)
    {
        frame_(roi).copyTo(face_);

        cv::resize(face_, face_, Size(xSize, ySize));
        cvtColor(face_, face_, CV_BGR2GRAY);
        fb.push_back(face_);
        cv::rectangle(frame_, roi, cv::Scalar(0, 0, 255), 2);
        vector<Mat> volume = fb.peep();

        /*������������, ��LBP-TOP����.*/
        sample_feature_data_ = lt.GenerateSampleData(volume);

        ShowBufferFrames(volume);
        lt.GetFullFrame(xy_, xt_, yt_);
        ShowLbpTopFrames();
    }

    q_image_ = QImage((const uchar*)frame_.data, frame_.cols, frame_.rows,
        frame_.step, QImage::Format_RGB888).rgbSwapped();
    main_view_scene_->clear();
    main_view_scene_->addPixmap(QPixmap::fromImage(q_image_));

    /*ͼ�����м������.*/
    if (file_index_ == file_info_list_.size() - 1)
    {
        ui_->statusBar->showMessage(tr("Recognizing expression, please wait."));
        int ret = (int)ec.SvmLbptopEmotion(sample_feature_data_);
        RelaxMusic(ret);

        image_timer_->stop();
        save_image_timer_->stop();
        OnStopReplayButtonPressed();

        QDateTime current_time = QDateTime::currentDateTime();
        QString current_date = current_time.toString("yyyyMMdd hh:mm:ss");
        ui_->label_emo_time->setText(tr(current_date.toLocal8Bit().toStdString().c_str()));
        ui_->label_emo->setText(tr(Lbptop_Expression_Hash_Table[ret]));

        string display_message = "The type of emotion is:\n\n";
        display_message += Lbptop_Expression_Hash_Table[ret];
        QMessageBox::information(this, "Emotion", QString::fromStdString(display_message));
    }
    else
    {
        file_index_++;
        ui_->statusBar->showMessage(tr("Image sequence is playing."));
    }
}

/*
@ץͼ.
*/
void EmotionAiMainWindow::OnSnipFrameButtonClicked()
{
    if (frame_.empty())
    {
        QMessageBox::warning(this, "warning", "No media",
                            QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return;
    }
    imshow("ץͼ", frame_);

    /*@TEST@*/
    string path  = g_snip_image_path + "//";
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("yyyyMMddhhmmss");
    path += current_date.toLocal8Bit().toStdString() + "-test.jpg";
    imwrite(path, frame_);
}

/*
@[����/��ͣ]��ť����.
*/
void EmotionAiMainWindow::OnPlayPauseButtonPressed()
{
    paused_status_ = !paused_status_;
    if (paused_status_)
    {
        switch (media_type_)
        {
        case 'c': /*camera.*/
            camera_timer_->stop();
            save_image_timer_->stop();
            break;
        case 'v': /*video.*/
            video_timer_->stop();
            save_image_timer_->stop();
            break;
        case 'i': /*image sequence.*/
            image_timer_->stop();
            save_image_timer_->stop();
            break;
        default:
            break;
        }
        mu.Stop();
        ui_->playPauseBtn->setText(tr("Play"));
        ui_->statusBar->showMessage(tr("Paused"));
    }
    else
    {
        switch (media_type_)
        {
        case 'c':   /*camera.*/
            camera_timer_->start();
            save_image_timer_->start();
            break;
        case 'v':   /*video.*/
            video_timer_->start();
            save_image_timer_->start();
            break;
        case 'i':   /*image sequence.*/
            image_timer_->start();
            break;
        default:
            break;
        }
        ui_->playPauseBtn->setText(tr("Pause"));
        ui_->statusBar->showMessage(tr("Media is playing."));
    }
}

/*
@[ֹͣ/�ز�]��ť����.
*/
void EmotionAiMainWindow::OnStopReplayButtonPressed()
{
    stop_replay_status_ = !stop_replay_status_;
    if (stop_replay_status_)
    {
        int cols, rows = 0;
        switch (media_type_)
        {
        case 'v':   /*video.*/
            video_cap_.release();
            video_cap_.open(file_path_.toLocal8Bit().toStdString());
            cols = video_cap_.get(CV_CAP_PROP_FRAME_WIDTH);
            rows = video_cap_.get(CV_CAP_PROP_FRAME_HEIGHT);
            main_view_scene_->setSceneRect(0, 0, cols, rows);
            ui_->stopReplayBtn->setText(tr("Stop"));
            video_timer_->start();
            save_image_timer_->start();
            break;
        case 'i':   /*image_sequence.*/
            file_index_ = 0;
            image_timer_->start();
            ui_->stopReplayBtn->setText(tr("Stop"));
            break;
        default:
            break;
        }
        paused_status_ = false;
        ui_->playPauseBtn->setEnabled(true);
        ui_->playPauseBtn->setText(tr("Pause"));
        ui_->statusBar->showMessage(tr("Replay"));
    }
    else
    {
        if (camera_cap_.isOpened())
        {
            save_image_timer_->stop();
            camera_timer_->stop();
            camera_cap_.release();
            ui_->stopReplayBtn->setText(tr("Stop/Replay"));
            ui_->stopReplayBtn->setDisabled(true);
        }
        else if (video_cap_.isOpened())
        {
            save_image_timer_->stop();
            video_timer_->stop();
            video_cap_.release();
            ui_->stopReplayBtn->setText(tr("Replay"));
        }
        else if (media_type_ == 'i')
        {
            save_image_timer_->stop();
            image_timer_->stop();
            ui_->stopReplayBtn->setText(tr("Replay"));
        }
        mu.Stop();
        ui_->playPauseBtn->setText(tr("Play/Pause"));
        ui_->playPauseBtn->setDisabled(true);
        ui_->delaySpinBox->setDisabled(true);
        ui_->statusBar->showMessage(tr("Stopped."));
    }
}

/*
@ʱ�ӵ�ֵ������.
*/
void EmotionAiMainWindow::DelayChanged()
{
    delay_ = ui_->delaySpinBox->value();
    switch (media_type_)
    {
    case 'c':   /*camera.*/
        camera_timer_->setInterval(delay_);
        break;
    case 'v':   /*video.*/
        video_timer_->setInterval(delay_);
        break;
    default:
        break;
    }
}

/*
@�򿪻�ͼչʾ����.
*/
void EmotionAiMainWindow::OpenExtension()
{
    ui_->openDrawerBtn->hide();
    ui_->closeDrawerBtn->show();
    resize(1273, 683);
}

/*
@�رջ�ͼչʾ����.
*/
void EmotionAiMainWindow::CloseExtension()
{
    ui_->openDrawerBtn->show();
    ui_->closeDrawerBtn->hide();
    resize(700, 683);
}

/*
@չʾ������ͼ��֡.
*/
void EmotionAiMainWindow::ShowBufferFrames(const std::vector<Mat> &volume)
{
    int buffer_size = volume.size();
    int step_len = buffer_size / 5;
    Mat m;
    QImage m_image;

    /*Just show 5 images.*/
    for (unsigned int i = 0;i < 5; ++i)
    {
        volume.at(buffer_size - i * step_len - 1).copyTo(m);
        cv::resize(m, m, Size(100, 100));
        m_image = QImage((const uchar*)m.data, m.cols, m.rows,
                m.step, QImage::Format_Indexed8).rgbSwapped();
        switch (i)
        {
        case 0:
            ui_->buffer5->setPixmap(QPixmap::fromImage(m_image));
            break;
        case 1:
            ui_->buffer4->setPixmap(QPixmap::fromImage(m_image));
            break;
        case 2:
            ui_->buffer3->setPixmap(QPixmap::fromImage(m_image));
            break;
        case 3:
            ui_->buffer2->setPixmap(QPixmap::fromImage(m_image));
            break;
        case 4:
            ui_->buffer1->setPixmap(QPixmap::fromImage(m_image));
            break;
        default:
            break;
        }
    }
}

/*
@չʾ����LBP-TOP����ͼ��.
*/
void EmotionAiMainWindow::ShowLbpTopFrames()
{
    /*X-Y.*/
    cv::resize(xy_, xy_, cv::Size(150, 150));
    frame_xy_image_ = QImage((const uchar*)xy_.data, xy_.cols, xy_.rows,
                        xy_.step, QImage::Format_Indexed8).rgbSwapped();
    ui_->frameXYView->setPixmap(QPixmap::fromImage(frame_xy_image_));

    /*X-T.*/
    cv::resize(xt_, xt_, cv::Size(150, 150));
    frame_xt_image_ = QImage((const uchar*)xt_.data, xt_.cols, xt_.rows,
                        xt_.step, QImage::Format_Indexed8).rgbSwapped();
    ui_->frameXTView->setPixmap(QPixmap::fromImage(frame_xt_image_));

    /*Y-T.*/
    cv::resize(yt_, yt_, cv::Size(150, 150));
    frame_yt_image_ = QImage((const uchar*)yt_.data, yt_.cols, yt_.rows,
                        yt_.step, QImage::Format_Indexed8).rgbSwapped();
    ui_->frameYTView->setPixmap(QPixmap::fromImage(frame_yt_image_));

    hist_image_ = lt.GetHistImage(sample_feature_data_);
    cv::resize(hist_image_, hist_image_, cv::Size(509, 170));
    q_hist_image_ = QImage((const uchar*)hist_image_.data, hist_image_.cols, 
                                         hist_image_.rows, hist_image_.step,
                                         QImage::Format_Indexed8).rgbSwapped();
    ui_->lbptopView->setPixmap(QPixmap::fromImage(q_hist_image_));
}

/*
@��ʱ�洢����֡
@TODO: ���߳�
*/
void EmotionAiMainWindow::SaveEmotionFrame()
{
    Mat will_save_frame = frame_.clone();
    result_frame_ = sample_feature_data_;
    if (will_save_frame.empty() || sample_feature_data_.empty())
    {
        return;
    }
    int ret = (int)ec.SvmLbptopEmotion(result_frame_);
    RelaxMusic(ret);
    /*
        QMessageBox::information(this, "Emotion", Lbptop_Expression_Hash_Table[ret]);
    */
    string txt = Lbptop_Expression_Hash_Table[ret];
    string path = g_save_image_path + "//";
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("yyyy.MM.dd-hh.mm.ss-ddd-");

    path += current_date.toLocal8Bit().toStdString() + txt + ".jpg";
    imwrite(path, will_save_frame);

    current_date = current_date_time.toString("yyyyMMdd hh:mm:ss");
    ui_->label_emo->setText(tr(txt.c_str()));
    ui_->label_emo_time->setText(tr(current_date.toLocal8Bit().toStdString().c_str()));
    ui_->statusBar->showMessage(tr("Succeed to save the last frame of frame sequence."));
}

/*
@������Ϊ��������ʱ, ��������; Ϊ��������ʱ, ֹͣ��������.
*/
void EmotionAiMainWindow::RelaxMusic(const int emotion_label)
{
    string txt = Lbptop_Expression_Hash_Table[emotion_label];
    if (txt == "anger" || txt == "contempt" || txt == "disgust" || txt == "fear" || txt == "sadness")
    {
        //down_emotion++;
        ui_->label_smile->setPixmap(QPixmap::fromImage(QImage(":/EmotionAI/Resources/boy1.png").
            scaled(51, 51, Qt::KeepAspectRatio)));
        mu.Play();
    }
    else if (txt == "happy" || txt == "surprise")
    {
        ui_->label_smile->setPixmap(QPixmap::fromImage(QImage(":/EmotionAI/Resources/boy2.png").
            scaled(51, 51, Qt::KeepAspectRatio)));
        mu.Stop();
    }
    else
    {
    }
}