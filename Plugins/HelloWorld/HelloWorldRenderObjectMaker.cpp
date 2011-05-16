#include "HelloWorldRenderObjectMaker.h"

rtRenderObject *HelloWorldRenderObjectMaker::createObject()
{
    return new HelloWorldRenderObject();
}
