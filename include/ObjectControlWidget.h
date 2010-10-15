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
#include "rtCrosshair2D.h"

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

  void setUserTransform(vtkTransform* t);

  void setWidgetOpacity(double op);
  double getWidgetOpacity();

  inline vtkActor* getObjectOfInterest() { return m_objOfInterest; }
  inline void setObjectOfInterest(vtkActor* obj) { m_objOfInterest = obj; }

public slots:
  virtual void mousePressEvent(QMouseEvent* event);
  virtual void mouseMoveEvent(QMouseEvent* event);
  virtual void mouseReleaseEvent(QMouseEvent* event);
  virtual void mouseDoubleClickEvent(QMouseEvent* event);
  virtual void keyPressEvent(QKeyEvent* event);
  virtual void keyReleaseEvent(QKeyEvent* event);
  virtual void wheelEvent(QWheelEvent* event);

  void cameraMode(bool);
  void interactionMode(bool);
  void placeMode(bool);
protected:
  bool m_showing;
  vtkTransform* m_transform;
  vtkTransform* m_userTransform;
  double m_xsize, m_ysize;

  double m_pointLocations[9][3];
  double m_convertedLocations[9][3];
  int m_corners[4];

  vtkActor *m_currProp;
  double m_clickPosition[3];
  double m_positiveDirection[3];
  double m_positiveDirectionT[3];

  //! The opacity of the widget.
  double m_widgetOpacity;

  //! The previous x location of the mouse
  int m_oldX;
  //! The previous y location of the mouse
  int m_oldY;

  //! The object that is going to be moved
  /*!
    This is used for the opacity picking.
    */
  vtkActor* m_objOfInterest;

  // Central sphere.
  vtkSphereSource* m_touchPoint;
  vtkPolyDataMapper* m_pointMapper;
  vtkActor* m_pointActor;

  // Crosshair
  rtCrosshair2D m_crosshair;

  // Box outline
  rtBox2DOutline m_boxOutline;

  // The three rings.
  vtkTransform *m_position[3];
  vtkParametricTorus *m_torus[3];
  vtkParametricFunctionSource *m_torusSrc[3];
  vtkPolyDataMapper *m_diskMapper[3];
  vtkActor *m_diskActor[3];

  void updateWidgetPosition();

  //! Get the actor at a given x,y location
  /*!
    If no actor is found the function returns NULL.
    The click position parameter is loaded with the coordinates in 3-space of the pick location.
    */
  vtkActor* getLocalPropAt(int x, int y, double clickPos[3]);

  //! Check if the object of interest is under the position x, y
  bool pickedObjectOfInterest(int x, int y);
};

#endif // OBJECTCONTROLWIDGET_H
