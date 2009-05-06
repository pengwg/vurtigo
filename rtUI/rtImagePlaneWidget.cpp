#include "rtImagePlaneWidget.h"

#include <vtkImageData.h>
#include <vtkDataSet.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkActor.h>
#include <vtkCommand.h>
#include <vtkCallbackCommand.h>
#include <vtkCellPicker.h>

#include <iostream>

rtImagePlaneWidget::rtImagePlaneWidget() {
  m_propAssembly = NULL;
}

rtImagePlaneWidget::~rtImagePlaneWidget() {
  m_propAssembly->Delete();
}


void rtImagePlaneWidget::fillWidgetActors(vtkPropAssembly* pa) {
  m_propAssembly = pa;
  m_propAssembly->AddPart( (vtkProp*)this->MarginActor );
  m_propAssembly->AddPart( (vtkProp*)this->PlaneOutlineActor );
  m_propAssembly->AddPart( (vtkProp*)this->TexturePlaneActor );
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
