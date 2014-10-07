#include "tracking.h"

#include <fstream>

Tracker::Tracker(const std::string &param)
    : m_nb_finger(0),
      m_mesh(nullptr),
      m_nb_epoch(10),
      m_sample_percent(0.02),
      m_min_nb_sample(200),
      m_to_draw(false)
{
    float target, fastRate, slowRate;
    int ageMax;
    std::ifstream parameters(param.c_str());
    parameters >> m_nb_epoch;
    parameters >> m_sample_percent;
    parameters >> m_min_nb_sample;

    parameters >> target;
    parameters >> fastRate;
    parameters >> slowRate;
    parameters >> ageMax;
    parameters.close();

    m_mesh = new Gngt(target, fastRate, slowRate, ageMax);
}

Tracker::~Tracker(){
    delete m_mesh;
}

void Tracker::updateFeatures(){
    m_nb_finger = 0;
    m_palm_center.first = m_palm_center.second = 0;
}
