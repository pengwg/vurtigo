#ifndef OBJECTCONTROLWIDGET_H
#define OBJECTCONTROLWIDGET_H

#include <QObject>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>

#include <vtkTransform.h>
#include <vtkSphereSource.h>
#include <vtkLineSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkParametricTorus.h>
#include <vtkParametricFunctionSource.h>

#include "rtBox2DOutline.h"

class ObjectControlWidget : public QObject
{
  Q_OBJECT
public:
  ObjectControlWidget();
  ~ObjectControlWidget();

  void setTransform(vtkTransform* input);
  void getTransform(vtkTransform* output);

  void setSize(double xsize, double ysize);
  double getXSize();
  double getYSize();

  void show();
  void hide();
  bool isShowing();
public slots:
  virtual void mousePressEvent(QMouseEvent* event);
  virtual void mouseMoveEvent(QMouseEvent* event);
  virtual void mouseReleaseEvent(QMouseEvent* event);
  virtual void mouseDoubleClickEvent(QMouseEvent* event);
  virtual void keyPressEvent(QKeyEvent* event);
  virtual void keyReleaseEvent(QKeyEvent* event);
  virtual void wheelEvent(QWheelEvent* event);
protected:
  bool m_showing;
  vtkTransform* m_transform;
  double m_xsize, m_ysize;

  double m_pointLocations[9][3];
  double m_convertedLocations[9][3];
  int m_corners[4];

  vtkActor *m_currProp;
  double m_clickPosition[3];
  double m_positiveDirection[3];
  double m_positiveDirectionT[3];

  //! The previous x location of the mouse
  int m_oldX;
  //! The previous y location of the mouse
  int m_oldY;

  vtkSphereSource* m_touchPoint;
  vtkPolyDataMapper* m_pointMapper;
  vtkActor* m_pointActor;

  rtBox2DOutline m_boxOutline;

  // The three rings.
  vtkTransform *m_position[3];
  vtkParametricTorus *m_torus[3];
  vtkParametricFunctionSource *m_torusSrc[3];
  vtkPolyDataMapper *m_diskMapper[3];
  vtkActor *m_diskActor[3];

  void updateWidgetPosition();
  void normalizeVector(double vec[3]);
};

#endif // OBJECTCONTROLWIDGET_H
