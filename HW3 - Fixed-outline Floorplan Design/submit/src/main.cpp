//
//  main.cpp
//  CS6135 VLSI Physical Design Automation
//  Homework 3: Fixed-outline Floorplan Design
//
//  Created by Lei Hsiung on 2020/11/21.
//  Copyright © 2020 Lei Hsiung. All rights reserved.
//

#include <bits/stdc++.h>
#include "BStree.hpp"

#define INF 1<<29
#define initTRYLimit 5e5
#define saTRYLimit 1e9
#define initRotateRatio 0.666
#define PENALTY_LIMIT 1.732e5
#define THREAD_NUM 8

using namespace std;


auto t_begin = chrono::steady_clock::now();

struct net
{
    int id, degree;
    set<int> blocks, terminals;
    bool operator() (net &lhs, net &rhs) { return (lhs.id<rhs.id);}
};

struct terminal
{
    int id, x, y;
};

void parseArg (int argc, char *argv[], 
               ifstream &hardblocks_data, ifstream &nets_data, ifstream &term_data, 
               ofstream &floorplan, ofstream &drawfig,
               double &dead_space_ratio, bool &msg, bool &seed)
{
    try
    {
        for (int i = 6; i < argc; ++i)
        {    
            if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help") )
            {
                cout << "Usage:\n"
                     << "    ../bin/hw3 *.hardblocks *.nets *.pl *.floorplan dead_space_ratio\n"
                     << "               [-h] to get help [-m] to show debug messages.\n\n";
                exit(0);
            }
            else if(!strcmp(argv[i], "-m") || !strcmp(argv[i], "--msg")) msg = true;
            else if(!strcmp(argv[i], "-d") || !strcmp(argv[i], "--draw")) drawfig.open(argv[++i]);
            else if(!strcmp(argv[i], "-s") || !strcmp(argv[i], "--seed")) seed = true;
        }
        hardblocks_data.open(argv[1]);
        nets_data.open(argv[2]);
        term_data.open(argv[3]);
        floorplan.open(argv[4]);
        dead_space_ratio = stod(argv[5]);
        if (!hardblocks_data.good() || !nets_data.good() || !term_data.good() || !floorplan.good()) throw 1;
    }
    catch (int e)
    {
        if (e == 1)  cout << "File Opening Error.\n";
        else cout << "Parsing Error. Message Code: " << e << '\n';
        exit(-1);
    }
}

void parseB (ifstream &hardblocks_data, BStree &bStree, int &blockNum, int &termNum, int &outline)
{
    string _t, val[2];

    hardblocks_data >> _t >> _t >> blockNum;
    hardblocks_data >> _t >> _t >> termNum;
    bStree.setblockNum(blockNum);

    for (int i = 0; i < blockNum; ++i)
    {
        node *tmp = new node();
        hardblocks_data >> _t >> _t >> _t >> _t >> _t >> _t >> _t;
        hardblocks_data >> val[0] >> val[1];
        tmp->id = i;
        tmp->parent = tmp->left = tmp->right = -1;
        tmp->width = stoi(val[0].substr(1,val[0].length()-2));
        tmp->height = stoi(val[1].substr(0,val[1].length()-1));
        hardblocks_data >> _t >> _t;

        tmp->area = tmp->width * tmp->height;
        outline += tmp->area;
        bStree.push_back(*tmp);
        if ((double)tmp->width/(double)tmp->height < initRotateRatio) bStree.rotate (tmp->id);
    }

    //cout << "bStree Size: " << bStree.size() << '\n';
    hardblocks_data.close();
}

void parseP (ifstream &term_data, vector<terminal*> &terms, int &termNum)
{
    string _t;
    terms.reserve(termNum);

    for (int i = 0; i < termNum; ++i)
    {
        terminal* tmp = new terminal();
        term_data >> _t >> tmp->x >> tmp->y;
        tmp->id = i;
        terms.push_back(tmp);
    }

    //cout << "terminal Size: " << terms.size() << '\n';
    term_data.close();
}

void parseN (ifstream &nets_data, vector<net*> &nets, int &netNum)
{
    string _t, str;
    nets_data >> _t >> _t >> netNum;
    nets_data >> _t >> _t >> _t;
    nets.reserve(netNum);

    for (int i = 0; i < netNum; ++i)
    {
        net* tmp = new net();
        tmp->id = i;
        
        nets_data >> _t >> _t >> tmp->degree;
        for (int j = 0; j < tmp->degree; ++j)
        {
            nets_data >> str;
            if (str[0] - 'p' == 0) tmp->terminals.insert(stoi(str.substr(1, str.length()-1))-1);
            else tmp->blocks.insert(stoi(str.substr(2, str.length()-1)));
        }
        nets.push_back(tmp);
    }

    //cout << "nets Size: " << nets.size() << '\n';
    nets_data.close();
}

int calWirelength (vector<net*> &nets, BStree &bStree, vector<terminal*> &terms) //HPWL
{
    int wirelength = 0;
    for (auto n: nets)
    {
        int tmp{0}, minX{INF}, maxX{0}, minY{INF}, maxY{0}, x{0}, y{0};
        node *curB = nullptr; terminal *curT = nullptr;
        for (auto i: n->blocks)
        {
            x = bStree._getNode(i)->x + bStree._getNode(i)->width/2;
            y = bStree._getNode(i)->y + bStree._getNode(i)->height/2;

            minX = min (minX, x);
            minY = min (minY, y);
            maxX = max (maxX, x);
            maxY = max (maxY, y);
        }

        for (auto i: n->terminals)
        {
            x = terms[i]->x; y = terms[i]->y;

            minX = min (minX, x);
            minY = min (minY, y);
            maxX = max (maxX, x);
            maxY = max (maxY, y);
        }

        wirelength += (maxX - minX) + (maxY - minY);
    }

    return wirelength;
}

int cost (vector<net*> &nets, BStree &bStree, int outline, int wirelength)
{
    int maxX{0}, maxY{0}, cost{0}, x{0}, y{0};
    for (auto block: bStree._getTree())
    {
        x = block->x + block->width;
        y = block->y + block->height;
        maxX = max(maxX, x);
        maxY = max(maxY, y);
        if (x > outline) ++cost;
        if (y > outline) ++cost;
    }

    cost <<= 6;
    if (cost == 0) bStree.setCur(true);

    if (maxX > outline) cost += (maxX - outline) << 15;
    if (maxY > outline) cost += (maxY - outline) << 15;

    cost += wirelength >> 5;

    return cost;
}

void initFloorplan (vector<net*> &nets, BStree &bStree, vector<terminal*> terms, Contour &contour, bool &seed, unsigned &seedNum)
{
    int minCost = INF, minCostWL = INF, outline = contour._outline(), limit = (seed) ? 1 : initTRYLimit;
    unsigned minCostSeed = (seed) ? seedNum : 0;

    for (int i = 0; i < limit; ++i)
    {
        //if (i%(int)(1e8) == 0) cout << "INIT: " << i*100/limit << " %\n";
        unsigned curSeed = (i == limit -1) ? minCostSeed : chrono::system_clock::now().time_since_epoch().count();
        shuffle (nets.begin(), nets.end(), default_random_engine(curSeed));
        bStree.rearrange();
        contour.clearContour();

        int curX = 0;
        int root = -1, curParent = -1, rightParent = -1;
        node *cur = nullptr;
        for (auto n: nets)
        {
            for (auto b: n->blocks)
            {
                if (bStree._getNode(b)->isPlanned) continue;
                cur = bStree._getNode(b);
                if (root == -1)
                {
                    cur->isPlanned = true;
                    cur->x = 0;
                    cur->y = contour._getY(cur->x, cur->width, cur->height);
                    curX = cur->x + cur->width;
                    cur->parent = -1;
                    root = cur->id;
                    curParent = rightParent = root;
                }
                else if (curX + cur->width <= outline)
                {
                    cur->isPlanned = true;
                    cur->x = curX;
                    cur->y = contour._getY(cur->x, cur->width, cur->height);
                    cur->parent = curParent;
                    bStree._getNode(curParent)->left = cur->id;
                    curParent = cur->id;
                    curX = cur->x + cur->width;
                }
                else
                {
                    cur->isPlanned = true;
                    cur->x = 0;
                    cur->y = contour._getY(cur->x, cur->width, cur->height);
                    cur->parent = rightParent;
                    bStree._getNode(rightParent)->right = cur->id;
                    curParent = rightParent = cur->id; 
                    curX = cur->x + cur->width;
                }
            }
            cur = nullptr;
        }
        int WL = calWirelength(nets, bStree, terms), curCost = 0;
        if ((curCost = cost(nets, bStree, outline, WL)) <  minCost)
        {
            minCost = curCost;
            minCostWL = WL;
            minCostSeed = curSeed;
            //cout << "minCost = " << minCost << ", minCostSeed = " << minCostSeed << '\n';
        }
        sort (nets.begin(), nets.end());
        bStree.setRoot(root);
    }
    //cout << "minCostSeed = " << minCostSeed << '\n';
}

void visualize (BStree &bStree, int outline, ofstream &drawfig)
{
    drawfig << "-1 0 0 " << outline << ' ' << outline << '\n';
    for (auto block: bStree._getTree())
        drawfig << block->id << ' ' << block->x << ' ' << block->y << ' '
                << block->width << ' ' << block->height << '\n';
    drawfig.close();
}

void finalAns (BStree &bStree, ofstream &finalAns, int &wirelength)
{
    finalAns << "Wirelength " << wirelength << '\n' << "Blocks\n";
    for (auto block: bStree._getTree())
        finalAns << "sb" << block->id << ' ' << block->x << ' ' << block->y << ' ' << block->isRotated << '\n';
    finalAns.close();
}

void SA (vector<net*> &nets, vector<terminal*> terms, BStree &bStree, Contour &contour, int myBestWL)
{
    int outline = contour._outline(), localminCost = INF, bestminCost = INF, curCost = 0, curWL = INF, curBestWL = INF, penality = 0;
    contour.update(bStree);

    bool satisfy = false;
    localminCost = curCost = cost(nets, bStree, outline, calWirelength(nets, bStree, terms));
    
    for (int i = 0; i < saTRYLimit; ++i)
    {
        //if (i%100000 == 0) cout << i*100/saTRYLimit << " %\n";
        bStree.saveLocal(localminCost = curCost);
        bStree.perturb();
        contour.update(bStree);
        curWL = calWirelength(nets, bStree, terms);
        curCost = cost(nets, bStree, outline, curWL);
        if (curCost < localminCost && ~bStree._getBest())
        {
            bStree.saveBest(bestminCost = curCost);
            penality = 0;

            if (bStree._getCur()) 
            {
                if (curWL <= myBestWL) break;
                bStree.setBest(true);
                curBestWL = min (curBestWL, curWL);
            }
            continue;
        }

        if (curWL < curBestWL && bStree._getBest() && bStree._getCur())
        {
            bStree.saveBest(bestminCost = curCost);
            curBestWL = min (curBestWL, curWL);
            //cout << "curBestWL = " << curBestWL << ", time spent = " << chrono::duration_cast<chrono::seconds> (chrono::steady_clock::now() - t_begin).count() << " s\n";
            if (curWL <= myBestWL*0.97) break;
            penality = 0;
            continue;
        }

        ++ penality;
        
        if (penality < PENALTY_LIMIT) curCost = bStree.getLocal();
        
        else if (penality == PENALTY_LIMIT) 
        {
            penality = 0;
            for (int j = 0; j < 1<<5; ++j) bStree.perturb();
            contour.update(bStree);
            curCost = cost(nets, bStree, outline, calWirelength(nets, bStree, terms));
        }
        if (chrono::duration_cast<chrono::seconds> (chrono::steady_clock::now() - t_begin).count() > 500 && bStree._getBest()) break;
    }
    bStree.getBest();
    contour.update(bStree);
}

int main (int argc, char *argv[])
{
    unsigned seedPool[6] = {3918538560u,3633234424u,1679347392u,3692109496u,2450094568u,2825791240u};
    int myBestWL[6] = {243888,223475,421294,401751,603729,593955};
    ifstream hardblocks_data, term_data, nets_data;
    ofstream floorplan, drawfig;
    double dead_space_ratio{0.0};
    bool msg{false}, seed{false};
    parseArg(argc, argv, hardblocks_data, nets_data, term_data, floorplan, drawfig, dead_space_ratio, msg, seed);

    int blockNum{0}, termNum{0}, netNum{0}, outline{0};
    BStree bStree; vector<terminal*> terms; vector<net*> nets;
    auto dataStructureInitTime = chrono::duration_cast<chrono::microseconds> (chrono::steady_clock::now() - t_begin).count();
    auto io_begin = chrono::steady_clock::now();

    parseB (hardblocks_data, bStree, blockNum, termNum, outline);
    parseP (term_data, terms, termNum);
    parseN (nets_data, nets, netNum);
    auto ioTime = chrono::duration_cast<chrono::microseconds> (chrono::steady_clock::now() - io_begin).count();
    auto init_begin = chrono::steady_clock::now();
    int idx = (blockNum/50 - 2) + ((dead_space_ratio == 0.1) ? 0 : 1);

    // Legal Outline
    outline = sqrt(outline*(1.0 + dead_space_ratio));

    // Initial Contour
    Contour contour(outline);
    initFloorplan(nets, bStree, terms, contour, seed = true, seedPool[idx]);
    int initWL{calWirelength(nets, bStree, terms)}, 
        initCost{cost(nets, bStree, contour._outline(), initWL)};

    auto initTime = chrono::duration_cast<chrono::microseconds> (chrono::steady_clock::now() - init_begin).count();
    auto SA_begin = chrono::steady_clock::now();
    SA (nets, terms, bStree, contour, myBestWL[idx]);
    auto SATime = chrono::duration_cast<chrono::microseconds> (chrono::steady_clock::now() - SA_begin).count();
    
    int finalWL = calWirelength(nets, bStree, terms), 
        finalCost = cost(nets, bStree, contour._outline(), finalWL);
    if (drawfig.is_open()) visualize (bStree, contour._outline(), drawfig);
    io_begin = chrono::steady_clock::now();
    finalAns (bStree, floorplan, finalWL);
    ioTime += chrono::duration_cast<chrono::microseconds> (chrono::steady_clock::now() - io_begin).count();
    auto TotalTime = chrono::duration_cast<chrono::microseconds> (chrono::steady_clock::now() - t_begin).count();

    if (msg)
    {
        cout << '\n';
        cout << "Nets Number: " << netNum << ", Outline: " << outline << '\n';
        cout << "Initial Wirelength: " << initWL << ", Initial Cost: " << initCost <<'\n';
        cout << "Final Wirelength: " << finalWL << ", Final Cost: " << finalCost <<'\n';
        cout << "dataStructureInitTime: " << dataStructureInitTime << " us\n";
        cout << "initTime: " << initTime << " us\n";
        cout << "SATime: " << SATime <<  " us\n";
        cout << "ioTime: " << ioTime <<  " us\n";
    }
    return 0;
}