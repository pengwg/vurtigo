#ifndef CUSTOM_QVTK_WIDGET_H
#define CUSTOM_QVTK_WIDGET_H

#include "QVTKWidget.h"
#include "vtkPropPicker.h"
#include "vtkProp.h"

class rtRenderObject;

class customQVTKWidget : public QVTKWidget {
 public:
  customQVTKWidget(QWidget* parent = NULL, Qt::WFlags f = 0);
  ~customQVTKWidget();

  void setSquare(bool sq) { m_forceSquare = sq; }
  bool isSquare() { return m_forceSquare; }

 protected:
  virtual void resizeEvent(QResizeEvent* event);
  virtual void mousePressEvent(QMouseEvent* event);
  virtual void mouseMoveEvent(QMouseEvent* event);
  virtual void mouseReleaseEvent(QMouseEvent* event);
  virtual void mouseDoubleClickEvent(QMouseEvent* event);

  vtkPropPicker* m_objectPicker;
  vtkProp* m_currProp;
  rtRenderObject* m_currRenObj;

  bool m_forceSquare;
};

#endif
