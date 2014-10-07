#ifndef TRACKING_H
#define TRACKING_H

#include "gngt.h"

enum Display {
    MESH,
    PALM
};

class Tracker {
public:
    Tracker(const std::string &param);
    ~Tracker();

    template<class Iterator>
    void newFrame(Iterator begin, Iterator end){
        int nb_sample = m_sample_percent*(end-begin);
        if(nb_sample > m_min_nb_sample){
            for(int i=0 ; i<m_nb_epoch ; ++i){
                std::random_shuffle(begin, end);
                //std::cout << "new epoch - " << i << std::endl;
                m_mesh->epoch(begin, begin+nb_sample);
            }
            updateFeatures();
            m_to_draw = true;
        } else {
            m_to_draw = false;
        }

    }

    void draw(cv::Mat &img, Display mode){
        if(m_to_draw)
            m_mesh->draw(img);
    }

    std::pair<float, float> getPalmCenter(){
        return m_palm_center;
    }

    int getNbFinger(){
        return m_nb_finger;
    }

private:
    void updateFeatures();

    // Features tracked by the tracker
    std::pair<float,float> m_palm_center;
    int m_nb_finger;

    // GNG_T graph
    Gngt* m_mesh;

    // Parameters
    int m_nb_epoch;
    float m_sample_percent;
    int m_min_nb_sample;

    // Other
    bool m_to_draw;
};

#endif // TRACKING_H
