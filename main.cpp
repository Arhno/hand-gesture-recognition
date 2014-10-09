#include <iostream>
#include <stdio.h>
#include <vector>
#include <utility>
#include <algorithm>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "tracking.h"
#include"handDetector.h"
#include"handDetectorHist.h"

int main(int argc, char* argv[]){

    std::string paramFileName = "param.conf";
    if(argc > 1){
        paramFileName = std::string(argv[1]);
    }

    cv::namedWindow("track", CV_WINDOW_NORMAL);
    cv::namedWindow("bin", CV_WINDOW_NORMAL);
    cv::VideoCapture vc(0);
    int h = 600;
    int w = 600;

    HandDetectorHist myHandDetector("track");
    //HandDetector myHandDetector;
    Tracker tracker(paramFileName);

    std::vector<std::pair<float, float>> samples;
    cv::Mat img, subImg, binimg;
    while(cv::waitKey(5) != 'q'){
        vc >> img ;
        subImg = img(cv::Range(0,h), cv::Range(0,w));
        cv::flip(subImg, subImg, 1);

        binimg = myHandDetector.findHand(subImg);

        samples.clear();

        for(int i=0 ; i<binimg.rows ; ++i){
            for(int j=0 ; j<binimg.cols ; ++j){
                if(binimg.at<unsigned char>(i,j) > 128)
                {
                    samples.push_back(std::make_pair(float(j),float(i)));
                }
            }
        }
        tracker.newFrame(samples.begin(), samples.end());
        tracker.draw(subImg, Display::MESH);

        cv::imshow("track", subImg);
        cv::imshow("skin", binimg);
    }

    return 0;
}
