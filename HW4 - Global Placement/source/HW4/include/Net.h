#ifndef NET_H
#define NET_H

#include "Pin.h"
#include <vector>

using namespace std;

class Net
{
public:
    Net() {}

    /////////////////////////////////////////////
    // get (for pins of this net)
    /////////////////////////////////////////////
    unsigned numPins() { return _pPins.size(); }
    Pin &pin(unsigned index) { return *_pPins[index]; } // index: 0 ~ (numPins-1), not Pin id

    /////////////////////////////////////////////
    // set (for pins of this net)
    /////////////////////////////////////////////
    void setNumPins(unsigned numPins) { _pPins.resize(numPins); }
    void addPin(Pin *pPin) { _pPins.push_back(pPin); }
    void clearPins() { _pPins.clear(); }

private:
    // pins of the module
    vector<Pin *> _pPins;
};

#endif // NET_H
