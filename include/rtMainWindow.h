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

#include <vtkRenderer.h>
#include <vtkAxesActor.h>
#include <vtkPropAssembly.h>
#include <vtkProp.h>
#include <vtkCellPicker.h>
#include <QWhatsThis>

class vtkRenderWindow;

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

class rtRenderObject;
class rtCameraControl;
class rtHelpManager;

#include "ui_rtMainWindow.h"
#include "customQVTKWidget.h"
#include "rtAxesProperties.h"
#include "rtOptions2DView.h"
#include "objTypes.h"
#include "rtOrientationMarkerWidget.h"

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
  vtkRenderWindow* getRenderWindow();

  //! Get the pointer to the renderer
  vtkRenderer* getRenderer();
  QTreeWidget* getObjectTree();
  customQVTKWidget* getRenderWidget() { return m_render3DVTKWidget; }

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

  //! Get the type of interaction the the user has currently selected
  customQVTKWidget::InteractionMode getInteractionMode() { return m_render3DVTKWidget->getInteraction(); }

  //! Check if the camera for the 3D view is moving
  /*!
    Some object types will render in lower quality when the camera is moving.
    \return True if the camera is moving and false otherwise. Also returns false if the camera control object has not yet been created.
    */
  bool cameraMoving();

  //! Get the distance from the focal point to the camera.
  double getCameraDistance();

  //! Get the currently selected prop.
  vtkProp* getSelectedProp();

  //! Get the up direction for the camera.
  void getCameraUp(double val[3]);

  //! Get the direction that points to the right of the camera.
  void getCameraRight(double val[3]);

  //! Get the direction that points from the camera to the focal point.
  void getCameraForward(double val[3]);

  //! Read the help index and create the help files.
  void setupHelpFiles();

  //! Get the class that controls the camera position and movement.
  rtCameraControl* getCameraControl() { return m_cameraControl; }

  void setCoordManual(int ct);

 public slots:
  void currItemChanged(QTreeWidgetItem * current, QTreeWidgetItem * previous);
  void itemChanged(QTreeWidgetItem * current, int column);
  void centerOnObject(QTreeWidgetItem *item, int column);
  void viewChangedMixed();
  void viewChanged3DOnly();
  void viewChanged2DOnly();
  void toggle3DDualView(bool dual);
  void loadPluginFile();

  void addNewObject();
  QList<int> loadObject(QString file = QString());
  void saveObject();
  void removeSelectedObject();

  void showAxesOptions();

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

  void pluginItemChanged(QTreeWidgetItem * current, QTreeWidgetItem * previous);

  void update2DWindowLists(QMultiHash<int, QString>* hash);

  //! The camera mode button was pressed.
  void cameraModeToggled(bool);
  //! The interaction mode button was pressed.
  void interactionModeToggled(bool);
  //! The placement mode button was pressed.
  void placeModeToggled(bool);

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
  customQVTKWidget *m_render3DVTKWidget;
  QHBoxLayout *m_render3DLayout;
  QHBoxLayout *m_objectBrowseLayout;
  QWidget* m_currentObjectWidget;
  QWidget* m_currentPluginWidget;

  QHBoxLayout m_only3DLayout;
  QHBoxLayout m_only2DLayout;

  vtkRenderWindow *m_renWin3D;
  vtkRenderer *m_renderer3D;
  rtCameraControl *m_cameraControl;

  vtkRenderer * m_localRenderer3D;
  rtCameraControl *m_localCameraControl;

  // The Axes in the corner
  vtkAxesActor *m_axesActor;
  vtkPropAssembly *m_propAssembly;
  rtOrientationMarkerWidget *m_orientationWidget;
  rtAxesProperties* m_axesProperties;

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

};



#endif
