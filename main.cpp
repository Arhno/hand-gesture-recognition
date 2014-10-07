#include <vector>
#include <utility>
#include <algorithm>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "tracking.h"
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include"SkinDetector.h"
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;


struct Region {
    bool isSelecting;
    cv::Point down;
    cv::Point up;
    bool stopSelecting;
    cv::Mat *hist;

    Region()
        : isSelecting(false),
          stopSelecting(false),
          hist(nullptr)
    {}

    ~Region(){
        delete hist;
    }
};

void onMouse(int event, int x, int y, int flags, void* userdata){
    Region* r = (Region*) userdata;
    if(r->isSelecting){
        r->up = cv::Point(x,y);
        if(event == cv::EVENT_LBUTTONUP){
            r->isSelecting = false;
            r->stopSelecting = true;
        }
    } else {
        if(event == cv::EVENT_LBUTTONDOWN){
            r->isSelecting = true;
            r->stopSelecting = false;
            r->down = cv::Point(x,y);
            r->up = cv::Point(x,y);
        }
    }
}

void targetCallback(int newT, void* userdata){
    ((Gngt*) userdata)->changeTarget(newT);
}

int main(int argc, char* argv[]){
  //create the cascade classifier object used for the face detection
	CascadeClassifier face_cascade;
	//use the haarcascade_frontalface_alt.xml library
	face_cascade.load("haarcascade_frontalface_alt.xml");

    std::string paramFileName = "param.conf";
    if(argc > 1){
        paramFileName = std::string(argv[1]);
    }

    Tracker tracker(paramFileName);

    cv::namedWindow("track", CV_WINDOW_NORMAL);

    cv::namedWindow("bin", CV_WINDOW_NORMAL);


    cv::VideoCapture vc(0);

    int h = 600;
    int w = 800;

    cv::Mat img;
    vc >> img ;
    cv::Mat subImg = img(cv::Range(0,h), cv::Range(0,w));
    cv::flip(subImg, subImg, 1);
    cv::Mat binimg(subImg.size(), CV_8UC1, cv::Scalar(0));
    Mat grayscaleFrame;

// Region r;
//  cv::setMouseCallback("track", onMouse, &r);

    std::vector<std::pair<float, float>> samples;

    // histogram parameters
    int Crbins = 128, Cbbins = 128;
    int histSize[] = {Crbins, Cbbins};
    float Crranges[] = { 0, 256 };
    float Cbranges[] = { 0, 256 };
    const float* ranges[] = { Crranges, Cbranges };
    int channels[] = {1, 2};
    // --------------------

    while(cv::waitKey(5) != 'q'){
        vc >> img ;
        subImg = img(cv::Range(0,h), cv::Range(0,w));
        cv::flip(subImg, subImg, 1);
        binimg = cv::Mat(subImg.size(), CV_8UC1, cv::Scalar(0));

	cvtColor(subImg, grayscaleFrame, CV_BGR2GRAY);
		equalizeHist(grayscaleFrame, grayscaleFrame);

		//create a vector array to store the face found
		std::vector<Rect> faces;

		//find faces and store them in the vector array
		face_cascade.detectMultiScale(grayscaleFrame, faces, 1.1, 3, CV_HAAR_FIND_BIGGEST_OBJECT|CV_HAAR_SCALE_IMAGE, Size(30,30));

		//draw a rectangle for all found faces in the vector array on the original image
		for(int i = 0; i < faces.size(); i++)
		{
			Point pt1(faces[i].x + faces[i].width+10, faces[i].y + faces[i].height+150);
			Point pt2(faces[i].x -7 , faces[i].y - 7 );
			rectangle(subImg, pt1, pt2, cvScalar(255, 0, 0, 0), CV_FILLED, 8, 0);
		}
		
		imshow("adsasdas",subImg);
		SkinDetector mySkinDetector;

		Mat skinMat;
		skinMat= mySkinDetector.getSkin(subImg);
		

		//cv::Mat YCrCb;
		//cv::cvtColor(subImg, YCrCb, CV_BGR2YCrCb);
	samples.clear();


	for(int i=0 ; i<skinMat.rows ; ++i){
	  for(int j=0 ; j<skinMat.cols ; ++j){
                            //std::cout << (int) YCrCb.at<cv::Vec3b>(i,j)[1] << " " << (int) YCrCb.at<cv::Vec3b>(i,j)[2] << std::endl ;
                            int prob = (int) skinMat.at<unsigned char>(i,j);
                            if(prob>250)
                            {
                                samples.push_back(std::make_pair(float(j),float(i)));
				binimg.at<unsigned char>(i,j) = 255;
                            }
                        }
	}
	cv::imshow("skin", binimg);
	  tracker.newFrame(samples.begin(), samples.end());
          tracker.draw(subImg, Display::MESH);
	/*
        if(!r.isSelecting){
            cv::Mat YCrCb;
            cv::cvtColor(subImg, YCrCb, CV_BGR2YCrCb);

            if(!r.stopSelecting){
                if(r.hist != nullptr){
                    samples.clear();

                    cv::calcBackProject(&YCrCb, 1, channels, *(r.hist), binimg,
                                        ranges);

                    for(int i=0 ; i<binimg.rows ; ++i){
                        for(int j=0 ; j<binimg.cols ; ++j){
                            //std::cout << (int) YCrCb.at<cv::Vec3b>(i,j)[1] << " " << (int) YCrCb.at<cv::Vec3b>(i,j)[2] << std::endl ;
                            int prob = (int) binimg.at<unsigned char>(i,j);
                            if(prob>200)
                            {
                                samples.push_back(std::make_pair(float(j),float(i)));
                            }
                        }
                    }

                    tracker.newFrame(samples.begin(), samples.end());
                    tracker.draw(subImg, Display::MESH);
                }
            } else {
                r.stopSelecting = false;
                // compute histogram
                if(r.hist == nullptr){
                    r.hist = new cv::Mat();
                }
                cv::Mat patch = YCrCb(cv::Rect(r.up, r.down));
                cv::calcHist( &patch, 1, channels, cv::Mat(), *(r.hist),
                              2, histSize, ranges, true, false );
            }
        } else {
            cv::rectangle(subImg, r.down, r.up, cv::Scalar(255, 0, 0));
        }
	*/
        cv::imshow("track", subImg);
        //cv::imshow("bin", binimg);
    }

    return 0;
}
