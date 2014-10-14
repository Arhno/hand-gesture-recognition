#include <opencv2/opencv.hpp>
#include <sstream>
#include <fstream>
#include "tracking.h"
#include"handDetectorHist2.h"


int main(int argc, char* argv[]){
    if(argc < 2){
        std::cout << "usage: " << argv[0] << " <folder>" << std::endl;
        exit(0);
    }

    std::string root(argv[1]);

    int image_nb = 0;

    std::vector<std::pair<float, float>> samples;

    cv::Mat img, binimg ;
    cv::namedWindow("track");
    cv::namedWindow("hand");

    std::stringstream logName;
    logName << root << "/" << "features" << ".csv" ;

    std::ofstream myfile;
    myfile.open(logName.str().c_str(),std::ios_base::app);

    HandDetectorHist2 myHandDetector("track");

    int c;
    do{
        Tracker tracker("param.txt");

        std::stringstream imgName;
        imgName << root << "/" << image_nb++ << ".png" ;
        int done_for = 0;
        do {

            img = cv::imread(imgName.str()) ;

            if(img.data == NULL){
                break;
            }

            binimg = myHandDetector.findHand(img);

            samples.clear();

            for(int i=0 ; i<binimg.rows ; ++i){
                for(int j=0 ; j<binimg.cols ; ++j){
                    if(binimg.at<unsigned char>(i,j) > 128)
                    {
                        samples.push_back(std::make_pair(float(j),float(i)));
                    }
                }
            }
            if(tracker.newFrame(samples.begin(), samples.end())){
                ++done_for;
            }
            tracker.draw(img, Display::MESH);

            cv::imshow("track", img);

            cv::imshow("hand", binimg);
            c = cv::waitKey(5);
        } while(done_for < 5 && c!='n');


        if(img.data == NULL){
            break;
        }

        std::vector<float> x = tracker.retvalue();
        std::vector<float> y = tracker.retvalue1();

        for (int d=0; d<x.size(); d++)
        {
            myfile<<x[d]<<","<<y[d]<<",";
        }
        myfile<<tracker.getRatio() ;
        myfile<<"\n";

        c = cv::waitKey(0);
    }while(c != 'q');

    myfile.close();

    return 0;
}
