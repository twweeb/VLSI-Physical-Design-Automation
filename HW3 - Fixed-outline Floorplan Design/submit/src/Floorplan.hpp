#ifndef FLOORPLAN_H
#define FLOORPLAN_H

#include "BStree.hpp"
#include "Parser.hpp"
#include "Contour.hpp"

#define INF 1<<29
#define initTRYLimit 5e5
#define saTRYLimit 1e9
#define saTime 300
#define PENALTY_LIMIT 1.732e5

class Floorplan
{
    private:
        std::vector<terminal*> terms;
        std::vector<net*> nets;
        BStree bStree;
        Contour contour;
        Parser parser;
        unsigned seedPool[6] = {2587745236u,2771856801u,1043690592u,3469760771u,3600008343u,416905712u};
        int myBestWLPool[6] = {212738,205763,405667,388417,553757,533757};
        int idx = -1, initWL, initCost, finalWL, finalCost, myBestWL;
        double curDSR{0.0};
        unsigned seed;
        bool msg{false}, admissible{false}, flag{false};
        std::chrono::time_point<std::chrono::steady_clock> program_begin = std::chrono::steady_clock::now();
    public:
        Floorplan(){};
        ~Floorplan(){};
        void open(int argc, char *argv[]);
        void setFlag(){flag = true;};
        void setinit(int WL, int Cost){initWL = WL; initCost=Cost;};
        void setfinal(int WL, int Cost){finalWL = WL; finalCost=Cost;};
        void initFloorplan();
        int calWirelength ();
        void calDSR (int width);
        int cost (int);
        void SA ();
        void show_msg ();
        bool isAdmissible(){return admissible;};
        void finalAns ();
        void visualize ();
        int _get_initWL(){return initWL;};
        int _get_initCost(){return initCost;};
        int _get_finalWL(){return finalWL;};
        int _get_finalCost(){return finalCost;};
        BStree& _get_bStree(){return bStree;}
};

#endif