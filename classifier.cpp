#include <opencv2/opencv.hpp>
#include <sstream>
#include <vector>
#include <fstream>
#include <limits>
#include "gesture.h"
#include "gestureexamples.h"

int main(int argc, char* argv[]){
    if(argc < 3){
        std::cout << "Not enough arguments" << std::endl;
        exit(0);
    }

    // Retrieve the list of all gestures we want to classify.
    // Each gesture is one line in the csv file (same format as for the training sample)
    std::vector<Gesture> gesturesToClassify;
    Gesture::extract(std::string(argv[1]), gesturesToClassify);

    // Retrieve the list of known gesture
    // Each known gesture stores all the examples of this gesture in the training set)
    std::vector<GestureExamples> knownGestures;
    knownGestures.reserve(argc-1);
    for(int i=2; i<argc ; ++i){
        knownGestures.push_back(GestureExamples(std::string(argv[i])));
    }

    // Classify the gesture by Neirest neighboor method
    for(int i=0 ; i<gesturesToClassify.size() ; ++i){
        float minScore = std::numeric_limits<float>::max();
        int bestClass = 0 ;
        for(int j=0 ; j<knownGestures.size() ; ++j){
            float score = knownGestures[j].score(gesturesToClassify[i]);
            if(score < minScore){
                minScore = score;
                bestClass = j;
            }
        }

        std::cout << "Gesture " << i << " is " << knownGestures[bestClass].name() << std::endl;
    }

    return 0;
}

