#ifndef RT_IMAGE_PLANE_WIDGET
#define RT_IMAGE_PLANE_WIDGET

#include "vtkImagePlaneWidget.h"
#include "vtkPropAssembly.h"
#include "vtkAbstractPropPicker.h"

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

  vtkPropAssembly* getPropsInAssembly() { return m_propAssembly; }
  void fillWidgetActors(vtkPropAssembly* pa);

  void SetEnabled(int enabling);
  void SetPicker(vtkAbstractPropPicker* picker);

  protected:
    vtkPropAssembly *m_propAssembly;
};

#endif
