#ifndef CONTOUR_H
#define CONTOUR_H

#include <cstring>
#include <cstdlib>
#include "BStree.hpp"
class Contour
{
    private:
        int *horizontal, *vertical;
        int tryLength, outline;

    public:
        Contour (int outline);
        Contour (){};
        ~Contour ();
        int _getY (int x, int width, int height);
        int _outline ();
        bool _isLegal (int l);
        int _H (int i);
        void bStree_update (int cur, BStree &bStree, bool isRight);
        void update (BStree &bStree);
        void set_outline (int);
        void clearContour ();
};

#endif