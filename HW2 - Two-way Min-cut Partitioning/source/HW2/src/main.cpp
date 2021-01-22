//
//  main.cpp
//  CS6135 VLSI Physical Design Automation
//  Homework 2: Two-way Min-cut Partitioning
//
//  Created by Lei Hsiung on 2020/10/24.
//  Copyright © 2020 Lei Hsiung. All rights reserved.
//

#include <bits/stdc++.h>

using namespace std;
#define RATIO 0.1

struct net{
    int id, cellsNum, cellsInA, cellsInB;
    bool lastMove = true; // true: from A to B, false: from B to A
    set<int> connectCells;
};

struct cell{
    int id, size, gain, pin;
    bool isA, isLock;
    set<int> connectNets;
};

map<int, int> cellIdxtoReal, cellRealtoIdx;

auto t_begin = chrono::steady_clock::now(), t_end = chrono::steady_clock::now();

int cellTotalSize = 0;
bool isBalance (int size, bool init) { return (init) ? size < cellTotalSize*(0.5+RATIO/2) : abs(cellTotalSize - 2*size) < cellTotalSize * RATIO;}
void parseArg(int argc, char *argv[], ifstream &in_cell, ifstream &in_net, ofstream &final_cut, bool &msg)
{
    for(int i = 1; i < argc; i++)
    {    
        if(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help") )
        {
            cout << "Usage:\n";
            cout << "    ../bin/hw2 [-n nets_file] [-c cells_file] [-o result_destination]\n";
            cout << "               [-h] to get help [-m] to show cut-change and time-info.\n\n";
            exit(0);
        }
        else if(!strcmp(argv[i], "-m") || !strcmp(argv[i], "--msg"))
        {
			msg = true;
        }
        else if(!strcmp(argv[i], "-n") || !strcmp(argv[i], "--net"))
        {
			in_net.open(argv[++i]);
            if (!in_net.is_open())
            {
			    cout << "Net file open error" << '\n';
                exit(0);
            }
			//else cout << argv[i] << " has been successfully read." << '\n';
        }
        else if(!strcmp(argv[i], "-c") || !strcmp(argv[i], "--cell"))
        {
			in_cell.open(argv[++i]);
            if (!in_cell.is_open())
            {
			    cout << "Cell file open error" << '\n';
			    exit(0);
            }
			//else cout << argv[i] << " has been successfully read." << '\n';
        }
        else if(!strcmp(argv[i], "-o") || !strcmp(argv[i], "--out"))
        {
			final_cut.open(argv[++i]);
            if (!final_cut.is_open())
            {
			    cout << "Does not specify or can not save the result." << '\n';
			    exit(0);
            }
        }

        else if(i == argc-1) break;
		else
		{
			cout << "Error" << '\n';
			exit(0);
		}
    }
}

void readNetandCell(ifstream &in_cell, ifstream &in_net, vector<net*> &Nets, vector<cell*> &Cells,
                    int &cellTotalSize, int &maxPin, int &maxSize)
{
    string tmp;
    net *n = nullptr;
    cell *c = nullptr;
    int id = 0;
    cellTotalSize = 0;
    while (in_cell >> tmp)
    {
        if (tmp[0] - 'c' == 0) 
        {
            c = new cell();
            c->id = stoi(tmp.substr(1,tmp.length()-1)); 
            c->gain = 0;
            c->pin = 0;
        }
        else
        {
            c->size = stoi(tmp);
            cellTotalSize += stoi(tmp);
            maxSize = max(maxSize, stoi(tmp));
            c->isLock = false;
            Cells.push_back(c);
            c = nullptr;
        }
    }

    //sort(Cells.begin(), Cells.end(), comparBySize);

    for (int i = 0; i < Cells.size(); ++i)
    {
        cellIdxtoReal[i] = Cells[i]->id;
        cellRealtoIdx[Cells[i]->id] = i;
    }
    while (in_net >> tmp)
    {
        if (tmp[0] - 'N' == 0) 
        {
            n = new net();
            n->cellsInA = 0;
            n->cellsInB = 0;
            n->cellsNum = 0;
        }
        else if (tmp[0] - 'n' == 0) n->id = stoi(tmp.substr(1,tmp.length()-1))-1;
        else if (tmp[0] - 'c' == 0) 
        {
            id = cellRealtoIdx[stoi(tmp.substr(1,tmp.length()-1))];
            n->connectCells.insert(id);
            Cells[id]->connectNets.insert(n->id);
            Cells[id]->pin++;
            n->cellsNum++;
            maxPin = max(maxPin, Cells[id]->pin);
        }
        else if (tmp[0] - '}' == 0)
        {
            Nets.push_back(n);
            n = nullptr;
        }
    }
    in_cell.close(); in_net.close();
}

tuple<int, int, int, int> initPartition(vector<net*> &Nets, vector<cell*> &Cells, int cellsCnt, set<int> &A, set<int> &B)
{
    int ACnt = 0, ASize = 0, BCnt = 0, BSize = 0;

    // Net first
    for (auto n: Nets)
    {
        if (n->cellsInA + n->cellsInB == n->cellsNum) continue;

        for (auto c: n->connectCells)
        {
            if (A.find(c)!=A.end() || B.find(c)!=B.end()) continue;
            if (isBalance(ASize + Cells[c]->size, true))
            {
                Cells[c]->isA = true;
                for (auto i: Cells[c]->connectNets) Nets[i]->cellsInA++;
                A.insert(c);
                ACnt++;
                ASize += Cells[c]->size;
                
            }
            else
            {
                Cells[c]->isA = false;
                for (auto i: Cells[c]->connectNets) Nets[i]->cellsInB++;
                B.insert(c);
                BCnt++;
                BSize += Cells[c]->size;
            }
        }
    }
    /*
    // Large Cell first
    for (auto i = 0; i < cellsCnt; ++i)
    {
        if (isBalance(ASize + Cells[i]->size, true))
        {
            Cells[i]->isA = true;
            for (auto p: Cells[i]->connectNets) Nets[p]->cellsInA++;
            A.insert(i);
            ACnt++;
            ASize += Cells[i]->size;
        }
        else
        {
            Cells[i]->isA = false;
            B.insert(i);
            BCnt++;
            BSize += Cells[i]->size;
        }
    }
    */
    return tie(ACnt, ASize, BCnt, BSize);
}

int getCutSize(vector<net*> &Nets)
{
    int cutsize = 0;
    for (auto n: Nets)
    {
        //cout << "n: "<< n->id << ", a: " << n->cellsInA << ", b: " << n->cellsInB << endl;
        if (n->cellsInA>0 && n->cellsInB>0) cutsize++;
    }
    return cutsize;
}

void savefile(ofstream &final_cut, set<int> &A, set<int> &B, int cutsize)
{
    final_cut << "cut_size " << cutsize << '\n';
    final_cut << "A " << A.size() << '\n';
    for (auto i:A) final_cut << 'c' << cellIdxtoReal[i] << '\n';
    final_cut << "B " << B.size() << '\n';
    for (auto i:B) final_cut << 'c' << cellIdxtoReal[i] << '\n';
    final_cut.close();
}

void initGain(vector<net*> &Nets, vector<cell*> &Cells)
{
    for (auto n: Nets)
    {
        if (n->cellsInA == 0 || n->cellsInB == 0)
        {
            for (auto c: n->connectCells) Cells[c]->gain--;
            continue;
        }
        if (n->cellsInA == 1)
        {
            for (auto c: n->connectCells) // Optimize
            {
                if (Cells[c]->isA) 
                {
                    Cells[c]->gain++;
                    break;
                }
            }
        }
        if (n->cellsInB == 1)
        {
            for (auto c: n->connectCells)
            {
                if (!Cells[c]->isA) 
                {
                    Cells[c]->gain++;
                    break;
                }
            }
        }
    }
}

void genGainList(vector<cell*> Cells, set<int> &A, set<int> &B, 
                 map<int, set<int>> &gainA, map<int, set<int>> &gainB)
{
    int gain = 0;

    // insert gainList
    for (auto i: A)
    {
        if (Cells[i]->isLock) continue;
        auto it = gainA.find(gain = Cells[i]->gain);
        set<int> *tmp = nullptr;
        if (it == gainA.end())
        {
            tmp = new set<int>();
            gainA[gain] = *tmp;
            it = gainA.find(gain);
        }
        it->second.insert(i);
    }
    for (auto i: B)
    {
        if (Cells[i]->isLock) continue;
        auto it = gainB.find(gain = Cells[i]->gain);
        set<int> *tmp = nullptr;
        if (it == gainB.end())
        {
            tmp = new set<int>();
            gainB[gain] = *tmp;
            it = gainB.find(gain);
        }
        it->second.insert(i);
    }
}

int getOnly(set<int> &A, set<int> &B)
{
    if (A.size() <= B.size())
    {
        for (auto i:A)
        {
            if (B.find(i)!=B.end()) return i;
        }
    }
    else
    {
        for (auto i:B)
        {
            if (A.find(i)!=A.end()) return i;
        }
    }
    return 0;
}

void updateGain(cell *c, const bool &fromAtoB, const bool &isMoved,
                vector<net*> &Nets, vector<cell*> &Cells, 
                set<int> &A, set<int> &B,
                map<int, set<int>> &gainA,
                map<int, set<int>> &gainB)
{
    set<int> affectCell;
    for (auto n: Cells[cellRealtoIdx[c->id]]->connectNets)
    {
        net* curNet = Nets[n];
        cell* curCell = nullptr;
        int T = (fromAtoB) ? curNet->cellsInB : curNet->cellsInA, 
            F = (fromAtoB) ? curNet->cellsInA : curNet->cellsInB,
            id = 0;
        if (!isMoved && T == 0) 
        {
            for (auto c: curNet->connectCells)
            {
                curCell = Cells[c];
                if (curCell->isLock) continue;
                
                if (affectCell.find(c) == affectCell.end())
                {
                    if (curCell->isA) 
                    {
                        gainA[curCell->gain].erase(c);
                        if (gainA[curCell->gain].empty()) gainA.erase(curCell->gain);
                    }
                    else 
                    {
                        gainB[curCell->gain].erase(c);
                        if (gainB[curCell->gain].empty()) gainB.erase(curCell->gain);
                    }
                    affectCell.insert(c);
                }
                curCell->gain++;
            }
        }
        else if (!isMoved && T == 1)
        {
            curCell = (fromAtoB) ? Cells[getOnly(B, curNet->connectCells)]:Cells[getOnly(A, curNet->connectCells)];
            id = cellRealtoIdx[curCell->id];
            if (!curCell->isLock) 
            {
                if (affectCell.find(id) == affectCell.end())
                {
                    if (curCell->isA) 
                    {
                        gainA[curCell->gain].erase(id);
                        if (gainA[curCell->gain].empty()) gainA.erase(curCell->gain);
                    }
                    else 
                    {
                        gainB[curCell->gain].erase(id);
                        if (gainB[curCell->gain].empty()) gainB.erase(curCell->gain);
                    }
                    affectCell.insert(id);
                }
                curCell->gain--;
            }
        } 
        else if (isMoved && F == 0)
        {
            for (auto c: curNet->connectCells)
            {
                curCell = Cells[c];
                if (curCell->isLock) continue;
                
                if (affectCell.find(c) == affectCell.end())
                {
                    if (curCell->isA) 
                    {
                        gainA[curCell->gain].erase(c);
                        if (gainA[curCell->gain].empty()) gainA.erase(curCell->gain);
                    }
                    else 
                    {
                        gainB[curCell->gain].erase(c);
                        if (gainB[curCell->gain].empty()) gainB.erase(curCell->gain);
                    }
                    affectCell.insert(c);
                }
                Cells[c]->gain--;
            }
        } 
        else if (isMoved && F == 1)
        {
            curCell = (fromAtoB) ? Cells[getOnly(A, curNet->connectCells)]:Cells[getOnly(B, curNet->connectCells)];
            id = cellRealtoIdx[curCell->id];
            if (!curCell->isLock) 
            {
                if (affectCell.find(id) == affectCell.end())
                {
                    if (curCell->isA) 
                    {
                        gainA[curCell->gain].erase(id);
                        if (gainA[curCell->gain].empty()) gainA.erase(curCell->gain);
                    }
                    else 
                    {
                        gainB[curCell->gain].erase(id);
                        if (gainB[curCell->gain].empty()) gainB.erase(curCell->gain);
                    }
                    affectCell.insert(id);
                }
                curCell->gain ++;
            }
        }
    }
    for (auto c: affectCell)
    {
        cell *curCell = Cells[c];
        if (curCell->isA) gainA[curCell->gain].insert(cellRealtoIdx[curCell->id]);
        else gainB[curCell->gain].insert(cellRealtoIdx[curCell->id]);
    }
    affectCell.clear();
}

void move(cell* c, bool &fromAtoB, vector<net*> Nets, 
          set<int> &A, set<int> &B, int &ASize, int &BSize,
          map<int, set<int>> &gainA,
          map<int, set<int>> &gainB)
{
    int id = cellRealtoIdx[c->id];
    if (fromAtoB)
    {
        A.erase(id); B.insert(id);
        c->isA = false;
        gainA[c->gain].erase(id);
        ASize -= c->size; BSize += c->size;
        if (gainA[c->gain].empty()) gainA.erase(c->gain);
        for (auto n: c->connectNets)
        {
            Nets[n]->cellsInA -= 1;
            Nets[n]->cellsInB += 1;
        }
    }
    else
    {
        B.erase(id); A.insert(id);
        c->isA = true;
        gainB[c->gain].erase(id);
        BSize -= c->size; ASize += c->size;
        if (gainB[c->gain].empty()) gainB.erase(c->gain);
        for (auto n: c->connectNets)
        {
            Nets[n]->cellsInA += 1;
            Nets[n]->cellsInB -= 1;
        }
    }
}

cell* getMaxGain (vector<cell*> &Cells, map<int, set<int>> &gainA, map<int, set<int>> &gainB, 
                  int &ASize, int &BSize)
{
    map<int, set<int>>::reverse_iterator maxGain, maxGainA, maxGainB;
    if (gainA.empty() && gainB.empty()) return nullptr;
    else if (gainA.empty()) 
    {
        maxGain = gainB.rbegin();
        if (!isBalance(ASize + Cells[*(maxGain->second.begin())]->size, false)) return nullptr;
    }
    else if (gainB.empty())
    {
        maxGain = gainA.rbegin();
        if (!isBalance(BSize + Cells[*(maxGain->second.begin())]->size, false)) return nullptr;
    } 
    else
    {
        maxGainA = gainA.rbegin(); maxGainB = gainB.rbegin();
        if (maxGainA->first >= maxGainB->first)
        {
            if (isBalance(BSize + Cells[*(maxGainA->second.begin())]->size, false)) maxGain = maxGainA;
            else if (isBalance(ASize + Cells[*(maxGainB->second.begin())]->size, false)) maxGain = maxGainB;
            else return nullptr;
        }
        else
        {
            if (isBalance(ASize + Cells[*(maxGainB->second.begin())]->size, false)) maxGain = maxGainB;
            else if (isBalance(BSize + Cells[*(maxGainA->second.begin())]->size, false)) maxGain = maxGainA;
            else return nullptr;
        }
    }
    return Cells[*(maxGain->second.begin())];
}

int fm(vector<net*> &Nets, vector<cell*> &Cells, set<int> &A, set<int> &B, 
        map<int, set<int>> &gainA, map<int, set<int>> &gainB, 
        int &ASize, int &BSize)
{
    int curPartialSum = 0, greatestPartialSum = 0, greatestPartialSumStep = 0;
    bool fromAtoB = false;
    stack<cell*> movedOrder;
    cell* movedCell = getMaxGain(Cells, gainA, gainB, ASize, BSize);
    while (movedCell != nullptr)
    {
        movedOrder.push(movedCell);
        fromAtoB = movedCell->isA;
        movedCell->isLock = true;
        curPartialSum += movedCell->gain;
        updateGain (movedCell, fromAtoB, false, Nets, Cells, A, B, gainA, gainB);
        move(movedCell, fromAtoB, Nets, A, B, ASize, BSize, gainA, gainB);
        updateGain (movedCell, fromAtoB, true, Nets, Cells, A, B,  gainA, gainB);

        movedCell = getMaxGain(Cells, gainA, gainB, ASize, BSize);
        if (greatestPartialSum < curPartialSum)
        {
            greatestPartialSum = curPartialSum;
            greatestPartialSumStep = movedOrder.size();
        }
        if (chrono::duration_cast<chrono::seconds> (chrono::steady_clock::now() - t_begin).count() > 570) break;
    }
    auto reverseStep = movedOrder.size() - greatestPartialSumStep;
    for (auto i = 0; i < reverseStep; ++i)
    {
        movedCell = movedOrder.top();
        movedOrder.pop();
        movedCell->isLock = false;
        fromAtoB = movedCell->isA;
        updateGain (movedCell, fromAtoB, false, Nets, Cells, A, B, gainA, gainB);
        move(movedCell, fromAtoB, Nets, A, B, ASize, BSize, gainA, gainB);
        updateGain (movedCell, fromAtoB, true, Nets, Cells, A, B,  gainA, gainB);
    }
    return greatestPartialSum;
}

int main(int argc, char *argv[])
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL); cout.tie(NULL);

    t_begin = chrono::steady_clock::now();
    ifstream in_cell, in_net;
    ofstream final_cut;
    bool msg = false;
	parseArg(argc, argv, in_cell, in_net, final_cut, msg);
    int maxPin = 0, maxSize = 0;
    vector<net*> Nets; vector<cell*> Cells;
	readNetandCell(in_cell, in_net, Nets, Cells, cellTotalSize, maxPin, maxSize);
    t_end = chrono::steady_clock::now();

    auto io_time = chrono::duration_cast<chrono::microseconds>(t_end - t_begin).count();

    t_begin = chrono::steady_clock::now();
    int ACnt = 0, ASize = 0, BCnt = 0, BSize = 0;
    set<int> A, B;
    tie(ACnt, ASize, BCnt, BSize) = initPartition(Nets, Cells, Cells.size(), A, B);
    
    int initcutSize = 0, cutSize = 0, iteration_sum = 0;
    if (msg) initcutSize = getCutSize(Nets);

    map<int, set<int>> gainA, gainB;
    do{
        initGain(Nets, Cells);
        gainA.clear(); gainB.clear();
        genGainList(Cells, A, B, gainA, gainB);

        iteration_sum = fm(Nets, Cells, A, B, gainA, gainB, ASize, BSize);
    } while (iteration_sum > 0);

    t_end = chrono::steady_clock::now();
    auto computation_time = chrono::duration_cast<chrono::microseconds>(t_end - t_begin).count();
    
    cutSize = getCutSize(Nets);
    t_begin = chrono::steady_clock::now();
    savefile(final_cut, A, B, cutSize);
    t_end = chrono::steady_clock::now();
    
    io_time += chrono::duration_cast<chrono::microseconds>(t_end - t_begin).count();
    
    if (msg)
    {
        cout << "------------------ Result ------------------\n";
        cout << "  Initial CutSize: " << initcutSize << '\n';
        cout << "  Final CutSize: " << cutSize << '\n';
        cout << "  I/O time: " << io_time << " microseconds\n";
        cout << "  Computation time: " << computation_time << " microseconds\n";
        cout << "  Execution time: " << io_time+computation_time << " microseconds\n";
        cout << "--------------------------------------------\n";
    }
 
    return 0;
}