#include "Contour.hpp"

Contour::Contour (int outline)
{
    this->tryLength = outline<<2;
    this->outline = outline;
    this->horizontal = (int*)malloc(sizeof(int)*tryLength);
    memset (this->horizontal, 0, this->tryLength*sizeof(int));
}
Contour::~Contour() {delete [] horizontal, vertical;}

int Contour::_outline() {return outline;}
bool Contour::_isLegal(int l) {return l <= tryLength;}
int Contour::_H(int i) {return horizontal[i];}
void Contour::update(BStree &bStree) {clearContour(); bStree_update(bStree._getRoot(), bStree, false);}
void Contour::clearContour() {
    memset (horizontal, 0, tryLength*sizeof(int));
}

void Contour::set_outline (int outline)
{
    this->tryLength = outline<<2;
    this->outline = outline;
    this->horizontal = (int*)malloc(sizeof(int)*tryLength);
    memset (this->horizontal, 0, this->tryLength*sizeof(int));
}
int Contour::_getY (int x, int width, int height)
{
    int maxH = 0, newH = 0;
    for (int i = x; i < x+width; ++i)
    {
        if (maxH < horizontal[i]) maxH = horizontal[i];
    }
    newH = maxH + height;
    //std::cout << "x = " << x << ", width = " << width <<  ", height = " << height << ", maxH = " << maxH <<", newH = "<< newH << '\n';
    for (int i = x; i < x+width; ++i) horizontal[i] = newH;
    return maxH;
}

void Contour::bStree_update (int cur, BStree &bStree, bool isRight)
{

    if (cur == bStree._getRoot()) bStree._getTree()[cur]->x = 0;
    else if (isRight) bStree._getNode(cur)->x = bStree._getNode(bStree._getNode(cur)->parent)->x;
    else bStree._getNode(cur)->x = bStree._getNode(bStree._getNode(cur)->parent)->x + bStree._getNode(bStree._getNode(cur)->parent)->width;

    bStree._getNode(cur)->y = _getY(bStree._getNode(cur)->x, bStree._getNode(cur)->width, bStree._getNode(cur)->height);

    if (bStree._getNode(cur)->left != -1) bStree_update(bStree._getNode(cur)->left, bStree, false);
    if (bStree._getNode(cur)->right != -1) bStree_update(bStree._getNode(cur)->right, bStree, true);
}