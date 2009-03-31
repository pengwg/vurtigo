#ifndef RT_MAIN_WINDOW_H
#define RT_MAIN_WINDOW_H

#include <QHBoxLayout>
#include "ui_rtMainWindow.h"
#include <QVTKWidget.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>

class rtMainWindow : public QMainWindow, private Ui::rtMainWindowUI
{
  Q_OBJECT
 
 public:
  rtMainWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);
  ~rtMainWindow();

 protected:
  QVTKWidget *m_render3DVTKWidget;
  QHBoxLayout *m_render3DLayout;
  vtkRenderWindowInteractor *m_inter3D;
  vtkRenderWindow *m_renWin3D;

  void connectSignals();
};



#endif
