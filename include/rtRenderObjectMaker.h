#ifndef RTRENDEROBJECTMAKER_H
#define RTRENDEROBJECTMAKER_H

#include "rtRenderObject.h"

//! A class for making Render Objects

class rtRenderObjectMaker
{
public:
    rtRenderObjectMaker() {}
    ~rtRenderObjectMaker() {}
    virtual rtRenderObject *createObject() = 0;
};

#endif // RTRENDEROBJECTMAKER_H
