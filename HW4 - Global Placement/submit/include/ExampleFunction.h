#ifndef EXAMPLEFUNCTION_H
#define EXAMPLEFUNCTION_H

#include "Placement.h"
#include "NumericalOptimizerInterface.h"

class ExampleFunction : public NumericalOptimizerInterface
{
public:
    ExampleFunction(Placement &placement);
    Placement &_placement;
    
    void evaluateFG(const vector<double> &x, double &f, vector<double> &g);
    void evaluateF(const vector<double> &x, double &f);
    unsigned dimension();
    void Increase_Lambda();
    
    unsigned numModules{0};
    int lambda{0}, binCut{0}, binTotalNum{0};
    double eta{0.0}, boundW{0.0}, boundH{0.0}, binW{0.0}, binH{0.0}, binArea{0.0}, avgDensity{0.0};
    double *grad{nullptr}, *xExp{nullptr}, *binDensity{nullptr};
};

#endif // EXAMPLEFUNCTION_H
