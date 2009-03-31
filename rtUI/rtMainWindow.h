#ifndef RT_MAIN_WINDOW_H
#define RT_MAIN_WINDOW_H

#include "ui_rtMainWindow.h"

class rtMainWindow : public QMainWindow, private Ui::rtMainWindowUI
{
  Q_OBJECT
 
 public:
  rtMainWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);

};



#endif
