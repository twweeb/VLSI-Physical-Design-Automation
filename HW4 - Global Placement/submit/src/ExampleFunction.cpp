//
//  ExampleFunction.cpp
//  CS6135 VLSI Physical Design Automation
//  Homework 4: Global Placement
//
//  Created by Lei Hsiung on 2020/12/25.
//  Copyright © 2020 Lei Hsiung. All rights reserved.
//
#include <cmath>
#include <cstdlib>
#include <cstring>
#include "ExampleFunction.h"

ExampleFunction::ExampleFunction(Placement &placement)
    :_placement(placement)
{
    boundW = _placement.boundryRight() - _placement.boundryLeft();
    boundH = _placement.boundryTop() - _placement.boundryBottom();
    numModules = _placement.numModules();

    lambda = 0;
    eta = 500;
    binCut = 10;

    binW = boundW / binCut;
    binH = boundH / binCut;
    grad = new double[numModules * 2]();
    xExp = new double[numModules * 4]();
    binTotalNum = binCut * binCut;
    binArea = binW * binH;
    binDensity = new double[binTotalNum]();

    avgDensity = 0.0;
    for(unsigned i = 0; i < numModules; ++i) avgDensity += _placement.module(i).area();
    avgDensity /= (boundW * boundH);
}

void ExampleFunction::evaluateFG(const vector<double> &x, double &f, vector<double> &g)
{
    fill(g.begin(), g.end(), 0.0);
    f = 0.0;

    // LSE wirelength
    for (unsigned i = 0; i < numModules; ++i)
    {
        xExp[4*i] = exp(x[2*i] / eta);
        xExp[4*i+1] = exp(-x[2*i] / eta);
        xExp[4*i+2] = exp(x[2*i+1] / eta);
        xExp[4*i+3] = exp(-x[2*i+1] / eta);
    }
    
    for (unsigned i = 0; i < _placement.numNets(); ++i)
    {
        double sumX1{0.0}, sumX2{0.0}, sumY1{0.0}, sumY2{0.0};
        for (unsigned j = 0; j < _placement.net(i).numPins(); ++j)
        {
            int mID =_placement.net(i).pin(j).moduleId();
            sumX1 += xExp[4*mID];
            sumX2 += xExp[4*mID+1];
            sumY1 += xExp[4*mID+2];
            sumY2 += xExp[4*mID+3];
        }

        f += eta*(log(sumX1) + log(sumX2) + log(sumY1) + log(sumY2));

        for (unsigned j = 0; j < _placement.net(i).numPins(); ++j)
        {
            int mID =_placement.net(i).pin(j).moduleId();
            if (!_placement.module(mID).isFixed())
            {
                g[2*mID] += xExp[4*mID] / (eta*sumX1);
                g[2*mID] -= xExp[4*mID+1] / (eta*sumX2);
                g[2*mID+1] += xExp[4*mID+2] / (eta*sumY1);
                g[2*mID+1] -= xExp[4*mID+3] / (eta*sumY2);
            }
        }    
    }

    if (lambda==0) return;
    
    // Bin Density
    memset(binDensity, 0.0, sizeof(double)*binTotalNum);
    memset(grad, 0.0, sizeof(double)*binTotalNum);
    double mW{0.0}, mH{0.0}, c{0.0};
    double thetaX{0.0}, thetaY{0.0}, dX{0.0}, dY{0.0}, ABSdX{0.0}, ABSdY{0.0}, aX{0.0}, bX{0.0}, aY{0.0}, bY{0.0};
    for (int a = 0; a < binCut; ++a)
    {
        for (int b = 0; b < binCut; ++b)
        {
            for (unsigned i = 0; i < numModules; ++i)
            {
                mW = _placement.module(i).width(); mH = _placement.module(i).height();
                if (!_placement.module(i).isFixed())
                {
                    c = _placement.module(i).area()/binArea;
                    
                    dX = _placement.module(i).centerX() - (((double)a+0.5)*binW + _placement.boundryLeft());
                    ABSdX = abs(dX);
                    dY = _placement.module(i).centerY() - (((double)b+0.5)*binH + _placement.boundryBottom());
                    ABSdY = abs(dY);
                    aX = 0.25 * (mW + 2*binW) * (mW + 4*binW);
                    bX = 0.5 * (binW * (mW + 4*binW) );
                    aY = 0.25 * (mH + 2*binH) * (mH + 4*binH);
                    bY = 0.5 * binH * (mH + 4*binH);

                    thetaX = (ABSdX <= mW*0.5 + binW) ? (1 - aX * ABSdX * ABSdX) : (ABSdX <= mW*0.5 + binW*2 ) ? (bX * pow(ABSdX - 2 * mW - 2 * binW, 2 )) : 0;
                    thetaY = (ABSdY <= mH*0.5 + binH) ? (1 - aY * ABSdY * ABSdY) : (ABSdY <= mH*0.5 + binH*2 ) ? (bY * pow(ABSdY - 2 * mH - 2 * binH, 2 )) : 0;

                    binDensity[a + binCut*b] += c * thetaX * thetaY;

                    if( !_placement.module(i).isFixed() ){
                        double signX{(dX>=0) ? 1.0 : -1.0}, signY{(dY>=0) ? 1.0 : -1.0};
                        grad[2*i] += (ABSdX <= mW*0.5 + binW) ? (-2 * signX * c * aX * ABSdX * thetaY) : (ABSdX <= mW*0.5 + binW*2 ) ? (2 * c * bX * signX * ( ABSdX - 2 * binW - 2 * mW ) * thetaY) : 0;
                        grad[2*i + 1] += (ABSdY <= mH*0.5 + binH) ? (-2 * c * signY * aY * ABSdY * thetaX) : (ABSdY <= mH*0.5 + binH*2 ) ? (2 * c * bY * signY * ( ABSdY - 2 * binH - 2 * mH ) * thetaX) : 0;
                    }
                }            
            }

            f += lambda * pow(binDensity[a + binCut * b] - avgDensity, 2);
            
            for (unsigned i = 0; i < numModules; ++i)
            {
                g[2*i] += lambda*2*(binDensity[a + binCut*b] - avgDensity)*grad[2*i];
                g[2*i+1] += lambda*2*(binDensity[a + binCut*b] - avgDensity)*grad[2*i+1];
            }
        }
    }
}

void ExampleFunction::evaluateF(const vector<double> &x, double &f) // objective function
{
    f = 0.0; // objective cost function : LSE wirelength + Bin Density

    // LSE wirelength
    for(unsigned i = 0; i < numModules; ++i)
    {
        xExp[4*i] = exp(x[2*i] / eta);
        xExp[4*i+1] = exp(-x[2*i] / eta);
        xExp[4*i+2] = exp(x[2*i+1] / eta);
        xExp[4*i+3] = exp(-x[2*i+1] / eta);
    }
    
    for(unsigned i = 0; i <  _placement.numNets(); ++i)
    {
        double sumX1{0.0}, sumX2{0.0}, sumY1{0.0}, sumY2{0.0};
        for(unsigned j = 0; j < _placement.net(i).numPins(); ++j)
        {
            int mID =_placement.net(i).pin(j).moduleId();
            sumX1 += xExp[4*mID];
            sumX2 += xExp[4*mID+1];
            sumY1 += xExp[4*mID+2];
            sumY2 += xExp[4*mID+3];
        }
        f += eta*(log(sumX1) + log(sumX2) + log(sumY1) + log(sumY2));
    }

    if(lambda == 0) return; //first round -> return
    
    // Bin Density, bell-shaped smoothing
    memset(binDensity, 0.0, sizeof(double)*binTotalNum);
    double mW{0.0}, mH{0.0}, c{0.0};
    double thetaX{0.0}, thetaY{0.0}, dX{0.0}, dY{0.0}, ABSdX{0.0}, ABSdY{0.0}, aX{0.0}, bX{0.0}, aY{0.0}, bY{0.0};
    for (int a = 0; a < binCut; ++a)
    {
        for (int b = 0; b < binCut; ++b)
        {
            for (unsigned i = 0; i < numModules; ++i)
            {
                mW = _placement.module(i).width(); mH = _placement.module(i).height();
                if (!_placement.module(i).isFixed())
                {
                    c = _placement.module(i).area()/binArea;
                    
                    dX = _placement.module(i).centerX() - (((double)a+0.5)*binW + _placement.boundryLeft());
                    ABSdX = abs(dX);
                    dY = _placement.module(i).centerY() - (((double)b+0.5)*binH + _placement.boundryBottom());
                    ABSdY = abs(dY);
                    aX = 0.25 * (mW + 2*binW) * (mW + 4*binW);
                    bX = 0.5 * (binW * (mW + 4*binW) );
                    aY = 0.25 * (mH + 2*binH) * (mH + 4*binH);
                    bY = 0.5 * binH * (mH + 4*binH);

                    thetaX = (ABSdX <= mW*0.5 + binW) ? (1 - aX * ABSdX * ABSdX) : (ABSdX <= mW*0.5 + binW*2 ) ? (bX * pow(ABSdX - 2 * mW - 2 * binW, 2 )) : 0;
                    thetaY = (ABSdY <= mH*0.5 + binH) ? (1 - aY * ABSdY * ABSdY) : (ABSdY <= mH*0.5 + binH*2 ) ? (bY * pow(ABSdY - 2 * mH - 2 * binH, 2 )) : 0;

                    binDensity[a + binCut*b] += c * thetaX * thetaY;
                }            
            }
            f += lambda * pow(binDensity[a + binCut*b] - avgDensity, 2);
        }
    }
}

void ExampleFunction::Increase_Lambda()
{
    lambda += 1000;
}

unsigned ExampleFunction::dimension()
{
    return numModules * 2; // num_blocks*2
    // each two dimension represent the X and Y dimensions of each block
}