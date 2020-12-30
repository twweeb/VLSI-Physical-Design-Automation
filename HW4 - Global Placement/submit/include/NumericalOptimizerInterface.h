#ifndef NUMERICALOPTIMIZERINTERFACE_H
#define NUMERICALOPTIMIZERINTERFACE_H

#include <vector>

using namespace std;

class NumericalOptimizerInterface
{
public:
    virtual void evaluateFG(const vector<double> &x, double &f, vector<double> &g) = 0;
    virtual void evaluateF(const vector<double> &x, double &f) = 0;
    virtual unsigned dimension() = 0;
};

#endif // NUMERICALOPTIMIZERINTERFACE_H
