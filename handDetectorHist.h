#ifndef HAND_DETECTOR_HIST_H
#define HAND_DETECTOR_HIST_H

#include <opencv2/opencv.hpp>

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

class HandDetectorHist
{
public:
    HandDetectorHist(const std::string &windowName){
        cv::setMouseCallback(windowName.c_str(), onMouse, &r);
    }

    ~HandDetectorHist(void){}

    cv::Mat findHand(cv::Mat &input)
    {
        cv::GaussianBlur( input, input, cv::Size(15,15), 0, 0, cv::BORDER_DEFAULT );

        // histogram parameters
        int Crbins = 128, Cbbins = 128;
        int histSize[] = {Crbins, Cbbins};
        float Crranges[] = { 0, 256 };
        float Cbranges[] = { 0, 256 };
        const float* ranges[] = { Crranges, Cbranges };
        int channels[] = {1, 2};
        // --------------------

        cv::Mat hand(input.size(), CV_8UC1, cv::Scalar(0));

        if(!r.isSelecting){
            cv::Mat YCrCb;
            cv::cvtColor(input, YCrCb, CV_BGR2YCrCb);

            if(!r.stopSelecting){
                if(r.hist != nullptr){

                    cv::calcBackProject(&YCrCb, 1, channels, *(r.hist), hand, ranges);

                    cv::inRange(hand,cv::Scalar(200),cv::Scalar(256),hand);

                    int ro = 1;
                    cv::Mat elementOpen =
                            cv::getStructuringElement(cv::MORPH_ELLIPSE,
                                                      cv::Size(2*ro+1, 2*ro+1),
                                                      cv::Point(ro, ro) );
                    cv::morphologyEx(hand, hand, cv::MORPH_OPEN, elementOpen);

                    int rc = 6;
                    cv::Mat elementClose =
                            cv::getStructuringElement(cv::MORPH_ELLIPSE,
                                                      cv::Size(2*rc+1, 2*rc+1),
                                                      cv::Point(rc, rc) );
                    cv::morphologyEx(hand, hand, cv::MORPH_CLOSE, elementClose);

                    int rd = 4;
                    cv::Mat elementDilate =
                            cv::getStructuringElement(cv::MORPH_ELLIPSE,
                                                      cv::Size(2*rd+1, 2*rd+1),
                                                      cv::Point(rd, rd) );
                    cv::morphologyEx(hand, hand, cv::MORPH_DILATE, elementDilate);

                    std::vector<std::vector<cv::Point> > contours;
                    std::vector<cv::Vec4i> hierarchy;
                    cv::findContours( hand, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE );

                    hand = cv::Scalar(0);

                    cv::drawContours(hand, contours, -1, cv::Scalar(255), -1);

                    cv::morphologyEx(hand, hand, cv::MORPH_ERODE, elementDilate);

//                    // -- Remove the face --
//                    cv::Mat grayscaleFrame;
//                    cv::cvtColor(input, grayscaleFrame, CV_BGR2GRAY);
//                    cv::equalizeHist(grayscaleFrame, grayscaleFrame);

//                    cv::CascadeClassifier face_cascade;
//                    face_cascade.load("haarcascade_frontalface_alt.xml");

//                    //create a vector array to store the face found
//                    std::vector<cv::Rect> faces;

//                    //find faces and store them in the vector array
//                    face_cascade.detectMultiScale(grayscaleFrame, faces, 1.1, 3, /*CV_HAAR_FIND_BIGGEST_OBJECT|*/CV_HAAR_SCALE_IMAGE, cv::Size(30,30));

//                    //draw a rectangle for all found faces in the vector array on the original image
//                    for(int i = 0; i < faces.size(); i++)
//                    {
//                        cv::Point pt1(faces[i].x + faces[i].width+10, faces[i].y + faces[i].height+150);
//                        cv::Point pt2(faces[i].x -7 , faces[i].y - 50 );
//                        cv::rectangle(hand, pt1, pt2, cv::Scalar(0), CV_FILLED, 8, 0);
//                    }
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
            cv::rectangle(input, r.down, r.up, cv::Scalar(255, 0, 0));
        }

        return hand;
    }

private:
    Region r;
};

#endif // HAND_DETECTOR_HIST_H

