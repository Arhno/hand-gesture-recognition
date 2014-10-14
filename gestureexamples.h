#ifndef GESTUREEXAMPLES_H
#define GESTUREEXAMPLES_H

#include "gesture.h"
#include <vector>

class GestureExamples
{
public:
    GestureExamples(const std::string &fileName);
    GestureExamples(const GestureExamples &ge);

    float score(const Gesture &g);

    std::string name() {return name_;}
private:
    std::vector<Gesture> examples;
    std::string name_;
};

#endif // GESTUREEXAMPLES_H
