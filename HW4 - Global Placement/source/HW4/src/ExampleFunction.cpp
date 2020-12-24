#include "ExampleFunction.h"

// minimize 3*x^2 + 2*x*y + 2*y^2 + 7

ExampleFunction::ExampleFunction()
{
}

void ExampleFunction::evaluateFG(const vector<double> &x, double &f, vector<double> &g)
{
    f = 3 * x[0] * x[0] + 2 * x[0] * x[1] + 2 * x[1] * x[1] + 7; // objective function
    g[0] = 6 * x[0] + 2 * x[1];                                  // gradient function of X
    g[1] = 2 * x[0] + 4 * x[1];                                  // gradient function of Y
}

void ExampleFunction::evaluateF(const vector<double> &x, double &f)
{
    f = 3 * x[0] * x[0] + 2 * x[0] * x[1] + 2 * x[1] * x[1] + 7; // objective function
}

unsigned ExampleFunction::dimension()
{
    return 2; // num_blocks*2
    // each two dimension represent the X and Y dimensions of each block
}
