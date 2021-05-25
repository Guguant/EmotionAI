#ifndef GLOBAL_H
#define GLOBAL_H

#include "face_buffer.h"
#include "face_tracker.h"
#include "lbptop.h"
#include "music.h"
#include "uniform_pattern.h"


#define CascadeClassifier_Model  "TrainModel\\haarcascade_frontalface_default.xml"
#define Uniform_Pattern_File_Dir "TrainModel\\patterns\\"

/*SAVE �洢ͼ���ļ�Ŀ¼.*/
#define PRE_S ""
#define SAVE_IMAGE_TIME_INTERVAL 10000
#define SAVE_IMAGE_PATH PRE_S"EmotionImage"
#define SNIP_IMAGE_PATH PRE_S"EmotionImage\\snip"

/*TEST ����ͼ���ļ�Ŀ¼.*/
#define TEST_VIDEO_PATH PRE_S"TestFile\\video\\"
#define TEST_IMAGE_PATH PRE_S"TestFile\\image\\"

extern FaceTracker ft;
extern FaceBuffer fb;
extern LbpTop lt;
extern Music mu;

extern int g_save_image_time_interval;
extern string g_save_image_path;
extern string g_snip_image_path;

/*
@LBP-TOP.
@g_P, ����������; g_R, �����뾶.
*/
extern int g_Pxy;
extern int g_Pxt;
extern int g_Pyt;
extern int g_Rx;
extern int g_Ry;
extern int g_Rt;

const unsigned int xSize = 280;           /*��ȡ����ͼ��󣬽�ͼ����������xSize.*/
const unsigned int ySize = 280;           /*��ȡ����ͼ��󣬽�ͼ��߶�������ySize.*/
const unsigned int xBlocks = 14;          /*����ͼ����X�᷽�򻮷ֵĿ���.*/
const unsigned int yBlocks = 14;          /*����ͼ����Y�᷽�򻮷ֵĿ���.*/
const unsigned int BUFFER_MAX_SIZE = 10;  /*ͼ�񻺳������洢��ͼ������.*/

/*
@kDefaultP: Ĭ�ϲ���������; kDefaultR: Ĭ�ϲ����뾶.
*/
const int kDefaultPxy = 8;
const int kDefaultPxt = 8;
const int kDefaultPyt = 8;
const int kDefaultRx = 3;
const int kDefaultRy = 3;
const int kDefaultRt = 3;

/*LBP-TOP�������-�������ת����.*/
static char* Lbptop_Expression_Hash_Table[9] =
{
    "unknown", "anger", "contempt", "disgust", "fear", "happy", "sadness", "surprise"
};
#endif  /*GLOBAL_H*/