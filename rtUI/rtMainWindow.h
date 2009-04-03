#ifndef RT_MAIN_WINDOW_H
#define RT_MAIN_WINDOW_H

#include <QHBoxLayout>
#include <QHash>

#include "ui_rtMainWindow.h"
#include <QVTKWidget.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>

#include "rtRenderObject.h"

class rtMainWindow : public QMainWindow, private Ui::rtMainWindowUI
{
  Q_OBJECT
 
 public:
  rtMainWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);
  ~rtMainWindow();

  vtkRenderWindow* getRenderWindow();
  vtkRenderWindowInteractor* getInteractor();
  vtkRenderer* getRenderer();

  void updateObjectList(QHash<int, rtRenderObject*>*);

 protected:
  QVTKWidget *m_render3DVTKWidget;
  QHBoxLayout *m_render3DLayout;
  vtkRenderWindowInteractor *m_inter3D;
  vtkRenderWindow *m_renWin3D;
  vtkRenderer *m_renderer3D;

  //! List of tree root objects.
  QHash<rtConstants::rtObjectType, QTreeWidgetItem *> m_topItems;
  QHash<rtConstants::rtObjectType, QString> m_rtObjectTypeNames;

  void connectSignals();
  void setupObjectTree();
  void populateObjectTypeNames();
};



#endif
