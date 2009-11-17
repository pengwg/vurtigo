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

class ObjectControlWidget : public QObject
{
  Q_OBJECT
public:
  ObjectControlWidget();
  ~ObjectControlWidget();

  void setTransform(vtkTransform* input);
  void getTransform(vtkTransform* output);

  void setSize(double size);
  double getSize();

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
  double m_size;

  double m_pointLocations[9][3];
  double m_convertedLocations[9][3];
  int m_corners[4];
  int m_currPropIndex;

  int m_oldX;
  int m_oldY;

  vtkSphereSource* m_touchPoints[9];
  vtkPolyDataMapper* m_pointMapper[9];
  vtkActor* m_pointActor[9];

  vtkLineSource* m_edges[4];
  vtkPolyDataMapper* m_lineMapper[4];
  vtkActor* m_lineActor[4];

  void updateWidgetPosition();
};

#endif // OBJECTCONTROLWIDGET_H
