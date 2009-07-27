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

  protected:
    QList<vtkProp*>* m_propAssembly;
};

#endif
