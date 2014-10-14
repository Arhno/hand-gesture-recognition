#include "gestureexamples.h"
#include <string>
#include <limits>

GestureExamples::GestureExamples(const std::string &fileName)
{
    Gesture::extract(fileName, examples);
    int slash = fileName.find_last_of("/\\");
    int point = fileName.find_last_of(".");
    if(slash == std::string::npos)
        slash = -1;
    name_ = fileName.substr(slash+1, point-slash-1);
}

GestureExamples::GestureExamples(const GestureExamples &ge)
    : examples(ge.examples),
      name_(ge.name_)
{}

float GestureExamples::score(const Gesture &g)
{
    float bestScore = std::numeric_limits<float>::max();
    for(int i=0 ; i<examples.size() ; ++i){
        float score = examples[i].distFrom(g);
        if(score < bestScore)
            bestScore = score;
    }

    return bestScore;
}


