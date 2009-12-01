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
#ifndef RT_IMAGE_PLANE_WIDGET
#define RT_IMAGE_PLANE_WIDGET

#include <QList>

#include "vtkImagePlaneWidget.h"
#include "vtkProp.h"
#include "vtkAbstractPropPicker.h"
#include "vtkActor.h"

class rtImagePlaneWidget : public vtkImagePlaneWidget {

 public:
  rtImagePlaneWidget();
  ~rtImagePlaneWidget();

  virtual void OnLeftButtonDown();
  virtual void OnLeftButtonUp();

  void SetInput(vtkDataSet* input);

  void SetPlaneOrientation(int);

  void SetPlaneOrientationToXAxes()
    { this->SetPlaneOrientation(0); }
  void SetPlaneOrientationToYAxes()
    { this->SetPlaneOrientation(1); }
  void SetPlaneOrientationToZAxes()
    { this->SetPlaneOrientation(2); }

  QList<vtkProp*>* getPropsInAssembly() { return m_propAssembly; }
  void fillWidgetActors(QList<vtkProp*>* propList);

  vtkActor* getMarginActor() { return this->MarginActor; }
  vtkActor* getPlaneOutlineActor() { return this->PlaneOutlineActor; }
  vtkActor* getTexturePlaneActor() { return this->TexturePlaneActor; }

  void SetEnabled(int enabling);
  void SetPicker(vtkAbstractPropPicker* picker);
  void setWindowLevel(int w, int l);

  protected:
    QList<vtkProp*>* m_propAssembly;
};

#endif
