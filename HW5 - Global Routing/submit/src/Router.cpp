#include "Router.hpp"
#include <fstream>
#include <string>
#include <iostream>

void Router::readInput(const std::string& testcase)
{
    std::ifstream fh;
    std::string _;
    fh.open(testcase);

    fh >> _ >> grid_Hor_ >> grid_Ver_;
    fh >> _ >> _ >> capacity_Ver_;
    fh >> _ >> _ >> capacity_Hor_;
    fh >> _ >> _ >> num_nets_;

    seed_ = (seed_map.find(num_nets_) != seed_map.end()) ? seed_map[num_nets_] : time(NULL);
    seed_feasible_ = (seed_map.find(num_nets_) != seed_map.end()) ? true : false;
    init_overflow_ = (init_overflow_map.find(num_nets_) != init_overflow_map.end()) ? init_overflow_map[num_nets_] : INF;
    init_wirelength_ = (init_wirelength_map.find(num_nets_) != init_wirelength_map.end()) ? init_wirelength_map[num_nets_] : INF;
    for (int i = 0; i < num_nets_; ++i)
    {
        std::string net_name;
        int net_id, pin_num, pin_x, pin_y;
        fh >> net_name >> net_id >> pin_num;
        std::vector<std::pair<int, int>> net_pins;
        for (int j = 0; j < pin_num; ++j)
        {
            fh >> pin_x >> pin_y;
            net_pins.emplace_back(pin_x, pin_y);
        }
        Nets.emplace_back(net_name, net_id, pin_num, net_pins);
    }
    
    num_nodes_ = grid_Hor_ * grid_Ver_;
    num_edges_ = (grid_Hor_-1)*grid_Ver_ + (grid_Ver_-1)*grid_Hor_;
    num_hor_edges_ = (grid_Hor_-1)*grid_Ver_;
    demand.resize(num_edges_);
    std::fill(demand.begin(), demand.end(), 0);

    fh.close();

    // show info
    if (show_log_ && router_id_ == 0)
    {
        std::cout << "Grid: " << grid_Hor_ << " x "  << grid_Ver_ << '\n';
        std::cout << "Vertical_cap: " << capacity_Ver_ << '\n';
        std::cout << "Horizontal_cap: " << capacity_Hor_ << '\n';
        std::cout << "# of nets: " << num_nets_ << '\n';
    }
}

std::pair<int, int> Router::get_next_coord (std::pair<int, int>& cur, int dir)
{
    if (dir == 0) return {cur.first,cur.second+1}; // up
    else if (dir == 1) return {cur.first+1,cur.second}; // right
    else if (dir == 2) return {cur.first,cur.second-1}; // down
    else return {cur.first-1,cur.second}; // left
}

bool Router::is_legal_coord (std::pair<int, int>& coord)
{
    if (coord.first < 0 || coord.first >= grid_Hor_) return false;
    if (coord.second < 0 || coord.second >= grid_Ver_) return false;
    return true;
}

double Router::get_edge_cost (int edgeID)
{
    double base{1.0};
    if (edgeID < num_hor_edges_)
    {
        if (demand[edgeID] < capacity_Hor_) return base + 1.0*(demand[edgeID]+1)/capacity_Hor_;
        else return 1e4;
    }
    else
    {
        if (demand[edgeID] < capacity_Ver_) return base + 1.0*(demand[edgeID]+1)/capacity_Ver_;
        else return 1e4;
    }
}

int Router::get_edge_id (std::pair<int, int>& coord, int dir)
{
    if (dir == 0) // up
        return num_hor_edges_ + (grid_Hor_ - 1)*coord.second + coord.first;
    else if (dir == 1) // right
        return (grid_Hor_ - 1)*coord.second + coord.first;
    else if (dir == 2) // down
        return num_hor_edges_ + (grid_Hor_ - 1)*(coord.second - 1) + coord.first;
    else // left
        return (grid_Hor_ - 1)*coord.second + (coord.first - 1);
}

int Router::get_node_id (std::pair<int, int>& coord)
{
    return grid_Hor_*coord.second + coord.first;
}

void Router::routeTwoPinNet(Net& net)
{
    ASSERT(net.pinNum() == 2);

    std::pair<int, int> start{net.pins()[0]}, end{net.pins()[1]};

    Node* node = new Node(nullptr, start);
    node->set_edgeID(-1);
    node->set_nodeID(this->get_node_id(node->coord()));
    
    auto cmp = [](Node* left, Node* right) { return (*left).cost() > (*right).cost(); };
    std::priority_queue<Node*, std::vector<Node*>, decltype(cmp)> q(cmp);
    q.push(node);

    std::vector<bool> nodeUsed(num_nodes_, false);
    Node *bestPath{nullptr};
    while (!q.empty()) // Low-cost BFS
    {
        Node* cur_node = q.top();
        q.pop();

        if (nodeUsed[cur_node->nodeID()]) continue;
        nodeUsed[cur_node->nodeID()] = true;

        if (cur_node->coord() == end)
        {
            bestPath = cur_node;
            break;
        }

        for (int i = 0; i < 4; ++i)
        {
            std::pair<int, int> next_coord{this->get_next_coord(cur_node->coord(), i)};
            if (!this->is_legal_coord(next_coord)) continue;

            Node* next_node = new Node(cur_node, next_coord);
            next_node->set_edgeID(this->get_edge_id(cur_node->coord(), i));
            next_node->set_nodeID(this->get_node_id(next_node->coord()));
            next_node->set_cost(cur_node->cost() + this->get_edge_cost(next_node->edgeID()));
            q.push(next_node);
        }
    }
    ASSERT (bestPath != nullptr);
    net.attachPath(Path(bestPath));
}

void Router::updateDemand(Path& path, bool add)
{
    for (const auto edgeID: path.edgeIDs()) 
    {
        if (add) ++demand[edgeID];
        else --demand[edgeID];
    }
}

void Router::updateOverflow()
{
    max_overflow_ = 0; total_overflow_ = 0; wirelength_ = 0;
    int overflow{0};
    for (int i = 0; i < num_edges_; ++i)
    {
        //std::cout << demand[i] << '\n';
        wirelength_ += demand[i];
        overflow = demand[i] - ((i < num_hor_edges_) ? capacity_Hor_ : capacity_Ver_);
        if (overflow <= 0) continue;
        
        total_overflow_ += overflow;
        max_overflow_ = std::max(overflow, max_overflow_);
    }
}

void Router::globalRoute()
{
    unsigned cur_seed = 0u;
    for (int i = 0; i < ((seed_feasible_) ? 1 : num_init_trials); ++i)
    {
        // Reset
        std::sort(Nets.begin(), Nets.end(), [](Net a, Net b) {return a.id() < b.id();});
        std::fill(demand.begin(), demand.end(), 0);

        #pragma omp critical
        {
            std::srand(cur_seed = ((i == ((seed_feasible_) ? 1 : num_init_trials) - 1) ? seed_ : std::chrono::system_clock::now().time_since_epoch().count()));
            std::random_shuffle(Nets.begin(), Nets.end());
            std::sort(Nets.begin(), Nets.end(), [](Net a, Net b) {return a.HPWL() < b.HPWL();});
        }
        //std::cout << "Thread " << router_id_ << " routeTwoPinNet\n";
        for (auto& net: Nets)
        {
            if (net.pinNum() == 2) routeTwoPinNet(net);
            updateDemand(net.path(), true);
        }
        //std::cout << "Thread " << router_id_ << " updateOverflow\n";
        updateOverflow();
        if (total_overflow_ <= init_overflow_)
        {
            if (wirelength_ < init_wirelength_ || total_overflow_ < init_overflow_)
            {
                seed_ = cur_seed;
                init_overflow_ = total_overflow_;
                init_wirelength_ = wirelength_;

                #pragma omp critical
                {
                    std::cout << "seed " << cur_seed << ", overflow: " << total_overflow_ << ", wirelength: " << wirelength_ << '\n';
                }
            }
        }
    }
    std::sort(Nets.begin(), Nets.end(), [](Net a, Net b) {return a.id() < b.id();});
}

bool Router::is_overflow (Path& path)
{
    for (auto edgeID: path.edgeIDs())
        if (demand[edgeID] > ((edgeID < num_hor_edges_) ? capacity_Hor_ : capacity_Ver_)) 
            return true;
    return false;
}

void Router::rip_reroute ()
{
    static int cnt = 0;
    ++cnt;
    if (cnt == num_reroute) return;

    std::vector<std::pair<int,int>> overflow_nets;
    for (auto& net: Nets)  // rip-off
    {
        Path path{net.path()};
        if (is_overflow(path)) overflow_nets.push_back({net.HPWL(),net.id()});
    }
    
    std::random_shuffle(overflow_nets.begin(), overflow_nets.end());
    std::sort(overflow_nets.begin(), overflow_nets.end()); //, [](auto& a, auto& b) {return a.second < b.second;}
    for (auto& ovf_net: overflow_nets)  // re-route
    {
        updateDemand(Nets[ovf_net.second].path(), false);
        routeTwoPinNet(Nets[ovf_net.second]);
        updateDemand(Nets[ovf_net.second].path(), true);
    }
    updateOverflow();
    if (show_log_) std::cout << "Reroute " << cnt << ", overflow: " << total_overflow_ << ", wirelength: " << wirelength_ << '\n';
    if (total_overflow_) rip_reroute();
}

void Router::dumpResult(const std::string& result)
{
    std::ofstream fh;
    fh.open(result);
    for (auto& net: Nets)
    {
        fh << net.name() << ' ' << net.id() << '\n';
        Path path{net.path()};
        std::vector<std::pair<int, int>> coords{path.coords()};
        for (int i = 0; i < coords.size()-1; ++i) 
        {
            fh << '(' << coords[i].first << ", " << coords[i].second << ", 1)" << '-'
               << '(' << coords[i+1].first << ", " << coords[i+1].second << ", 1)\n";
        }
        fh << "!\n";
    }
    fh.close();
    return;
}