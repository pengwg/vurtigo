#ifndef RT_MAIN_WINDOW_H
#define RT_MAIN_WINDOW_H

#include <QHBoxLayout>
#include <QBoxLayout>
#include <QHash>

#include "ui_rtMainWindow.h"
#include <QVTKWidget.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>

class rtRenderObject;
class rtObjectManager;
#include "objTypes.h"

//! Object that controls the visible Qt widgets. 
class rtMainWindow : public QMainWindow, private Ui::rtMainWindowUI
{
  Q_OBJECT
 
 public:
  rtMainWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);
  ~rtMainWindow();

  vtkRenderWindow* getRenderWindow();
  vtkRenderWindowInteractor* getInteractor();
  vtkRenderer* getRenderer();
  QTreeWidget* getObjectTree();

  void updateObjectList(QHash<int, rtRenderObject*>*);
  void setObjectManager(rtObjectManager* man);

  //! Set the value of the render flag.
  /*!
    If the render flag is set to true then the next render operation will do a redraw in the 3D window. 
   */
  void setRenderFlag3D(bool flag) { m_renderFlag3D=flag; }

  void tryRender3D();

 public slots:
  void currItemChanged(QTreeWidgetItem * current, QTreeWidgetItem * previous);
  void itemChanged(QTreeWidgetItem * current, int column);
  void viewChangedMixed();
  void viewChanged3DOnly();
  void viewChanged2DOnly();
  void loadPluginFile();

 protected:
  //! Handle to the object manager.
  rtObjectManager* m_objMan;

  QVTKWidget *m_render3DVTKWidget;
  QHBoxLayout *m_render3DLayout;
  QHBoxLayout *m_objectBrowseLayout;
  QWidget* m_currentObjectWidget;

  QHBoxLayout m_only3DLayout;
  QHBoxLayout m_only2DLayout;

  vtkRenderWindowInteractor *m_inter3D;
  vtkRenderWindow *m_renWin3D;
  vtkRenderer *m_renderer3D;

  bool m_renderFlag3D;

  //! Hash of tree root objects.
  QHash<rtConstants::rtObjectType, QTreeWidgetItem *> m_topItems;
  //! Hash of the names root objects.
  QHash<rtConstants::rtObjectType, QString> m_rtObjectTypeNames;

  void connectSignals();
  void setupObjectTree();
  void populateObjectTypeNames();
};



#endif
