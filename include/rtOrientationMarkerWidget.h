#ifndef RT_ORIENTATION_MARKER_WIDGET_H
#define RT_ORIENTATION_MARKER_WIDGET_H

#include <vtkOrientationMarkerWidget.h>
#include <vtkActor2D.h>
#include <QObject>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QSize>

class customQVTKWidget;

class rtOrientationMarkerWidget : public QObject, public vtkOrientationMarkerWidget
{
  Q_OBJECT
public:
  //! Constructor
  rtOrientationMarkerWidget(customQVTKWidget*);

  //! Destructor
  ~rtOrientationMarkerWidget();

  void setOutlineVisibility(bool visible) { this->OutlineActor->SetVisibility( visible ); };

public slots:
  void mousePress(QMouseEvent*);
  void mouseMove(QMouseEvent*);
  void mouseRelease(QMouseEvent*);
  void mouseDoubleClick(QMouseEvent*);
  void keyPress(QKeyEvent*);
  void keyRelease(QKeyEvent*);
  void wheel(QWheelEvent*);

protected:
  customQVTKWidget* m_eventWidget;
  QSize m_winSize;

  bool m_leftMouseDown;
  bool m_rightMouseDown;
  bool m_midMouseDown;

  void connectEvents();
};

#endif // RT_ORIENTATION_MARKER_WIDGET_H
