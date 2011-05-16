#include "HelloWorldRenderObject.h"
#include "HelloWorldDataObject.h"

#include "rtBaseHandle.h"
#include "rtApplication.h"
#include "rtMessage.h"

#include <vtkPolyDataMapper.h>
#include <vtkFollower.h>
#include <vtkVectorText.h>

HelloWorldRenderObject::HelloWorldRenderObject()
{
    setObjectType("HelloWorld");
    setName("HelloWorld Renderer");
    m_msgMapper = vtkPolyDataMapper::New();
    m_msgActor = vtkFollower::New();
    m_msgText = vtkVectorText::New();

    m_msgText->SetText("Hello World!");
    m_msgMapper->SetInputConnection(m_msgText->GetOutputPort());
    m_msgActor->SetMapper(m_msgMapper);

    m_msgActor->SetPosition(0,0,0);
    m_msgActor->SetScale(10);

    // This object can be rendered in 3D
    m_canRender3D = true;

    setupDataObject();
}

HelloWorldRenderObject::~HelloWorldRenderObject()
{
    if (m_msgMapper) m_msgMapper->Delete();
    if (m_msgActor) m_msgActor->Delete();
    if (m_msgText) m_msgText->Delete();
}

//! Take info from the data object
void HelloWorldRenderObject::update()
{
    HelloWorldDataObject *dObj = static_cast<HelloWorldDataObject*>(m_dataObj);
    if (!dObj) {
      rtApplication::instance().getMessageHandle()->warning(__LINE__, __FILE__, QString("Update fialed! Data object is NULL."));
      return;
    }

    m_msgText->SetText(dObj->getMessage().toStdString().c_str());

}

//! Add this object to the given renderer.
bool HelloWorldRenderObject::addToRenderer(vtkRenderer *ren, int window)
{
    if (!ren)
    {
      rtApplication::instance().getMessageHandle()->warning(__LINE__, __FILE__, QString("addToRenderer failed! Renderer object is NULL."));
      return false;
    }

    if (!ren->HasViewProp(m_msgActor))
    {
        ren->AddViewProp(m_msgActor);
        m_msgActor->SetCamera(ren->GetActiveCamera());
    }

    rtBaseHandle::instance().connectObjectToEvents(this,window);

    setVisible3D(window,true);
    return true;
}

//! Remove this object from the given renderer.
bool HelloWorldRenderObject::removeFromRenderer(vtkRenderer *ren, int window)
{
    if (!ren) return false;
    setVisible3D(window,false);

    if (ren->HasViewProp(m_msgActor))
        ren->RemoveViewProp(m_msgActor);

    rtBaseHandle::instance().disconnectObjectFromEvents(this,window);

    return true;

}

//! Create the correct data object.
void HelloWorldRenderObject::setupDataObject()
{
  m_dataObj = new HelloWorldDataObject();
}

bool HelloWorldRenderObject::getObjectLocation(double loc[6])
{
    if (!m_msgActor)
    {
        rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Could not get object location. Object actor is NULL."));
        return false;
    }

    m_msgActor->GetBounds(loc);
    return true;
}
