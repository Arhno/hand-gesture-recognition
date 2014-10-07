//SkinDetector.h


#pragma once
#include<opencv/cv.h>
using namespace std;
class SkinDetector
{
public:

int Y_MIN;
int Y_MAX;
int Cr_MIN;
int Cr_MAX;
int Cb_MIN;
int Cb_MAX;

  SkinDetector(void){
Y_MIN  = 0;
    Y_MAX  = 185;
    Cr_MIN = 131;
    Cr_MAX = 185;
    Cb_MIN = 80;
    Cb_MAX = 135;
};
  ~SkinDetector(void){};

  cv::Mat getSkin(cv::Mat input)
    {
      cv::Mat skin;
      cv::cvtColor(input,skin,cv::COLOR_BGR2YCrCb);
      cv::inRange(skin,cv::Scalar(Y_MIN,Cr_MIN,Cb_MIN),cv::Scalar(Y_MAX,Cr_MAX,Cb_MAX),skin);
      return skin;
};

};

// end of SkinDetector.h file

