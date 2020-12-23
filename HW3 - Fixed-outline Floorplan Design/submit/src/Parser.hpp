#ifndef PARSE_H
#define PARSE_H
#define initRotateRatio 0.666

#include <algorithm>
#include <chrono>
#include <string>
#include <string.h>
#include <cmath>
#include <fstream>
#include <set>
#include "BStree.hpp"

struct net
{
    int id, degree;
    std::set<int> blocks, terminals;
    bool operator() (net &lhs, net &rhs) { return (lhs.id<rhs.id);}
};

struct terminal
{
    int id, x, y;
};

class Parser
{
    private:
        bool msg{false}, seed{true};
        std::ifstream hardblocks_data, nets_data, term_data;
        std::ofstream floorplan, drawfig;
        int blockNum{0}, termNum{0}, netNum{0}, blockArea{0}, outline{0};
        double DSR{0.0};
        
    public:
        Parser(){};
        ~Parser(){hardblocks_data.close();nets_data.close();term_data.close();floorplan.close();drawfig.close();};
        void readARG(int, char**);
        void readBlock(BStree&);
        void readPin(std::vector<terminal*>&);
        void readNet(std::vector<net*>&);
        void update_outline();
        void finalAns (BStree &, int);
        void visualize (BStree &);
        int _get_blockNum(){return blockNum;};
        int _get_termNum(){return termNum;};
        int _get_netNum(){return netNum;};
        int _get_outline(){return outline;};
        int _get_blockArea(){return blockArea;};
        double _get_DSR(){return DSR;};
        bool _get_msg(){return msg;};
        bool _get_seed(){return seed;};
};

#endif