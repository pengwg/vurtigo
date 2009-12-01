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
#include "rtImagePlaneWidget.h"

#include <vtkImageData.h>
#include <vtkDataSet.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkActor.h>
#include <vtkCommand.h>
#include <vtkCallbackCommand.h>
#include <vtkCellPicker.h>
#include <vtkLookupTable.h>

#include <iostream>

rtImagePlaneWidget::rtImagePlaneWidget() {
  m_propAssembly = NULL;
}

rtImagePlaneWidget::~rtImagePlaneWidget() {
}


void rtImagePlaneWidget::fillWidgetActors(QList<vtkProp*>* propList) {
  m_propAssembly = propList;
  m_propAssembly->push_back( (vtkProp*)this->MarginActor );
  m_propAssembly->push_back( (vtkProp*)this->PlaneOutlineActor );
  m_propAssembly->push_back( (vtkProp*)this->TexturePlaneActor );
}


void rtImagePlaneWidget::OnLeftButtonDown() {
}

void rtImagePlaneWidget::OnLeftButtonUp() {
}

void rtImagePlaneWidget::SetInput(vtkDataSet* input) {

  if (this->ImageData == vtkImageData::SafeDownCast(input)) {
    return;
  }

  vtkImagePlaneWidget::SetInput(input);
}

void rtImagePlaneWidget::SetPlaneOrientation(int i) {
  if (this->PlaneOrientation == i) return;

  vtkImagePlaneWidget::SetPlaneOrientation(i);
}


void rtImagePlaneWidget::SetEnabled(int enabling)
{

  if ( ! this->Interactor ) {
    vtkErrorMacro(<<"The interactor must be set prior to enabling/disabling widget");
    return;
  }

  if ( enabling ) //----------------------------------------------------------
    {
    vtkDebugMacro(<<"Enabling plane widget");

    //already enabled, just return
    if ( this->Enabled ) return;

    if ( ! this->CurrentRenderer )
      {
      this->SetCurrentRenderer(this->Interactor->FindPokedRenderer(
        this->Interactor->GetLastEventPosition()[0],
        this->Interactor->GetLastEventPosition()[1]));
      if (this->CurrentRenderer == NULL)
        {
        return;
        }
      }

    this->Enabled = 1;

    // we have to honour this ivar: it could be that this->Interaction was
    // set to off when we were disabled
    if (this->Interaction) {
      this->AddObservers();
    }

    // Add the plane
    this->PlaneOutlineActor->SetProperty(this->PlaneProperty);

    //add the TexturePlaneActor
    this->TexturePlaneActor->SetProperty(this->TexturePlaneProperty);

    // Add the cross-hair cursor
    this->CursorActor->SetProperty(this->CursorProperty);

    // Add the margins
    this->MarginActor->SetProperty(this->MarginProperty);

    //this->MarginActor->SetVisibility(1);
    this->PlaneOutlineActor->SetVisibility(1);
    this->TexturePlaneActor->SetVisibility(1);

    this->TexturePlaneActor->PickableOn();

    this->InvokeEvent(vtkCommand::EnableEvent,0);
    }

  else //disabling----------------------------------------------------------
    {
    vtkDebugMacro(<<"Disabling plane widget");

    if ( ! this->Enabled ) //already disabled, just return
      {
      return;
      }

    this->Enabled = 0;

    // don't listen for events any more
    this->Interactor->RemoveObserver(this->EventCallbackCommand);

    this->MarginActor->SetVisibility(0);
    this->PlaneOutlineActor->SetVisibility(0);
    this->TexturePlaneActor->SetVisibility(0);

    this->TexturePlaneActor->PickableOff();

    this->InvokeEvent(vtkCommand::DisableEvent,0);
    this->SetCurrentRenderer(NULL);
    }

  this->Interactor->Render();
}

void rtImagePlaneWidget::SetPicker(vtkAbstractPropPicker* picker)
{
  // we have to have a picker for slice motion, window level and cursor to work
  if (this->PlanePicker != picker)
    {
    // to avoid destructor recursion
    vtkAbstractPropPicker *temp = this->PlanePicker;
    this->PlanePicker = picker;
    if (temp != 0)
      {
      temp->UnRegister(this);
      }

    int delPicker = 0;
    if (this->PlanePicker == 0)
      {
      this->PlanePicker = vtkCellPicker::New();
      vtkCellPicker::SafeDownCast(this->PlanePicker)->SetTolerance(0.005);
      delPicker = 1;
      }

    this->PlanePicker->Register(this);
    this->PlanePicker->AddPickList(this->TexturePlaneActor);
    this->PlanePicker->PickFromListOn();
    if ( delPicker )
      {
      this->PlanePicker->Delete();
      }
    }
}

void rtImagePlaneWidget::setWindowLevel(int w, int l) {
  double rmin = l - 0.5*fabs( w );
  double rmax = rmin + fabs( w );
  this->LookupTable->SetTableRange( rmin, rmax );
}
