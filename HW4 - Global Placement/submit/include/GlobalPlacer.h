#ifndef GLOBALPLACER_H
#define GLOBALPLACER_H

#include "Placement.h"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <chrono>
#include <map>
#define INIT_TRY 1e4

class GlobalPlacer
{
public:
    GlobalPlacer(Placement &placement): _placement(placement){};
    void place();
    void plotPlacementResult(const string outfilename, bool isPrompt = false);
    void randomPlace(vector<double>&); // An example of random placement implemented by TA
    void netFirstPlace(vector<double>&, unsigned);
    void seedSearch(vector<double>&);
    void netFirstPlaceInit(unsigned);
    int getIdx(int);

private:
    Placement &_placement;
    void plotBoxPLT(ofstream &stream, double x1, double y1, double x2, double y2);
    map<int,int> dict{{11507,0},{18429,1},{28446,2},{44394,3},{47944,4},{50393,5}};
    unsigned seedPool[6]={2905740380u,1216657378u,0u,4109022181u,2351525260u,1356089626u};
    double binMaxArea{0.0}, boundW{0.0}, boundH{0.0}, binW{0.0}, binH{0.0};
    unsigned numNets{0u}, binCut{20};
};

#endif // GLOBALPLACER_H
