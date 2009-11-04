#ifndef OBJECTCONTROLWIDGET_H
#define OBJECTCONTROLWIDGET_H

#include <QObject>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>

#include <vtkTransform.h>

class ObjectControlWidget : public QObject
{
  Q_OBJECT
public:
    ObjectControlWidget();
    ~ObjectControlWidget();

    void setTransform(vtkTransform* input);
    void getTransform(vtkTransform* output);

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



  void updateWidgetPosition();
};

#endif // OBJECTCONTROLWIDGET_H
