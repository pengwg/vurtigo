/*******************************************************************************
    Vurtigo: Visualization software for interventional applications in medicine
    Copyright (C) 2009 Sunnybrook Health Sciences Centre

    This file is part of Vurtigo.

    Vurtigo is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/
#ifndef RT_MAIN_WINDOW_H
#define RT_MAIN_WINDOW_H

#include <QWhatsThis>

class vtkRenderWindow;
class vtkAxesActor;
class vtkPropAssemply;
class vtkProp;

// Remove the conflict with AssertMacros.h
// Conflict occurs in OSX
#ifdef Q_WS_MAC
  #ifdef check
  #undef check
  #endif
#endif

#include <QHBoxLayout>
#include <QBoxLayout>
#include <QHash>
#include <QSemaphore>
#include <QtHelp>
#include <QTreeWidget>

class rtRenderObject;
class rtCameraControl;
class rtHelpManager;
class pointPlacementDialog;
class rtRegistration;
class rtOrientationMarkerWidget;

#include "ui_rtMainWindow.h"
#include "customQVTKWidget.h"
#include "rtAxesProperties.h"
#include "rtOptions2DView.h"
#include "objTypes.h"

//! Object that controls the visible Qt widgets. 
/*!
  The class that takes care of the main window object in Vurtigo.
  */
class rtMainWindow : public QMainWindow, private Ui::rtMainWindowUI
{
  Q_OBJECT
 
 public:
  //! Constructor
  /*!
    \param parent The parent widget for this window. In most cases this is NULL since this is a main window and has no parent.
    \param flags The window flags.
    */
  rtMainWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);

  //! Destructor
  /*!
  Removes the current widget from the viewer at the bottom.
  Also deletes objects that were created in this class.
  */
  ~rtMainWindow();

  //! Get the pointer to the render window
  vtkRenderWindow* getRenderWindow(int window);

  //! Get the pointer to the renderer
  vtkRenderer* getRenderer(int window = 0);
  QTreeWidget* getObjectTree();
  customQVTKWidget* getRenderWidget(int window) { return m_render3DVTKWidget[window]; }

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

  void addRenderItem(vtkProp* prop, int window);
  void removeRenderItem(vtkProp* prop, int window);

  int createNew2DWidget();
  bool remove2DWidget(int id);
  rtOptions2DView* get2DWidget(int id);

  //! Get the type of interaction the the user has currently selected
  customQVTKWidget::InteractionMode getInteractionMode() { return m_render3DVTKWidget[0]->getInteraction(); }

  //! Get the number of render windows
  int getNumRenWins() {return m_numRenWin;}

  //! Check if the camera for the 3D view is moving
  /*!
    Some object types will render in lower quality when the camera is moving.
    \return True if the camera is moving and false otherwise. Also returns false if the camera control object has not yet been created.
    */
  bool cameraMoving(int window);

  //! Get the distance from the focal point to the camera.
  double getCameraDistance(int window);

  //! Get the currently selected prop in the given rendering window.
  vtkProp* getSelectedProp(int window);

  //! Get the up direction for the camera.
  void getCameraUp(double val[3], int window);

  //! Get the direction that points to the right of the camera.
  void getCameraRight(double val[3], int window);

  //! Get the direction that points from the camera to the focal point.
  void getCameraForward(double val[3], int window);

  //! Get the point placement options
  pointPlacementDialog *getPointPlacement() { return m_pointPlacementDialog; }

  //! Read the help index and create the help files.
  void setupHelpFiles();

  //! Get the class that controls the camera position and movement.
  rtCameraControl* getCameraControl(int window = 0) { return m_cameraControl[window]; }

  void setCoordManual(int ct);

 public slots:
  void currItemChanged(QTreeWidgetItem * current, QTreeWidgetItem * previous);
  void itemChanged(QTreeWidgetItem * current, int column);
  void centerOnObject(QTreeWidgetItem *item, int column);
  void viewChangedMixed();
  void viewChanged3DOnly();
  void viewChanged2DOnly();
  void addRenWinPressed();
  void remRenWinPressed();
  void loadPluginFile();

  void addNewObject();
  QList<int> loadObject(QString file = QString());
  void saveObject();
  void removeSelectedObject();
  void renameObject();

  void showAxesOptions();
  void registerDialog();
  void showPointPlacement();

  void textError(bool);
  void textWarning(bool);
  void textDebug(bool);
  void textBenchmark(bool);
  void textLog(bool);
  void showRenderOptions();

  void cameraDefaultView();
  void cameraRobotArmView();

  void showTimeOptionsDialog();

  void add2DFrame();
  void remove2DFrame();
  void removeAll2DFrame();

  void refreshRenderItems(bool);

  void pluginItemChanged(QTreeWidgetItem * current, QTreeWidgetItem * previous);

  void update2DWindowLists(QMultiHash<int, QString>* hash);

  //! The custom context menu for the object tree
  void objectTreeContextMenu(QPoint pos);

  double getMovingQuality() { return m_movingQuality; }
  double getStillQuality() { return m_stillQuality; }

  void activateWhatsThis() { QWhatsThis::enterWhatsThisMode(); }

signals:
  //! Emited when the interaction mode in Vurtigo is switched to camera mode.
  void cameraModeSignal(bool);
  //! Emited when the interaction mode in Vurtigo is switched to interactive mode.
  void interactionModeSignal(bool);
  //! Emited when the interaction mode in Vurtigo is switched to placement mode.
  void placeModeSignal(bool);

 protected:
  //! The widget that handles the 3D rendering window from VTK.
  QList<customQVTKWidget *>m_render3DVTKWidget;
  QHBoxLayout *m_render3DLayout;
  QHBoxLayout *m_objectBrowseLayout;
  QWidget* m_currentObjectWidget;
  QWidget* m_currentPluginWidget;

  QHBoxLayout m_only3DLayout;
  QHBoxLayout m_only2DLayout;
  QHBoxLayout m_dual3DLayout;

  QList<vtkRenderWindow *>m_renWin3D;
  QList<vtkRenderer *>m_renderer3D;
  QList<rtCameraControl *>m_cameraControl;

  //vtkRenderer * m_localRenderer3D;
  //rtCameraControl *m_localCameraControl;

  // The Axes in the corner
  QList<vtkAxesActor *>m_axesActor;
  QList<vtkPropAssembly *>m_propAssembly;
  QList<rtOrientationMarkerWidget *>m_orientationWidget;
  rtAxesProperties* m_axesProperties;

  // the point placement dialog
  pointPlacementDialog* m_pointPlacementDialog;

  // the registration menu
  rtRegistration *m_registrationDialog;

  bool m_renderFlag3D;
  QSemaphore m_renderLock;

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

  //! Moving Quality
  double m_movingQuality;
  //! Quality while the camera is still
  double m_stillQuality;
  
  //! The class that displays all of the help menu windows.
  rtHelpManager *m_helpManager;


  //! The last directory a loaded plugin was chosen from
  QDir m_lastPluginDir;

  //! The number of rendering windows we have
  int m_numRenWin;


  //////////////////////
  // Protected functions
  /////////////////////////

  //! Connect signals to slots for the main window.
  void connectSignals();

  void setupObjectTree();

  //! Create a list of object types and add them to the object list.
  void populateObjectTypeNames();

  void setViewType(rtAxesProperties::ViewType);

  //! Set the coordinate system that is shown by the axes.
  void setCoordType(rtAxesProperties::CoordType);

  void view2DHashCleanup();

  void addNewRenderWindow();

};



#endif
