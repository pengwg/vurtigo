/*******************************************************************************
    Vurtigo: Visualization software for interventional applications in medicine
    Copyright (C) 2009 Sunnybrook Health Sciences Centre

    This file is part of Vurtigo.

    Vurtigo is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

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
