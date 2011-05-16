#ifndef HELLOWORLDRENDEROBJECTMAKER_H
#define HELLOWORLDRENDEROBJECTMAKER_H

#include "rtRenderObjectMaker.h"
#include "HelloWorldRenderObject.h"

class HelloWorldRenderObjectMaker : public rtRenderObjectMaker
{
public:
    virtual rtRenderObject *createObject();
};

#endif // HELLOWORLDRENDEROBJECTMAKER_H
