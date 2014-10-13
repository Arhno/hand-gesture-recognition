#include <opencv2/opencv.hpp>
#include <sstream>

int main(int argc, char* argv[]){
    cv::namedWindow("img", CV_WINDOW_NORMAL);
    cv::VideoCapture vc(0);
    int h = 600;
    int w = 600;

    if(argc < 2){
        std::cout << "usage: " << argv[0] << " <folder>" << std::endl;
        exit(0);
    }

    std::string root(argv[1]);

    int image_nb = 0;

    cv::Mat img, subImg;
    int c = cv::waitKey(5);
    while(c != 'q'){
        vc >> img ;
        subImg = img(cv::Range(0,h), cv::Range(0,w));
        cv::flip(subImg, subImg, 1);
        cv::imshow("img", subImg);
        c = cv::waitKey(5);
        if(c == 's'){
            std::stringstream imgName;
            imgName << root << "/" << image_nb++ << ".png" ;
            cv::imwrite(imgName.str(), subImg);
        }
    }

    return 0;
}
