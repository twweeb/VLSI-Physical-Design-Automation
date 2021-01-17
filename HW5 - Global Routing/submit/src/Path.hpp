#ifndef PATH_H
#define PATH_H

#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include "Util.hpp"

class Node
{
private:
    Node* prev_;
    int edgeID_, nodeID_;
    double cost_;
    std::pair<int, int> coord_;

public:
    Node(Node* prev, std::pair<int, int> & coord):
        prev_(prev), cost_(0.0), coord_(coord){}
    ~Node() = default;

    
    double cost() {return cost_;}
    int edgeID() {return edgeID_;}
    int nodeID() {return nodeID_;}
    Node* prev() {return prev_;}
    std::pair<int, int>& coord(){return coord_;}
    void set_cost (double cost) {cost_ = cost;}
    void set_edgeID (int edgeID) {edgeID_ = edgeID;}
    void set_nodeID (int nodeID) {nodeID_ = nodeID;}
};

class Path
{
private:
    std::vector<std::pair<int, int>> coords_;
    std::vector<int> edgeIDs_;
    double cost_{0};
public:
    Path() = default;
    Path(Node* endNode);
    ~Path() = default;
    //friend std::ostream& operator<<(std::ostream& os, Path& path);
    const std::pair<int, int>& operator [](int idx) {return coords_[idx];}
    const std::vector<std::pair<int, int>>& coords() {return coords_;}
    const std::vector<int>& edgeIDs() {return edgeIDs_;}
    double cost() {return cost_;}
};

#endif