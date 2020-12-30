#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <algorithm>

using namespace std;

class Rectangle
{
public:
    Rectangle(double left = 0, double bottom = 0, double right = 0, double top = 0)
        : _left(left), _bottom(bottom), _right(right), _top(top)
    {
    }

    /////////////////////////////////////////////
    // get
    /////////////////////////////////////////////
    double left() const { return _left; }
    double bottom() const { return _bottom; }
    double right() const { return _right; }
    double top() const { return _top; }
    double width() const { return _right - _left; }
    double height() const { return _top - _bottom; }
    double centerX() const { return (_left + _right) / 2; }
    double centerY() const { return (_bottom + _top) / 2; }

    /////////////////////////////////////////////
    // set
    /////////////////////////////////////////////
    void setBounds(double left, double bottom, double right, double top)
    {
        _left = left;
        _bottom = bottom;
        _right = right;
        _top = top;
    }

    /////////////////////////////////////////////
    // overlap area of two rectangles
    /////////////////////////////////////////////
    static double overlapArea(const Rectangle &rect1, const Rectangle &rect2)
    {
        double overlapH = min(rect1.right(), rect2.right()) - max(rect1.left(), rect2.left());
        double overlapV = min(rect1.top(), rect2.top()) - max(rect1.bottom(), rect2.bottom());
        if (overlapH < 0)
            overlapH = 0;
        if (overlapV < 0)
            overlapV = 0;
        return overlapH * overlapV;
    }

private:
    double _left;
    double _bottom;
    double _right;
    double _top;
};

#endif // RECTANGLE_H
