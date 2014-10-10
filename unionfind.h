#ifndef UNIONFIND_H
#define UNIONFIND_H

#include <unordered_map>
#include <vector>

template <class Key>
class UnionFind
{
public:
    UnionFind(){}

    int connectedComponents(std::vector<std::vector<Key>> &components){
        components.clear();
        std::unordered_map<Key, int> mapping;
        for(auto iter = id.begin() ; iter != id.end() ; ++iter){
            Key r = root(iter->first);
            if(mapping.count(r) == 0){
                mapping[r] = components.size();
                components.push_back(std::vector<Key>(0));
            }
            components[mapping[r]].push_back(iter->first);
        }

        return components.size();
    }

    void unite(Key a, Key b){
        a = root(a);
        b = root(b);
        if( a != b ){
            if(size[a]>size[b]){
                id[b] = a;
                size[a] += size[b];
            } else {
                id[a] = b;
                size[b] += size[a];
            }
        }
    }

private:
    std::unordered_map<Key,Key> id;
    std::unordered_map<Key,int> size;

    Key root(Key i){
        if(id.count(i) == 0){
            id[i] = i;
            size[i] = 1;
        }

        while (id[i] != i){
            id[i] = id[id[i]];
            i = id[i];
        }

        return i;
    }
};

#endif // UNIONFIND_H
