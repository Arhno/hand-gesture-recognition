#ifndef GNGT_H
#define GNGT_H

#include <utility>
#include <vector>
#include <list>
#include <limits>
#include <cmath>
#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/tuple/tuple.hpp>
#include <opencv2/opencv.hpp>

enum edge_age_t { edge_age };
enum vertex_error_t { vertex_error };
enum vertex_pos_t { vertex_pos };
enum vertex_win_t { vertex_win };
enum vertex_finger_t { vertex_finger };

namespace boost {
    BOOST_INSTALL_PROPERTY(edge, age);
    BOOST_INSTALL_PROPERTY(vertex, error);
    BOOST_INSTALL_PROPERTY(vertex, pos);
    BOOST_INSTALL_PROPERTY(vertex, win);
    BOOST_INSTALL_PROPERTY(vertex, finger);
}

class Gngt
{
public:

    typedef boost::property<edge_age_t, int> EdgeProperties;
    typedef boost::property<vertex_error_t, float,
                boost::property<vertex_pos_t, std::pair<float, float>,
                    boost::property<vertex_win_t, bool,
                        boost::property<vertex_finger_t, bool> > > > VertexProperties;

/*
    struct EdgeProperties {
        int age;
    };

    struct VertexProperties {
        float error;
        std::pair<float, float> position;
        bool hasWon;
    };
*/

    typedef boost::adjacency_list<boost::listS, boost::listS, boost::undirectedS,
                                  VertexProperties, EdgeProperties> Graph;

    typedef boost::property_map<Graph, edge_age_t>::type age_map_t;
    typedef boost::property_map<Graph, vertex_error_t>::type error_map_t;
    typedef boost::property_map<Graph, vertex_pos_t>::type pos_map_t;
    typedef boost::property_map<Graph, vertex_win_t>::type win_map_t;
    typedef boost::property_map<Graph, vertex_finger_t>::type finger_map_t;

    typedef boost::graph_traits<Graph>::vertex_iterator vertex_iter;
    typedef boost::graph_traits<Graph>::edge_iterator edge_iter;
    typedef boost::graph_traits<Graph>::adjacency_iterator adjacency_iterator;
    typedef boost::graph_traits<Graph>::out_edge_iterator out_edge_iterator;

    typedef boost::graph_traits<Graph>::vertex_descriptor vertex_descriptor;
    typedef boost::graph_traits<Graph>::edge_descriptor edge_descriptor;

    Gngt(float target, float fastRate, float slowRate, int maxAge)
        : m_t(target),
          m_alpha1(fastRate),
          m_alpha2(slowRate),
          m_maxAge(maxAge)
    {}

    template<typename Iterator>
    void epoch(Iterator begin, Iterator end, bool add_node = true){
        // Retrieve the property map used in the method
        age_map_t age_map = boost::get(edge_age, m_graph);
        win_map_t win_map = boost::get(vertex_win, m_graph);
        error_map_t error_map = boost::get(vertex_error, m_graph);
        pos_map_t pos_map = boost::get(vertex_pos, m_graph);

        // Used whenever we need to iterate through the vertices
        std::pair<vertex_iter, vertex_iter> vp;

        // Clean the error computed during the last epoch
        for (vp = boost::vertices(m_graph); vp.first != vp.second; ++vp.first){
            error_map[*vp.first] = 0;
            win_map[*vp.first] = false;
        }

        // Make sure the graph has at least two nodes
        while(boost::num_vertices(m_graph) < 2){
            vertex_descriptor vd = boost::add_vertex(m_graph);
            error_map[vd] = 0;
            win_map[vd] = false;
            pos_map[vd] = *begin;
            ++begin;
        }

        // Update the graph topology for each sample
        while(begin != end){
            update(*begin);
            ++begin;
        }

        // Remove vertices that didn't win this epoch and the ones without edges
        // In the mean time, compute the mean error and the vertices with the biggest
        // and smallest error
        vp = boost::vertices(m_graph);
        vertex_iter next;
        for (next = vp.first; vp.first != vp.second; vp.first=next){
            ++next;
            if(!win_map[*vp.first]){
                //std::cout << win_map[*vp.first] << " - " << boost::out_degree(*vp.first, m_graph) << std::endl;
                boost::clear_vertex(*vp.first, m_graph);
                //std::cout << boost::out_degree(*vp.first, m_graph) << std::endl;
                boost::remove_vertex(*vp.first, m_graph);
            }
        }

        vp = boost::vertices(m_graph);
        for (next = vp.first; vp.first != vp.second; vp.first=next){
            ++next;
            if(boost::out_degree(*vp.first, m_graph) == 0){
                boost::remove_vertex(*vp.first, m_graph);
            }
        }

        vp = boost::vertices(m_graph);
        vertex_descriptor max_ver = *vp.first;
        vertex_descriptor min_ver = *vp.first;
        float mean = 0;
        float max = error_map[max_ver];
        float min = error_map[min_ver];
        for (; vp.first != vp.second; ++vp.first){
            //std::cout << "has won: " << pos_map[*vp.first].first << " " << pos_map[*vp.first].second << std::endl;
            float error = error_map[*vp.first];
            mean += error;
            if(error < min){
                min = error;
                min_ver = *vp.first;
            }
            if(error > max){
                max = error;
                max_ver = *vp.first;
            }
        }
        mean /= boost::num_vertices(m_graph);

        if(add_node){
            // Adapt the number of vertices to get closer to the target
            if(mean < m_t){
                // to much accuracy, remove the vertex with the least error
                boost::clear_vertex(min_ver, m_graph);
                boost::remove_vertex(min_ver, m_graph);
            } else {
                // not enough accuracy, add a vertex where needed
                max = 0;
                vertex_descriptor max_neighbor;
                std::pair<adjacency_iterator, adjacency_iterator> vp;
                //std::cout << "max: " << pos_map[max_ver].first << ", " << pos_map[max_ver].second << std::endl;
                bool neighborFound = false;
                for (vp = boost::adjacent_vertices(max_ver, m_graph); vp.first != vp.second; ++vp.first){
                    //std::cout << "5.3 " << std::endl;
                    float error = error_map[*vp.first];
                    //std::cout << "5.4 " << pos_map[*vp.first].first << " " << pos_map[*vp.first].second << std::endl;
                    if(error > max){
                        max = error;
                        //std::cout << "neighbor found" << std::endl;
                        neighborFound = true;
                        max_neighbor = *vp.first;
                    }
                }

                if(neighborFound){
                    //std::cout << "neighbor pos (" << max_neighbor << "): " << pos_map[max_neighbor].first << " " << pos_map[max_neighbor].second << std::endl;


                    //std::cout << "6" << std::endl;
                    edge_descriptor edge_to_delete;
                    bool edge_to_delete_exist;
                    boost::tie(edge_to_delete, edge_to_delete_exist) = boost::edge(max_ver, max_neighbor, m_graph);
                    if(edge_to_delete_exist)
                        boost::remove_edge(max_ver, max_neighbor, m_graph);

                    //std::cout << "neighbor pos 2 (" << max_neighbor << "): " << pos_map[max_neighbor].first << " " << pos_map[max_neighbor].second << std::endl;


                    //std::cout << "7" << std::endl;
                    float x1, x2, y1, y2;
                    boost::tie(x1,y1) = pos_map[max_ver];
                    boost::tie(x2,y2) = pos_map[max_neighbor];

                    //std::cout << "8: " << x1 << " " << y1 << " " << x2 << " " << y2 << std::endl;
                    vertex_descriptor vd = boost::add_vertex(m_graph);
                    error_map[vd] = 0;
                    win_map[vd] = false;
                    pos_map[vd] = std::make_pair((x1+x2)/2, (y1+y2)/2);

                    //std::cout << "9" << std::endl;
                    edge_descriptor ed;
                    //std::cout << "9'" << std::endl;
                    bool exist;
                    //std::cout << "9''" << std::endl;
                    boost::tie(ed, exist) = boost::add_edge(max_ver, vd, m_graph);
                    //std::cout << "9'''" << std::endl;
                    if(exist)
                        age_map[ed] = 0;
                    //std::cout << "9''''" << std::endl;
                    boost::tie(ed, exist) = boost::add_edge(max_neighbor, vd, m_graph);
                    //std::cout << "9'''''" << std::endl;
                    if(exist)
                        age_map[ed] = 0;
                    //std::cout << "10" << std::endl;
                }
            }
        }
    }

    void changeTarget(int newTarget){
        m_t = newTarget;
    }

    void draw(cv::Mat &img){
        // Retrieve the property map used in the method
        pos_map_t pos_map = boost::get(vertex_pos, m_graph);
        finger_map_t finger_map = boost::get(vertex_finger, m_graph);

        std::pair<edge_iter, edge_iter> ep;
        for(ep = boost::edges(m_graph); ep.first != ep.second ; ++ep.first){
            vertex_descriptor s = boost::source(*ep.first, m_graph);
            vertex_descriptor t = boost::target(*ep.first, m_graph);
            cv::line(img,
                     cv::Point(pos_map[s].first, pos_map[s].second),
                     cv::Point(pos_map[t].first, pos_map[t].second),
                     cv::Scalar(120,120,120),
                     2);
        }

        std::pair<vertex_iter, vertex_iter> vp;
        for (vp = boost::vertices(m_graph); vp.first != vp.second; ++vp.first){
            cv::Scalar color = cv::Scalar(255, 180, 0);
            if(boost::out_degree(*vp.first, m_graph) < 4){
                bool next_to_palm = false;
                std::pair<adjacency_iterator, adjacency_iterator> neighboors;
                for (neighboors = boost::adjacent_vertices(*vp.first, m_graph); neighboors.first != neighboors.second; ++neighboors.first){
                    if(boost::out_degree(*neighboors.first, m_graph) >= 4){
                        next_to_palm = true;
                        break;
                    }
                }
                if(!next_to_palm)
                    color = cv::Scalar(0,0,255);
            }

//            if(finger_map[*vp.first])
//                color = cv::Scalar(0,0,255);

            cv::circle(img,
                       cv::Point(pos_map[*vp.first].first, pos_map[*vp.first].second),
                       5, cv::Scalar(150, 100, 0), -1);
            cv::circle(img,
                       cv::Point(pos_map[*vp.first].first, pos_map[*vp.first].second),
                       3, color, -1);
        }
    }

private:
    Graph m_graph;
    float m_t;
    float m_alpha1;
    float m_alpha2;
    int m_maxAge;

    void update(const std::pair<float, float> &position){
        // Retrieve the property map used in the method
        age_map_t age_map = boost::get(edge_age, m_graph);
        win_map_t win_map = boost::get(vertex_win, m_graph);
        error_map_t error_map = boost::get(vertex_error, m_graph);
        pos_map_t pos_map = boost::get(vertex_pos, m_graph);

        // Used whenever we need to iterate through the vertices
        std::pair<vertex_iter, vertex_iter> vp;
        vertex_descriptor closest, second_closest;
        float d1 = std::numeric_limits<float>::infinity();
        float d2 = std::numeric_limits<float>::infinity();

        for (vp = boost::vertices(m_graph); vp.first != vp.second; ++vp.first){
            float distance = dist(position, pos_map[*vp.first]);
            if(distance < d1){
                d2 = d1;
                second_closest = closest;
                d1 = distance;
                closest = *vp.first;
            } else if (distance < d2) {
                d2 = distance;
                second_closest = *vp.first;
            }
        }

        win_map[closest] = true;
        error_map[closest] += d1;

        edge_descriptor ed;
        bool exist;
        boost::tie(ed, exist) = boost::edge(closest, second_closest, m_graph);
        if(!exist){
            boost::tie(ed, exist) = boost::add_edge(closest, second_closest, m_graph);
        }
        age_map[ed] = 0;

        std::pair<out_edge_iterator, out_edge_iterator> ep = boost::out_edges(closest, m_graph);
        out_edge_iterator nextE;
        for(nextE = ep.first; ep.first != ep.second ; ep.first = nextE){
            ++nextE;
            ++age_map[*ep.first];
            if(age_map[*ep.first] >= m_maxAge){
                boost::remove_edge(ep.first, m_graph);
            }
        }
//        boost::remove_out_edge_if(closest,
//                                  [this](edge_descriptor &e){
//                                      return this->m_graph[e].age >= this->m_maxAge;
//                                  },
//                                  m_graph);

        float x, y;
        boost::tie(x,y) = pos_map[closest];
        pos_map[closest].first += m_alpha1 * (position.first - x);
        pos_map[closest].second += m_alpha1 * (position.second - y);

        std::pair<adjacency_iterator, adjacency_iterator> nei;
        for (nei = boost::adjacent_vertices(closest, m_graph); nei.first != nei.second; ++nei.first){
            boost::tie(x,y) = pos_map[*nei.first];
            pos_map[*nei.first].first += m_alpha2 * (position.first - x);
            pos_map[*nei.first].second += m_alpha2 * (position.second - y);
        }
    }

    float dist(const std::pair<float,float> &d1, const std::pair<float,float> &d2){
        float dx = d1.first - d2.first ;
        float dy = d1.second - d2.second ;
        return sqrt(dx*dx + dy*dy);
    }
};

#endif // GNGT_H
