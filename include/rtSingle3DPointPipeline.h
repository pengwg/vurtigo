/***************************************************************************
Vurtigo: Visualization software for interventional applications in medicine


Copyright (c) 2011, Sunnybrook Research Institute
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Sunnybrook Research Institute nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Sunnybrook Research Institute BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************************/
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
