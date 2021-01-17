//
//  main.cpp
//  CS6135 VLSI Physical Design Automation
//  Homework 5: Global Routing
//
//  Created by Lei Hsiung on 2021/01/10.
//  Copyright © 2020 Lei Hsiung. All rights reserved.
//

#include "Router.hpp"

using namespace std;

int main (int argc, char *argv[])
{
    clock_t prog_begin, in_begin, global_route_begin, rip_route_begin, out_begin, prog_end;
    
    prog_begin = clock();
    Router router[num_thread];
    in_begin = clock();
    #pragma omp parallel for num_threads(num_thread)
    for (int i = 0; i < num_thread; ++i)
    {
        router[i].set_id(i);
        router[i].readInput(argv[1]);
        global_route_begin = clock();
        router[i].globalRoute();
    }
    
    int bestRouteIdx{0}, bestRouteOverflow{INF}, bestWirelength{INF};
    for (int i = 0; i < num_thread; ++i)
    {
        if (router[i].overflow() <= bestRouteOverflow)
        {
            if (router[i].wirelength() < bestWirelength)
            {
                bestRouteOverflow = router[i].overflow();
                bestWirelength = router[i].wirelength();
                bestRouteIdx = i;
            }
        }
    }
    if (router[bestRouteIdx].show_log()) 
        std::cout << "Max Overflow: " << router[bestRouteIdx].max_overflow() << ", Total Overflow: " << router[bestRouteIdx].overflow() << ", Wirelength: " << router[bestRouteIdx].wirelength() << ", seed: " << router[bestRouteIdx].seed() << '\n';
    
    rip_route_begin = clock();
    if (bestRouteOverflow > 0) router[bestRouteIdx].rip_reroute();
    out_begin = clock();
    router[bestRouteIdx].dumpResult(argv[2]);

    prog_end = clock();
    if (router[bestRouteIdx].show_log())
    {
        router[bestRouteIdx].drawCongestionMap(argv[2]);
        std::cout << "Constructing Data Structure: " << ((double)(in_begin - prog_begin)/CLOCKS_PER_SEC) << " s\n"
                  << "I/O: " << ((double)(global_route_begin - in_begin + prog_end - out_begin)/CLOCKS_PER_SEC) << " s\n"
                  << "Initial Route: " << ((double)(rip_route_begin - global_route_begin)/CLOCKS_PER_SEC) << " s\n" 
                  << "Rip and Route: " << ((double)(out_begin - rip_route_begin)/CLOCKS_PER_SEC) << " s\n";
    }
    return 0;
}
