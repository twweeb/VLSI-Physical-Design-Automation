#ifndef PIN_H
#define PIN_H

class Pin
{
public:
    Pin(unsigned moduleId = -1, unsigned netId = -1, double xOffset = -1, double yOffset = -1)
        : _x(-1), _y(-1), _xOffset(xOffset), _yOffset(yOffset), _moduleId(moduleId), _netId(netId)
    {
    }

    /////////////////////////////////////////////
    // get
    ////////////////////////////////////////////
    double x() { return _x; }
    double y() { return _y; }
    double xOffset() { return _xOffset; }
    double yOffset() { return _yOffset; }
    unsigned moduleId() { return _moduleId; }
    unsigned netId() { return _netId; }
    unsigned pinId() { return _pinId; }

    /////////////////////////////////////////////
    // set
    /////////////////////////////////////////////
    void setPosition(double x, double y)
    {
        _x = x;
        _y = y;
    }
    void setOffset(double xOffset, double yOffset)
    {
        _xOffset = xOffset;
        _yOffset = yOffset;
    }
    void setModuleId(unsigned moduleId)
    {
        _moduleId = moduleId;
    }
    void setNetId(unsigned netId)
    {
        _netId = netId;
    }
    void setPinId(unsigned pinId)
    {
        _pinId = pinId;
    }

private:
    // variables from benchmark input
    double _x, _y;             // absolute x and y
    double _xOffset, _yOffset; // offsets from the center of the module
    unsigned _moduleId;        // id of the associated module
    unsigned _netId;           // id of the associated net
    unsigned _pinId;           // Pin Id
};

#endif // PIN_H
