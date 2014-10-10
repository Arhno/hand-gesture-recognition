#include "tracking.h"
#include "unionfind.h"
#include <vector>
#include <fstream>
#include <math.h>
#include <boost/graph/connected_components.hpp>

#define PI 3.14159265

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
    computeStds();
    normalizeFingerCenters();
    computeAngles();
    computeDistances();
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

    std::cout << "components: " << num << std::endl;
    m_biggest_comp = 0;
    int maxNb = 0;
    for(int i=0 ; i<num ; ++i){
        if(nb_elements[i] > maxNb){
            maxNb = nb_elements[i];
            m_biggest_comp = i;
        }
    }

    if(maxNb > 65 && !m_mesh->is_frozen()){
        // Remove everything but the maximal connected componant
        vp = boost::vertices(g);
        Gngt::vertex_iter next;
        for (next = vp.first; vp.first != vp.second; vp.first=next){
            ++next;
            if(connected_components_map[*vp.first] != m_biggest_comp){
                boost::clear_vertex(*vp.first, g);
                boost::remove_vertex(*vp.first, g);
            }
        }

        // Freeze the number of nodes in the graph
        m_mesh->freeze();
    }

    std::cout << "-> " << maxNb << std::endl ;
}

void Tracker::findFingerNodes(){

    Gngt::Graph &g = m_mesh->getGraph();
    // Retrieve the property map used in the method
    Gngt::finger_map_t finger_map = boost::get(vertex_finger, g);

    int nb_neighboor = 2;

    std::pair<Gngt::vertex_iter, Gngt::vertex_iter> vp;
    for (vp = boost::vertices(g); vp.first != vp.second; ++vp.first){
        if(connected_components_map[*vp.first] == m_biggest_comp){

            //if(!m_mesh->is_frozen())
                finger_map[*vp.first] = false;

            if(boost::out_degree(*vp.first, g) <= nb_neighboor){
                bool next_to_palm = false;
                std::pair<Gngt::adjacency_iterator, Gngt::adjacency_iterator> neighboors;
                for (neighboors = boost::adjacent_vertices(*vp.first, g); neighboors.first != neighboors.second; ++neighboors.first){
                    if(boost::out_degree(*neighboors.first, g) > nb_neighboor){
                        next_to_palm = true;
                        break;
                    }
                }
                //if(!next_to_palm)
                    finger_map[*vp.first] = true;
            }
        }
    }
}

void Tracker::findPalmCenter(){
    // Retrieve the graph
    Gngt::Graph &g = m_mesh->getGraph();
    Gngt::finger_map_t finger_map = boost::get(vertex_finger, g);
    Gngt::pos_map_t pos_map = boost::get(vertex_pos, g);

    int nb_nodes_in_palm = 0;
    m_palm_center.first = 0;
    m_palm_center.second = 0;

    std::pair<Gngt::vertex_iter, Gngt::vertex_iter> vp;
    for (vp = boost::vertices(g); vp.first != vp.second; ++vp.first){
        if(connected_components_map[*vp.first] == m_biggest_comp && !finger_map[*vp.first]){
            /* *
             * Compute here the center of the palm
             * Also compute the Covariance matrix and use it to detect when all the inger are connected
             * to adapt the center of the palm accordingly
             * */
            ++ nb_nodes_in_palm;
            m_palm_center.first += pos_map[*vp.first].first;
            m_palm_center.second += pos_map[*vp.first].second;
        }
    }
    if(nb_nodes_in_palm>0){
        m_palm_center.first /= nb_nodes_in_palm;
        m_palm_center.second /= nb_nodes_in_palm;
    }
}

void Tracker::computeStds(){
    // Retrieve the graph
    Gngt::Graph &g = m_mesh->getGraph();
    Gngt::finger_map_t finger_map = boost::get(vertex_finger, g);
    Gngt::pos_map_t pos_map = boost::get(vertex_pos, g);

    int nb_nodes_in_palm = 0;
    m_palm_stds.first = 0;
    m_palm_stds.second = 0;

    std::pair<Gngt::vertex_iter, Gngt::vertex_iter> vp;
    for (vp = boost::vertices(g); vp.first != vp.second; ++vp.first){
        if(connected_components_map[*vp.first] == m_biggest_comp && !finger_map[*vp.first]){
            /* *
             * Compute here the std of the palm (x-drirection)
             * */
            ++ nb_nodes_in_palm;
            m_palm_stds.first += (pos_map[*vp.first].first - m_palm_center.first) *  (pos_map[*vp.first].first - m_palm_center.first);
            m_palm_stds.second += (pos_map[*vp.first].second - m_palm_center.second) *  (pos_map[*vp.first].second - m_palm_center.second);
        }
    }
    if(nb_nodes_in_palm>0){
        m_palm_stds.first = sqrt(m_palm_stds.first/nb_nodes_in_palm);
        m_palm_stds.second = sqrt(m_palm_stds.second/nb_nodes_in_palm);
    }
}

void Tracker::countFingers(){
    UnionFind<Gngt::vertex_descriptor> u;

    // Retrieve the graph
    Gngt::Graph &g = m_mesh->getGraph();
    Gngt::finger_map_t finger_map = boost::get(vertex_finger, g);
    Gngt::pos_map_t pos_map = boost::get(vertex_pos, g);

    std::pair<Gngt::vertex_iter, Gngt::vertex_iter> vp;
    for (vp = boost::vertices(g); vp.first != vp.second; ++vp.first){
        if(connected_components_map[*vp.first] == m_biggest_comp && finger_map[*vp.first]){
            u.unite(*vp.first, *vp.first);
            std::pair<Gngt::adjacency_iterator, Gngt::adjacency_iterator> neighboors;
            for (neighboors = boost::adjacent_vertices(*vp.first, g);
                 neighboors.first != neighboors.second;
                 ++neighboors.first){
                if(finger_map[*neighboors.first]){
                    u.unite(*vp.first, *neighboors.first);
                }
            }
        }
    }

    std::vector<std::vector<Gngt::vertex_descriptor>> components;
    m_nb_finger = u.connectedComponents(components);
    //std::cout << "before: " << m_nb_finger << std::endl;

    m_finger_center.clear();
    m_finger_center.reserve(m_nb_finger);
    m_finger_center_normalized.clear();
    m_finger_center_normalized.reserve(m_nb_finger);
    fingerDistances.clear();
    fingerDistances.reserve(m_nb_finger);
    fingerAngles.clear();
    fingerAngles.reserve(m_nb_finger);
    
    m_nb_finger = 0;
    for(int i=0 ; i<components.size() ; ++i){
        if(components[i].size() >= 3){

            m_finger_center.push_back(std::make_pair(0.0f, 0.0f));
            for(int j=0 ; j<components[i].size() ; ++j){
                m_finger_center.back().first += pos_map[components[i][j]].first;
                m_finger_center.back().second += pos_map[components[i][j]].second;
            }
            m_finger_center.back().first /= components[i].size();
            m_finger_center.back().second /= components[i].size();

            ++m_nb_finger;
        } else {
            // Erase the false fingers
            for(int j=0 ; j<components[i].size() ; ++j){
                finger_map[components[i][j]] = false;
            }
        }
    }
    
    for(int i=0 ; i<m_finger_center.size() ; ++i){
        for(int j =0; j<m_finger_center.size(); j++)
        {
            if (m_finger_center[i].first > m_finger_center[j].first) {
                int temp = m_finger_center[i].first;
                m_finger_center[i].first=m_finger_center[j].first;
                m_finger_center[j].first=temp;
                temp = m_finger_center[i].second;
                m_finger_center[i].second=m_finger_center[j].second;
                m_finger_center[j].second=temp;
            }
        }
    }
    //std::cout << "after: " << m_nb_finger << std::endl;
}

void Tracker::normalizeFingerCenters(){
    for(int i=0 ; i<m_finger_center.size() ; ++i){
        m_finger_center_normalized[i].first = (m_finger_center[i].first - m_palm_center.first);
        m_finger_center_normalized[i].second = (m_finger_center[i].first - m_palm_center.first);
    }
}

void Tracker::computeAngles(){
    for(int i=0 ; i<m_finger_center.size() ; ++i){
        float dist1,dist2;
        dist1 = (m_finger_center[i].first - m_palm_center.first);
        dist2 = (m_palm_center.second - m_finger_center[i].second);
        fingerAngles[i] = atan(dist1/dist2) * 180/PI;
        std::cout << "Angles: " << fingerAngles[i] << std::endl;
    }
}

void Tracker::computeDistances(){
    for(int i=0 ; i<m_finger_center.size(); ++i){
        fingerDistances[i] = (m_finger_center[i].first - m_palm_center.first);
        std::cout << "Distance: " << fingerDistances[i] << std::endl;
    }
}

void Tracker::draw(cv::Mat &img, Display mode){
    if(m_to_draw){
        Gngt::Graph &g = m_mesh->getGraph();
        // Retrieve the property map used in the method
        Gngt::pos_map_t pos_map = boost::get(vertex_pos, g);
        Gngt::finger_map_t finger_map = boost::get(vertex_finger, g);

        // Draw finger centers
        for(int i=0 ; i<m_finger_center.size() ; ++i){
            cv::circle(img,
                       cv::Point(m_finger_center[i].first, m_finger_center[i].second),
                       7, cv::Scalar(0, 200, 200), -1);
        }

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

        // Draw palm center
        cv::circle(img,
                   cv::Point(m_palm_center.first, m_palm_center.second),
                   10, cv::Scalar(0, 200, 0), -1);

    } else {
        m_mesh->freeze(false);
    }
}
