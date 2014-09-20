#include <vector>
#include <utility>
#include <algorithm>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "gngt.h"

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

    int target = 10;
    float alpha1 = 0.05;
    float alpha2 = 0.0075;
    int age = 30;

    if(argc > 4){
        target = atof(argv[1]);
        alpha1 = atof(argv[2]);
        alpha2 = atof(argv[3]);
        age = atoi(argv[4]);
    }

    Gngt mesh(target, alpha1, alpha2, age);

    int nb_sample = 5000;
    int nb_epoch = 2;

    cv::namedWindow("Parameters", CV_WINDOW_NORMAL);
    cv::createTrackbar("Target", "Parameters", &target, 200, targetCallback, &mesh);
    cv::createTrackbar("Number of epochs", "Parameters", &nb_epoch, 10);
    cv::createTrackbar("Number of samples per epoch", "Parameters", &nb_sample, 10000);

    cv::namedWindow("track", CV_WINDOW_NORMAL);

    cv::namedWindow("bin", CV_WINDOW_NORMAL);


    cv::VideoCapture vc(0);

    int h = 600;
    int w = 600;

    cv::Mat img;
    vc >> img ;
    cv::Mat subImg = img(cv::Range(0,h), cv::Range(0,w));
    cv::flip(subImg, subImg, 1);
    cv::Mat binimg(subImg.size(), CV_8UC1, cv::Scalar(0));

    Region r;
    cv::setMouseCallback("track", onMouse, &r);

    std::vector<std::pair<float, float>> samples;

    // histogram parameters
    int Crbins = 32, Cbbins = 32;
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

        if(!r.isSelecting){
            cv::Mat YCrCb;
            cv::cvtColor(subImg, YCrCb, CV_BGR2YCrCb);

            if(!r.stopSelecting){
                if(r.hist != nullptr){
                    samples.clear();
//                    for(int i=0 ; i<YCrCb.rows ; ++i){
//                        for(int j=0 ; j<YCrCb.cols ; ++j){
//                            //std::cout << (int) YCrCb.at<cv::Vec3b>(i,j)[1] << " " << (int) YCrCb.at<cv::Vec3b>(i,j)[2] << std::endl ;
//                            int Y = (int) YCrCb.at<cv::Vec3b>(i,j)[0];
//                            if(Y < minY || Y > maxY)
//                                continue;
//                            int Cr = (int) YCrCb.at<cv::Vec3b>(i,j)[1];
//                            int Cb = (int) YCrCb.at<cv::Vec3b>(i,j)[2];
//                            if(Cr <= maxCr &&
//                               Cr >= minCr &&
//                               Cb <= maxCb &&
//                               Cb >= minCb)
//                            {
//                                samples.push_back(std::make_pair(float(j),float(i)));
//                                binimg.at<unsigned char>(i,j) = 0 ;
//                            }
//                        }
//                    }

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

                    nb_sample = samples.size()/50 ;
                    std::cout << nb_sample << std::endl ;
                    //if(samples.size() > nb_sample){
                    if(nb_sample > 200){
                        for(int i=0 ; i<nb_epoch ; ++i){
                            std::random_shuffle(samples.begin(), samples.end());
                            std::cout << "new epoch - " << i << std::endl;
                            mesh.epoch(samples.begin(), samples.begin()+nb_sample);
                        }
                        mesh.draw(subImg);
                    }
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

        cv::imshow("track", subImg);
        cv::imshow("bin", binimg);
    }

    return 0;
}
