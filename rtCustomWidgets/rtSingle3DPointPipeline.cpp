#include "rtSingle3DPointPipeline.h"
#include "rtApplication.h"
#include "rtMainWindow.h"

rtSingle3DPointPipeline::rtSingle3DPointPipeline() {
  m_actor = vtkActor::New();

  m_labelMapper = vtkPolyDataMapper::New();
  m_labelText = vtkVectorText::New();
  m_labelMapper->SetInputConnection(m_labelText->GetOutputPort());
  for (int ix1=0; ix1 < rtApplication::instance().getMainWinHandle()->getNumRenWins(); ix1++)
  {
      m_labelActor.append(vtkFollower::New());
      m_labelActor.last()->SetMapper(m_labelMapper);
  }
}

rtSingle3DPointPipeline::~rtSingle3DPointPipeline() {

  m_actor->Delete();
  m_labelMapper->Delete();
  m_labelText->Delete();
  for (int ix1=0; ix1<m_labelActor.size(); ix1++)
      m_labelActor[ix1]->Delete();
}

void rtSingle3DPointPipeline::setPosition(double x, double y, double z) {
  m_actor->SetPosition(x,y,z);
}

void rtSingle3DPointPipeline::setPosition(double coords[3]) {
  m_actor->SetPosition(coords);
}

void rtSingle3DPointPipeline::setLabelPosition(int i, double x, double y, double z) {
    if (i < m_labelActor.size())
        m_labelActor[i]->SetPosition(x,y,z);
}

void rtSingle3DPointPipeline::setLabelPosition(int i, double coords[3]) {
    if (i < m_labelActor.size())
        m_labelActor[i]->SetPosition(coords);
}

void rtSingle3DPointPipeline::setRadius(double r) {
  if (r<=0.0) return;
  m_actor->SetScale(r);
}

void rtSingle3DPointPipeline::setLabelScale(int i, double s)
{
    if (i < m_labelActor.size())
        m_labelActor[i]->SetScale(s);
}

void rtSingle3DPointPipeline::setLabelCamera(int i, vtkCamera *cam)
{
    if (i < m_labelActor.size())
        m_labelActor[i]->SetCamera(cam);
}

void rtSingle3DPointPipeline::setLabelText(const char *text)
{
        m_labelText->SetText(text);
}

void rtSingle3DPointPipeline::setProperty(vtkProperty* prop) {
  if (!prop) return;
  m_actor->SetProperty(prop);
}

void rtSingle3DPointPipeline::setNumRenWin(int num)
{
    // add any new actors if needed
    int ix1;
    for (ix1=0; ix1<(num - m_labelActor.size()); ix1++)
    {
        m_labelActor.append(vtkFollower::New());
        m_labelActor.last()->SetMapper(m_labelMapper);
    }
}
