#ifndef EXAMPLEFUNCTION_H
#define EXAMPLEFUNCTION_H

#include "NumericalOptimizerInterface.h"

class ExampleFunction : public NumericalOptimizerInterface
{
public:
    ExampleFunction();

    void evaluateFG(const vector<double> &x, double &f, vector<double> &g);
    void evaluateF(const vector<double> &x, double &f);
    unsigned dimension();
};

#endif // EXAMPLEFUNCTION_H
