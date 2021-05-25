#include "hist.h"

/*
@��ȡ�Ҷ�ͼ��ֱ��ͼ�ֲ�.
@���: gray_image, ԭʼ�Ҷ�ͼ��.
@����: hist_image, ͼ������ֵ�ֲ�����.
@����ֵ: ��.
*/
void GetHistValue(const Mat & gray_image, Mat & hist_image)
{
    if (gray_image.empty())
    {
        return;
    }

    const float range[] = { 0, 256 };
    const float* kHistRange = { range };
    const int kHistSize = 256;
    const bool kUniform = true;
    const bool kAccumulate = false;
    calcHist(&gray_image, 1, 0, Mat(), hist_image, 1, &kHistSize, 
                            &kHistRange, kUniform, kAccumulate);
}