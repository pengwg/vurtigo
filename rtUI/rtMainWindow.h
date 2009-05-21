#ifndef RT_MAIN_WINDOW_H
#define RT_MAIN_WINDOW_H

#include <QHBoxLayout>
#include <QBoxLayout>
#include <QHash>

#include "ui_rtMainWindow.h"
#include "customQVTKWidget.h"

#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkAxesActor.h>
#include <vtkPropAssembly.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkCellPicker.h>

class rtRenderObject;
#include "rtAxesProperties.h"
#include "rtOptions2DView.h"
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
  vtkCellPicker* getGlobalCellPicker() { return m_cellPicker; }

  void updateObjectList(QHash<int, rtRenderObject*>*);
  void updatePluginList(QHash<int, QTreeWidgetItem*>*);
  void clearObjectList();
  void clearPluginList();

  //! Set the value of the render flag.
  /*!
    If the render flag is set to true then the next render operation will do a redraw in the 3D window. 
   */
  void setRenderFlag3D(bool flag) { m_renderFlag3D=flag; }

  void tryRender3D();
  void update2DViews();

  void addRenderItem(vtkProp* prop);
  void removeRenderItem(vtkProp* prop);

  int createNew2DWidget();
  bool remove2DWidget(int id);
  rtOptions2DView* get2DWidget(int id);

 public slots:
  void currItemChanged(QTreeWidgetItem * current, QTreeWidgetItem * previous);
  void itemChanged(QTreeWidgetItem * current, int column);
  void viewChangedMixed();
  void viewChanged3DOnly();
  void viewChanged2DOnly();
  void loadPluginFile();

  void showAxesOptions();

  void pluginItemChanged(QTreeWidgetItem * current, QTreeWidgetItem * previous);



  void update2DWindowLists(QMultiHash<int, QString>* hash);

 protected:
  customQVTKWidget *m_render3DVTKWidget;
  QHBoxLayout *m_render3DLayout;
  QHBoxLayout *m_objectBrowseLayout;
  QWidget* m_currentObjectWidget;

  QHBoxLayout m_only3DLayout;
  QHBoxLayout m_only2DLayout;

  vtkRenderWindowInteractor *m_inter3D;
  vtkRenderWindow *m_renWin3D;
  vtkRenderer *m_renderer3D;
  vtkCellPicker *m_cellPicker;

  // The Axes in the corner
  vtkAxesActor *m_axesActor;
  vtkPropAssembly *m_propAssembly;
  vtkOrientationMarkerWidget *m_orientationWidget;
  rtAxesProperties* m_axesProperties;

  bool m_renderFlag3D;

  //! Hash of tree root objects.
  QHash<rtConstants::rtObjectType, QTreeWidgetItem *> m_topItems;
  //! Hash of the names root objects.
  QHash<rtConstants::rtObjectType, QString> m_rtObjectTypeNames;

  QHBoxLayout m_scrollArea2DImagesLayout;

  //! Hash of 2D view widgets
  QHash<int, rtOptions2DView*> m_view2DHash;

  //! Max Number of 2D Widgets
  int m_max2DWidgets;

  //! Layout for the plugin widget.
  QHBoxLayout m_pluginWidgetLayout;

  void connectSignals();
  void setupObjectTree();
  void populateObjectTypeNames();

  void setViewType(rtAxesProperties::ViewType);
  void setCoordType(rtAxesProperties::CoordType);

  void view2DHashCleanup();

};



#endif
