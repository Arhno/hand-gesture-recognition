//SkinDetector.h

#pragma once

#include <opencv/cv.h>

class HandDetector
{
public:

    int Y_MIN;
    int Y_MAX;
    int Cr_MIN;
    int Cr_MAX;
    int Cb_MIN;
    int Cb_MAX;

    HandDetector(void){
        Y_MIN  = 0;
        Y_MAX  = 185;
        Cr_MIN = 131;
        Cr_MAX = 185;
        Cb_MIN = 80;
        Cb_MAX = 135;
    }

    ~HandDetector(void){}

    cv::Mat findHand(cv::Mat input)
    {
        cv::Mat hand;

        // -- Find skin pixel --
        cv::cvtColor(input,hand,cv::COLOR_BGR2YCrCb);
        cv::inRange(hand,cv::Scalar(Y_MIN,Cr_MIN,Cb_MIN),cv::Scalar(Y_MAX,Cr_MAX,Cb_MAX),hand);

        // -- Remove the face --
        cv::Mat grayscaleFrame;
        cv::cvtColor(input, grayscaleFrame, CV_BGR2GRAY);
        cv::equalizeHist(grayscaleFrame, grayscaleFrame);

        cv::CascadeClassifier face_cascade;
        face_cascade.load("haarcascade_frontalface_alt.xml");

        //create a vector array to store the face found
        std::vector<cv::Rect> faces;

        //find faces and store them in the vector array
        face_cascade.detectMultiScale(grayscaleFrame, faces, 1.1, 3, /*CV_HAAR_FIND_BIGGEST_OBJECT|*/CV_HAAR_SCALE_IMAGE, cv::Size(30,30));

        //draw a rectangle for all found faces in the vector array on the original image
        for(int i = 0; i < faces.size(); i++)
        {
            cv::Point pt1(faces[i].x + faces[i].width+10, faces[i].y + faces[i].height+150);
            cv::Point pt2(faces[i].x -7 , faces[i].y - 7 );
            cv::rectangle(hand, pt1, pt2, cv::Scalar(0), CV_FILLED, 8, 0);
        }

        return hand;
    }

};

// end of SkinDetector.h file

