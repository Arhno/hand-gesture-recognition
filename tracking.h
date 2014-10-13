#ifndef TRACKING_H
#define TRACKING_H

#include <unordered_map>

#include "gngt.h"

enum Display {
    MESH,
    PALM
};

class Tracker {
public:
    std::vector<float> retvalue();
    std::vector<float> retvalue1();
    Tracker(const std::string &param);
    ~Tracker();

    // return true if the mesh is frozen
    template<class Iterator>
    bool newFrame(Iterator begin, Iterator end){
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

        return m_mesh->is_frozen();
    }

    void draw(cv::Mat &img, Display mode);

    std::pair<float, float> getPalmCenter(){
        return m_palm_center;
    }

    int getNbFinger(){
        return m_nb_finger;
    }

private:
    void updateFeatures();

    void findBiggestConectedComponent();
    void findFingerNodes();
    void countFingers();
    void findPalmCenter();
    void computeStds();
    void normalizeFingerCenters();
    void computeAngles();
    void computeDistances();
    // Features tracked by the tracker
    std::pair<float,float> m_palm_center;
    std::pair<float,float> m_palm_stds;
    int m_nb_finger;
    std::vector<std::pair<float,float>> m_finger_center;
    std::vector<std::pair<float,float>> m_finger_center_normalized;
    std::vector<float> fingerDistances;
    std::vector<float> fingerAngles;


    // GNG_T graph
    Gngt* m_mesh;
    std::unordered_map<Gngt::vertex_descriptor, int> connected_components_map;
    int m_biggest_comp;

    // Parameters
    int m_nb_epoch;
    float m_sample_percent;
    int m_min_nb_sample;

    // Other
    bool m_to_draw;
};

#endif // TRACKING_H
