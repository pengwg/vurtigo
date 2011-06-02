#ifndef RTSINGLE3DPOINTPIPELINE_H
#define RTSINGLE3DPOINTPIPELINE_H

#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkFollower.h>
#include <vtkVectorText.h>
#include <QList>

//! Complete rendering pipeline for a single point in 3D space.
class rtSingle3DPointPipeline
{
public:
  rtSingle3DPointPipeline();
  ~rtSingle3DPointPipeline();

  inline void setMapper(vtkPolyDataMapper *mapper) { m_actor->SetMapper(mapper); }

  inline vtkActor* getActor() { return m_actor; }

  inline vtkActor* getLabelActor(int i) { if (m_labelActor.size() > i) return m_labelActor[i]; }

  void setPosition(double x, double y, double z);

  void setPosition(double coords[3]);

  void setLabelPosition(int i, double x,double y,double z);

  void setLabelPosition(int i, double coords[3]);

  //! Set the radius of the displayed point.
  void setRadius(double r);

  void setLabelScale(int i, double s);

  void setLabelText(const char *text);

  void setLabelCamera(int i, vtkCamera *cam);

  inline vtkProperty* getLabelProperty(int i) {return m_labelActor[i]->GetProperty(); }

  void setProperty(vtkProperty* prop);

  inline vtkProperty* getPropertyHandle() { return m_actor->GetProperty(); }

  void setNumRenWin(int num);
  int getNumRenWin() {return m_labelActor.size();}

protected:
  // VTK pipeline objects.
  vtkActor* m_actor;
  QList<vtkFollower *> m_labelActor;
  vtkPolyDataMapper *m_labelMapper;
  vtkVectorText *m_labelText;
};

#endif // RTSINGLE3DPOINTPIPELINE_H
