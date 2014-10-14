#include "gesture.h"
#include <limits>
#include <string>
#include <fstream>
#include <iostream>

Gesture::Gesture(const std::string& line)
    : angles(0),
      distances(0),
      ratio(0)
{
    int found = 0;
    bool angle = true;
    while(true){
        int temp = line.find(',', found);
        if(temp == std::string::npos){
            std::string val = line.substr(found);
            ratio = atof(val.c_str());
            break;
        } else {
            std::string val = line.substr(found, temp-found);
            if(angle){
                angles.push_back(atof(val.c_str()));
            }else{
                distances.push_back(atof(val.c_str()));
            }
        }
        found = temp+1;
        angle = !angle;
    }
}

Gesture::Gesture(const Gesture &g)
    : angles(g.angles),
      distances(g.distances),
      ratio(g.ratio)
{}

float Gesture::distFrom(const Gesture &g){
    // Compare only if the same number of fingers has been detected
    if(g.angles.size() != angles.size()){
        return std::numeric_limits<float>::max();
    }

    // ******
    // TODO compute and return the distance using the angles, the distances and the ratio
    // I think it might be a good idea to normalize the distance by divising by the number of finger + 1
    // so that gestures with a lot of finger do not suffer of handicap

    return 0;

    // ******
}

void Gesture::extract(const std::string &fileName, std::vector<Gesture> &v)
{
    v.clear();

    std::ifstream file(fileName.c_str());

    std::string line;
    while (std::getline(file, line)){
        if(!line.empty())
            v.push_back(Gesture(line));
    }
}
