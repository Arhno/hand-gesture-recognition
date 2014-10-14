#ifndef GESTURE_H
#define GESTURE_H

#include <string>
#include <vector>

class Gesture
{
public:
    Gesture(const std::string& line);
    Gesture(const Gesture& g);

    float distFrom(const Gesture& g);
    static void extract(const std::string &fileName, std::vector<Gesture> &v);
private:
    std::vector<float> angles;
    std::vector<float> distances;
    float ratio;
};

#endif // GESTURE_H
