#ifndef NUMERICALOPTIMIZER_H
#define NUMERICALOPTIMIZER_H

#include "NumericalOptimizerInterface.h"

class NumericalOptimizer
{
public:
    NumericalOptimizer(NumericalOptimizerInterface &noi);

    // set
    void setNumIteration(unsigned num);
    void setStepSizeBound(double size);
    void setX(const vector<double> &x);

    // operation
    void solve();

    // get
    unsigned dimension() { return _n; }
    double objective() { return _f; }
    double x(unsigned i) { return _x[i]; }

private:
    // parameter
    unsigned _numIteration;
    double _stepSizeBound;

    // conjugate-gradient search
    unsigned _n;
    double _alpha, _alphaLast, _beta, _f, _fLast;
    vector<double> _grad, _gradLast, _dir, _dirLast, _x;
    void computeBeta();
    void computeDirection();
    bool lineSearch1();
    bool lineSearch2();
    NumericalOptimizerInterface &_noi;
};

#endif // NUMERICALOPTIMIZER_H
