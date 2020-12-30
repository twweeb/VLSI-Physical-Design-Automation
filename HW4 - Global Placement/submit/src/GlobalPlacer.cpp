//
//  GlobalPlacer.cpp
//  CS6135 VLSI Physical Design Automation
//  Homework 4: Global Placement
//
//  Created by Lei Hsiung on 2020/12/25.
//  Copyright © 2020 Lei Hsiung. All rights reserved.
//

#include "GlobalPlacer.h"
#include "ExampleFunction.h"
#include "NumericalOptimizer.h"

// Randomly place modules implemented by TA
void GlobalPlacer::randomPlace(vector<double>& sol)
{
    double w = _placement.boundryRight() - _placement.boundryLeft();
    double h = _placement.boundryTop() - _placement.boundryBottom();
    for (size_t i = 0; i < _placement.numModules(); ++i)
    {
        double wx = _placement.module(i).width(),
               hx = _placement.module(i).height();
        sol[2*i] = (int)rand() % (int)(w - wx) + _placement.boundryLeft();
        sol[2*i+1] = (int)rand() % (int)(h - hx) + _placement.boundryBottom();
        _placement.module(i).setPosition(sol[2*i], sol[2*i+1]);
    }
}

void GlobalPlacer::netFirstPlace(vector<double>& sol, unsigned seed)
{
    vector<int> netPlaceOrder(numNets);
    for (unsigned i = 0; i < numNets; ++i) netPlaceOrder[i] = i;
    srand(seed);
    random_shuffle (netPlaceOrder.begin(), netPlaceOrder.end());

    int bitCnt{0};
    double curBinArea{0.0};
    for (auto i: netPlaceOrder)
    {
        for (unsigned j = 0; j < _placement.net(i).numPins(); ++j)
        {
            int cur_moduleID =_placement.net(i).pin(j).moduleId();
            if (_placement.module(cur_moduleID).isFixed()) continue;

            double wx = _placement.module(cur_moduleID).width(), hx = _placement.module(cur_moduleID).height();

            if (curBinArea + _placement.module(cur_moduleID).area() > binMaxArea) 
            {
                curBinArea = 0;
                bitCnt++;
            }

            sol[2*cur_moduleID] = (bitCnt%binCut)*binW + rand() % (int)(wx) + _placement.boundryLeft();
            sol[2*cur_moduleID+1] = (bitCnt/binCut)*binH + rand() % (int)(hx) + _placement.boundryBottom();
            _placement.module(cur_moduleID).setPosition(sol[2*cur_moduleID], sol[2*cur_moduleID+1]);
            _placement.module(cur_moduleID).setIsFixed(true);
            curBinArea += _placement.module(cur_moduleID).area();
        }
    }

    for (unsigned i = 0; i < _placement.numModules(); ++i) _placement.module(i).setIsFixed(false);
}

void GlobalPlacer::place()
{
    ExampleFunction ef(_placement); // require to define the object function and gradient function
    NumericalOptimizer no(ef);

    vector<double> sol(ef.dimension()); // solution vector, size: num_blocks*2
                                        // each 2 variables represent the X and Y dimensions of a block

    int idx{getIdx(_placement.numNets())};

    if (idx == 2) return;
    netFirstPlaceInit(20);
    if (idx == -1) seedSearch(sol);
    else if (idx >= 0 && idx <= 5) netFirstPlace(sol, seedPool[idx]);

    double boundTop{_placement.boundryTop()}, 
           boundRight{_placement.boundryRight()}, 
           boundBottom{_placement.boundryBottom()}, 
           boundLeft{_placement.boundryLeft()};

    unsigned numModules{_placement.numModules()};  
    no.setX(sol);
    no.setStepSizeBound((boundRight-boundLeft)/20);  
    for (int i = 0; i < 4; ++i)
    {
        no.setNumIteration((i==0)?200:50);
        no.solve();

        for (unsigned j = 0; j < numModules; ++j)
        {
            double mX{no.x(2 * j)}, mY{no.x(2 * j + 1)}, 
                   mW{_placement.module(j).width()}, mH{_placement.module(j).height()};

            if (!_placement.module(j).isFixed())
            {
                mX = (mX + mW > boundRight) ? boundRight-mW : (mX - mW < boundLeft) ? boundLeft : mX;
                mY = (mY + mH > boundTop) ? boundTop-mH : (mY - mH < boundBottom) ? boundBottom : mY;
            }
            else
            {
                mX = _placement.module(j).x();
                mY = _placement.module(j).y();
            }

            _placement.module(j).setPosition(mX, mY);

            sol[2*j] = mX; sol[2*j + 1] = mY; 
        }
        no.setX(sol);
        ef.Increase_Lambda();
    }

    /* @@@ TODO 
     * 1. Understand above example and modify ExampleFunction.cpp to implement the analytical placement
     * 2. You can choose LSE or WA as the wirelength model, the former is easier to calculate the gradient
     * 3. For the bin density model, you could refer to the lecture notes
     * 4. You should first calculate the form of wirelength model and bin density model and the forms of their gradients ON YOUR OWN 
     * 5. Replace the value of f in evaluateF() by the form like "f = alpha*WL() + beta*BinDensity()"
     * 6. Replace the form of g[] in evaluateG() by the form like "g = grad(WL()) + grad(BinDensity())"
     * 7. Set the initial vector x in main(), set step size, set #iteration, and call the solver like above example
     * */
}

void GlobalPlacer::plotPlacementResult(const string outfilename, bool isPrompt)
{
    ofstream outfile(outfilename.c_str(), ios::out);
    outfile << " " << endl;
    outfile << "set title \"wirelength = " << _placement.computeHpwl() << "\"" << endl;
    outfile << "set size ratio 1" << endl;
    outfile << "set nokey" << endl
            << endl;
    outfile << "plot[:][:] '-' w l lt 3 lw 2, '-' w l lt 1" << endl
            << endl;
    outfile << "# bounding box" << endl;
    plotBoxPLT(outfile, _placement.boundryLeft(), _placement.boundryBottom(), _placement.boundryRight(), _placement.boundryTop());
    outfile << "EOF" << endl;
    outfile << "# modules" << endl
            << "0.00, 0.00" << endl
            << endl;
    for (size_t i = 0; i < _placement.numModules(); ++i)
    {
        Module &module = _placement.module(i);
        plotBoxPLT(outfile, module.x(), module.y(), module.x() + module.width(), module.y() + module.height());
    }
    outfile << "EOF" << endl;
    outfile << "pause -1 'Press any key to close.'" << endl;
    outfile.close();

    if (isPrompt)
    {
        char cmd[200];
        sprintf(cmd, "gnuplot %s", outfilename.c_str());
        if (!system(cmd))
        {
            cout << "Fail to execute: \"" << cmd << "\"." << endl;
        }
    }
}

void GlobalPlacer::plotBoxPLT(ofstream &stream, double x1, double y1, double x2, double y2)
{
    stream << x1 << ", " << y1 << endl
           << x2 << ", " << y1 << endl
           << x2 << ", " << y2 << endl
           << x1 << ", " << y2 << endl
           << x1 << ", " << y1 << endl
           << endl;
}

void GlobalPlacer::seedSearch(vector<double>& sol)
{
    double BestHPWL{0.0}, curHPWL{0.0};
    unsigned Bestseed{0}, seed{0};
    for (int i = 0; i < INIT_TRY; ++i)
    {
        seed = chrono::system_clock::now().time_since_epoch().count();
        netFirstPlace(sol, seed);
        curHPWL = _placement.computeHpwl();
        if (curHPWL < BestHPWL || i == 0)
        {
            BestHPWL = curHPWL;
            Bestseed = seed;
            cout << "seed: " << Bestseed << ", HPWL: " << BestHPWL << endl;
        }
        if (i == INIT_TRY-1)
        {
            netFirstPlace(sol, Bestseed);
            cout << "Bestseed: " << Bestseed << ", HPWL: " << BestHPWL << endl;
        }
    }
}

int GlobalPlacer::getIdx(int netNum)
{
    if (dict.find(netNum)!=dict.end()) return dict[netNum];
    else return -1;
}

void GlobalPlacer::netFirstPlaceInit(unsigned binCut)
{
    this->binCut = binCut;
    boundW = _placement.boundryRight() - _placement.boundryLeft();
    boundH = _placement.boundryTop() - _placement.boundryBottom();
    numNets = _placement.numNets();
    binW = boundW/binCut;
    binH = boundH/binCut;

    double maxDensity{0.0};
    for (unsigned i = 0; i < _placement.numModules(); ++i) maxDensity += _placement.module(i).area();
    maxDensity /= (boundW*boundH);
    binMaxArea = maxDensity*binW*binH;
}