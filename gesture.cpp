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

    float sum=0;
    
    for(int i=0;i<g.angles.size();i++){
        sum = sum + fabs(g.angles[i] - angles[i]) + fabs(distances[i] - distances[i]);
    }
    sum = sum + fabs(g.ratio - ratio);
    sum = sum/(g.angles.size()+1);
    return sum;

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
