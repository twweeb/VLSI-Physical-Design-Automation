#ifndef ROUTER_H
#define ROUTER_H

#include "Net.hpp"
#include "Path.hpp"
#include "Util.hpp"
#include <omp.h>
#include <vector>
#include <map>
#include <string>
#include <queue>
#include <cstdlib>
#include <chrono>

#define INF 1<<30
#define num_thread 2
#define num_init_trials 10
#define num_reroute 15

class Router
{
private:
    std::vector<Net> Nets;
    std::vector<int> demand;
    int router_id_;
    int grid_Hor_, grid_Ver_, capacity_Ver_, capacity_Hor_;
    int num_nets_, num_hor_edges_, num_nodes_, num_edges_;
    int max_overflow_, total_overflow_, wirelength_;
    std::map<int,unsigned> seed_map{{13357,765426436u},{22465,2778138155u},{21609,4093394120u},{27781,632883833u}}; // 1512147616
    std::map<int,int> init_overflow_map{{13357,0},{22465,0},{21609,0},{27781,148}};
    std::map<int,int> init_wirelength_map{{13357,60025},{22465,157550},{21609,142502},{27781,160678}};
    unsigned seed_ = 0u;
    int init_overflow_, init_wirelength_;
    bool show_log_{false}, seed_feasible_{false};

public:
    Router() = default;
    ~Router() = default;
    void set_id (int id) {router_id_ = id;}
    void readInput(const std::string&);
    void routeTwoPinNet(Net&);
    void updateOverflow();
    void updateDemand(Path&, bool);
    void globalRoute();
    void rip_reroute();
    std::pair<int, int> get_next_coord (std::pair<int, int>&, int);
    double get_edge_cost(int);
    bool is_legal_coord (std::pair<int, int>&);
    bool is_overflow (Path&);
    void dumpResult(const std::string&);
    bool show_log() {return show_log_;}

    int get_edge_id (std::pair<int, int>&, int);
    int get_node_id (std::pair<int, int>&);
    int overflow() {return total_overflow_;}
    int wirelength() {return wirelength_;}
    int max_overflow() {return max_overflow_;}
    unsigned seed() {return seed_;}
};

#endif