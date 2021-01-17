#ifndef NET_H
#define NET_H

#include <vector>
#include <utility>
#include <string>
#include <algorithm>
#include "Path.hpp"
#define INF 1<<30

class Net
{
private:
    std::string net_name_;
    int net_ID_, pin_nums_, HPWL_;
    std::vector<std::pair<int, int>> pins_coord_;
    Path path_;
    
public:
    Net(const std::string& net_name, int net_ID, int pin_nums, const std::vector<std::pair<int, int>>& pins_coord):
        net_name_(net_name), net_ID_(net_ID), pin_nums_(pin_nums), pins_coord_(pins_coord){updateHPWL ();}
    ~Net() = default;
    std::vector<std::pair<int, int>>& pins(){return pins_coord_;}
    void attachPath(const Path& path){path_ = std::move(path);}
    Path& path(){return path_;}
    
    void updateHPWL ();
    int HPWL () {return HPWL_;}
    int pinNum () {return pin_nums_;}
    int id () {return net_ID_;}
    std::string name () {return net_name_;}
};

#endif