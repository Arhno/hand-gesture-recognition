#include "tracking.h"

#include <fstream>
#include <boost/graph/connected_components.hpp>

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
    // We compute the connected componenents and find the one with the largest number of nodes
    findBiggestConectedComponent();

    // We identify the finger nodes
    findFingerNodes();

    // Now we can compute the features
    findPalmCenter();
    countFingers();
}

void Tracker::findBiggestConectedComponent(){
    Gngt::Graph &g = m_mesh->getGraph();

    connected_components_map.clear();
    boost::associative_property_map< std::unordered_map<Gngt::vertex_descriptor, int> >
        connected_components(connected_components_map);

    std::unordered_map<Gngt::vertex_descriptor, boost::default_color_type> cm;
    boost::associative_property_map< std::unordered_map<Gngt::vertex_descriptor, boost::default_color_type> >
        colorMap(cm);

    int num = boost::connected_components(g, connected_components, color_map(colorMap));

    std::vector<int> nb_elements(num, 0);

    std::pair<Gngt::vertex_iter, Gngt::vertex_iter> vp;
    for (vp = boost::vertices(g); vp.first != vp.second; ++vp.first){
        ++nb_elements[connected_components_map[*vp.first]];
    }

    //std::cout << "nb_components: " << num << std::endl;
    m_biggest_comp = 0;
    int maxNb = 0;
    for(int i=0 ; i<num ; ++i){
        if(nb_elements[i] > maxNb){
            maxNb = nb_elements[i];
            m_biggest_comp = i;
        }
    }
}

void Tracker::findFingerNodes(){
    Gngt::Graph &g = m_mesh->getGraph();
    // Retrieve the property map used in the method
    Gngt::finger_map_t finger_map = boost::get(vertex_finger, g);

    std::pair<Gngt::vertex_iter, Gngt::vertex_iter> vp;
    for (vp = boost::vertices(g); vp.first != vp.second; ++vp.first){
        if(connected_components_map[*vp.first] == m_biggest_comp){
            finger_map[*vp.first] = false;
            if(boost::out_degree(*vp.first, g) < 5){
                bool next_to_palm = false;
                std::pair<Gngt::adjacency_iterator, Gngt::adjacency_iterator> neighboors;
                for (neighboors = boost::adjacent_vertices(*vp.first, g); neighboors.first != neighboors.second; ++neighboors.first){
                    if(boost::out_degree(*neighboors.first, g) >= 5){
                        next_to_palm = true;
                        break;
                    }
                }
                if(!next_to_palm)
                    finger_map[*vp.first] = true;
            }
        }
    }
}

void Tracker::findPalmCenter(){
    // Retrieve the graph
    Gngt::Graph &g = m_mesh->getGraph();
    Gngt::finger_map_t finger_map = boost::get(vertex_finger, g);

    std::pair<Gngt::vertex_iter, Gngt::vertex_iter> vp;
    for (vp = boost::vertices(g); vp.first != vp.second; ++vp.first){
        if(connected_components_map[*vp.first] == m_biggest_comp && !finger_map[*vp.first]){
            /* *
             * Compute here the center of the palm
             * Also compute the Covariance matrix and use it to detect when all the inger are connected
             * to adapt the center of the palm accordingly
             * */
        }
    }
}

void Tracker::countFingers(){

}

void Tracker::draw(cv::Mat &img, Display mode){
    if(m_to_draw){
        Gngt::Graph &g = m_mesh->getGraph();
        // Retrieve the property map used in the method
        Gngt::pos_map_t pos_map = boost::get(vertex_pos, g);
        Gngt::finger_map_t finger_map = boost::get(vertex_finger, g);

        // Draw edges
        std::pair<Gngt::edge_iter, Gngt::edge_iter> ep;
        for(ep = boost::edges(g); ep.first != ep.second ; ++ep.first){
            Gngt::vertex_descriptor s = boost::source(*ep.first, g);
            Gngt::vertex_descriptor t = boost::target(*ep.first, g);
            if(connected_components_map[s] == m_biggest_comp){
                cv::line(img,
                         cv::Point(pos_map[s].first, pos_map[s].second),
                         cv::Point(pos_map[t].first, pos_map[t].second),
                         cv::Scalar(120,120,120),
                         2);
            }
        }

        // Draw vertices
        std::pair<Gngt::vertex_iter, Gngt::vertex_iter> vp;
        for (vp = boost::vertices(g); vp.first != vp.second; ++vp.first){
            if(connected_components_map[*vp.first] == m_biggest_comp){
                cv::Scalar color = cv::Scalar(255, 180, 0);

                if(finger_map[*vp.first])
                    color = cv::Scalar(0,0,255);

                cv::circle(img,
                           cv::Point(pos_map[*vp.first].first, pos_map[*vp.first].second),
                           5, cv::Scalar(150, 100, 0), -1);
                cv::circle(img,
                           cv::Point(pos_map[*vp.first].first, pos_map[*vp.first].second),
                           3, color, -1);
            }
        }
    }
}
