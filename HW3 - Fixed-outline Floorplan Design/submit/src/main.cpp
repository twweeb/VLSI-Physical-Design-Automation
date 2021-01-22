//
//  main.cpp
//  CS6135 VLSI Physical Design Automation
//  Homework 3: Fixed-outline Floorplan Design
//
//  Created by Lei Hsiung on 2020/11/21.
//  Copyright © 2020 Lei Hsiung. All rights reserved.
//

#include "Floorplan.hpp"

using namespace std;

int main (int argc, char *argv[])
{
    Floorplan floorplans[num_thread];
    bool flag_for_thread = false;
    for(int i=0; i<num_thread; ++i) floorplans[i].open(argc, argv);

    #pragma omp parallel for num_threads(num_thread)
    for(int i=0; i<num_thread; ++i) 
    {
        floorplans[i].initFloorplan();
        if (i!=0) std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count());
        floorplans[i].SA();
        
        if (!flag_for_thread)
        {
            flag_for_thread = true;
            for (int j = 0; j < num_thread; ++j) floorplans[j].setFlag();
        }
	}

    int minWL = INF, minWL_T=-1;
    // Visualize
    for(int i=0; i<num_thread; ++i)
    {
        auto& floorplan = floorplans[i];
        if(floorplan.isAdmissible()) 
        {
            if (floorplan._get_finalWL() < minWL)
            {
                minWL = floorplan._get_finalWL();
                minWL_T = i;
            }
        }
    }

    floorplans[minWL_T].show_msg();
    floorplans[minWL_T].visualize();
    floorplans[minWL_T].finalAns();

    return 0;
}