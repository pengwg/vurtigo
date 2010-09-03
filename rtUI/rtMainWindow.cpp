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
#include <QtGui>
#include <QApplication>
#include <QFileDialog>

#include <vtkPropAssembly.h>
#include <vtkCamera.h>

#include <iostream>
#include <cmath>

#include "rtMainWindow.h"
#include "rtObjectManager.h"
#include "rtRenderObject.h"
#include "rtDataObject.h"
#include "rtPluginLoader.h"
#include "rtMessage.h"
#include "rtRenderOptions.h"
#include "rtCameraControl.h"
#include "ui_newObjectDialog.h"
#include "rtConfigOptions.h"
#include "buildParam.h"
#include "rtHelpManager.h"
#include "rtApplication.h"
#include "rtTimeManager.h"

#define USE_NEW_MESSAGES

rtMainWindow::rtMainWindow(QWidget *parent, Qt::WindowFlags flags) {

#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::rtMainWindow() start") );
#endif

  setupUi(this);

  m_cellPicker = vtkCellPicker::New();
  m_helpManager = NULL;
  m_currentObjectWidget = NULL;
  m_currentPluginWidget = NULL;
  m_renderFlag3D = false;
  m_renderLock.release(); // Must start with one resource.

  m_render3DVTKWidget = new customQVTKWidget(this->frame3DRender);
  m_render3DVTKWidget->setMinimumSize(300,300);
  m_render3DLayout = new QHBoxLayout();
  m_render3DLayout->setContentsMargins(0,0,0,0);
  m_inter3D = m_render3DVTKWidget->GetInteractor();
  m_renWin3D = m_inter3D->GetRenderWindow();

  // Stereo rendering causes bugs in OSX.
#ifdef Q_WS_MAC

#else
  m_renWin3D->StereoCapableWindowOn();
#endif

  m_renderer3D = vtkRenderer::New();
  m_renWin3D->AddRenderer(m_renderer3D);

  m_cameraControl = new rtCameraControl( m_renderer3D->GetActiveCamera(), m_render3DVTKWidget );
  m_cameraControl->setToDefaultPosition();

  m_movingQuality = 0.5f;
  m_stillQuality = 1.0f;

  m_axesActor = vtkAxesActor::New();
  m_propAssembly = vtkPropAssembly::New();
  m_orientationWidget = new rtOrientationMarkerWidget(m_render3DVTKWidget);

#ifdef USE_NEW_MESSAGES
  m_inter3D->Disable();
#endif

  m_propAssembly->AddPart( m_axesActor );
  m_orientationWidget->SetOrientationMarker(m_propAssembly);

  m_orientationWidget->SetInteractor( m_inter3D );
  m_orientationWidget->SetViewport( 0.75, 0.0, 1.0, 0.25 );

  m_axesProperties = new rtAxesProperties();

  if (m_axesProperties) {
    setViewType( m_axesProperties->getViewType() );
    setCoordType( m_axesProperties->getCoordType() );
  }

  m_render3DLayout->addWidget(m_render3DVTKWidget);
  this->frame3DRender->setLayout(m_render3DLayout);

  // The GUI must be stretched more than the tabs. 
  mainRLSplitter->setStretchFactor(0, 1);
  mainRLSplitter->setStretchFactor(1, 5);

  // The 3D view must be larger than the 2D view. 
  mainUDSplitter->setStretchFactor(0, 2);
  mainUDSplitter->setStretchFactor(1, 1);

  m_objectBrowseLayout = new QHBoxLayout();
  objBrowseFrame->setLayout(m_objectBrowseLayout);

  view3D->setLayout(&m_only3DLayout);
  view2D->setLayout(&m_only2DLayout);

  // Remove spacing
  m_only3DLayout.setSpacing(0);
  m_only2DLayout.setSpacing(0);
  dualView->layout()->setSpacing(0);

  // Remove borders
  m_only3DLayout.setContentsMargins(0,0,0,0);
  m_only2DLayout.setContentsMargins(0,0,0,0);
  dualView->layout()->setContentsMargins(0,0,0,0);

  m_view2DHash.clear();
  m_max2DWidgets = 100;

  scrollAreaWidget->setLayout(&m_scrollArea2DImagesLayout);

  pluginBrowseFrame->setLayout(&m_pluginWidgetLayout);

  // the default is camera mode.
  actionCamera_Mode->setChecked(true);
  actionInteraction_Mode->setChecked(false);
  actionPlacement_Mode->setChecked(false);

#ifdef DEBUG_VERBOSE_MODE_ON
  actionDebugText->setChecked(true);
#endif

  viewChangedMixed();
  populateObjectTypeNames();
  connectSignals();
  setupObjectTree();

#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::rtMainWindow() end") );
#endif
}

rtMainWindow::~rtMainWindow() {
  m_renderFlag3D = false;

  m_cellPicker->Delete();

  if (m_cameraControl) delete m_cameraControl;

  clearPluginList();
  clearObjectList();

  // Remove any remaining widget from the dialog. 
  if (m_currentObjectWidget) {
    m_objectBrowseLayout->removeWidget(m_currentObjectWidget);
    m_currentObjectWidget->setParent(NULL);
    m_currentObjectWidget = NULL;
  }

  // Cleanup the 2D widget hash
  view2DHashCleanup();

  if (m_render3DVTKWidget) delete m_render3DVTKWidget;
  if (m_render3DLayout) delete m_render3DLayout;
  if (m_objectBrowseLayout) delete m_objectBrowseLayout;

  if (m_renderer3D) m_renderer3D->Delete();
  if (m_axesActor) m_axesActor->Delete();
  if (m_propAssembly) m_propAssembly->Delete();
  if (m_orientationWidget) {
    m_orientationWidget->SetInteractor(NULL);
    m_orientationWidget->Delete();
    m_orientationWidget = NULL;
  }
  if (m_axesProperties) delete m_axesProperties;
  if (m_helpManager) delete m_helpManager;
}


void rtMainWindow::setupHelpFiles() {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::setupHelpFiles() start") );
#endif

  // The help engine
  m_helpManager = new rtHelpManager();

  // Help Menu
  connect(actionContents, SIGNAL(triggered()), m_helpManager, SLOT(showHelpContents()));
  connect(actionSource_Docs, SIGNAL(triggered()), m_helpManager, SLOT(showSourceDocs()));
  connect(actionAbout, SIGNAL(triggered()), m_helpManager, SLOT(showHelpAbout()));

#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::setupHelpFiles() end") );
#endif
}

vtkRenderWindow* rtMainWindow::getRenderWindow() {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::getRenderWindow(): ").append( QString::number((long)m_renWin3D, 16) ) );
#endif
  return m_renWin3D;
}

vtkRenderWindowInteractor* rtMainWindow::getInteractor() {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::getInteractor(): ").append( QString::number((long)m_inter3D, 16) ) );
#endif
  return m_inter3D;
}

//! Get the vtkRenderer object for the 3D view. 
vtkRenderer* rtMainWindow::getRenderer() {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::getRenderer(): ").append( QString::number((long)m_renderer3D, 16) ) );
#endif
  return m_renderer3D;
}

//! Get the object tree
QTreeWidget* rtMainWindow::getObjectTree() {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::getObjectTree(): ").append( QString::number((long)objectTree, 16) ) );
#endif
  return objectTree;
}

//! Called with the newest set of render objects. 
void rtMainWindow::updateObjectList(QHash<int, rtRenderObject*>* hash) {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::updateObjectList() start") );
#endif

  QHash<int, rtRenderObject*>::iterator i;
  QHash<rtConstants::rtObjectType, QTreeWidgetItem *>::iterator wIter;
  bool objExists;

  if (!hash) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("In updateObjectList() hash pointer is NULL!") );
    return;
  }

  // Update all the items.
  for (i = hash->begin(); i != hash->end(); ++i) {
    i.value()->updateTreeItem();
  }

  if (objectTree->topLevelItemCount() == 0) {
    // Setup the new items.
    for (wIter = m_topItems.begin(); wIter!=m_topItems.end(); ++wIter){
      objectTree->addTopLevelItem(wIter.value());
    }
  }

  for (int ix1=0; ix1<objectTree->topLevelItemCount(); ix1++) {
    for (int ix2=0; ix2<objectTree->topLevelItem(ix1)->childCount(); ix2++) {
      objExists = false;
      for (i = hash->begin(); i != hash->end(); ++i) {
        if(i.value()->getTreeItem() == objectTree->topLevelItem(ix1)->child(ix2)) objExists = true;
      }
      if(!objExists) objectTree->topLevelItem(ix1)->takeChild(ix2);
    }
  }

  for (i = hash->begin(); i != hash->end(); ++i) {
    i.value()->updateTreeItem();
    if ( m_topItems.contains(i.value()->getObjectType()) ) {
      if ( m_topItems.value(i.value()->getObjectType())->indexOfChild(i.value()->getTreeItem()) == -1 ) {
        m_topItems.value(i.value()->getObjectType())->addChild(i.value()->getTreeItem());
      }
    } else {
      QString warn;
      warn = "Could not find category: ";
      warn.append( QString::number(i.value()->getObjectType()) );
      warn.append( " for object: ");
      warn.append( QString::number((long)i.value(), 16) );
      rtApplication::instance().getMessageHandle()->warning(__LINE__, __FILE__, warn);
    }
  }

#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::updateObjectList() end") );
#endif
}

//! Empty the object list from the GUI
void rtMainWindow::clearObjectList() {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::clearObjectList() start") );
#endif
  while (objectTree->takeTopLevelItem(0));
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::clearObjectList() end") );
#endif
}

//! Update the GUI with a new set of pligins.
void rtMainWindow::updatePluginList(QHash<int, QTreeWidgetItem*>* hash) {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::updatePluginList() start") );
#endif

   if (!hash) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("In updatePluginList() hash pointer is NULL!") );
    return;
  }

  if (!pluginTree) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("In updatePluginList() pluginTree pointer is NULL!") );
    return;
  }

  QHash<int, QTreeWidgetItem*>::iterator i;

  while (pluginTree->takeTopLevelItem(0));

  for (i = hash->begin(); i != hash->end(); ++i) {
    pluginTree->addTopLevelItem(i.value());
  }

#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::updatePluginList() end") );
#endif
}

//! Empty the plugin list from the GUI.
void rtMainWindow::clearPluginList() {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::clearPluginList() start") );
#endif

  if (!pluginTree) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("In clearPluginList() pluginTree pointer is NULL!") );
    return;
  }

  while (pluginTree->takeTopLevelItem(0));

#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::clearPluginList() end") );
#endif
}

//! This slot is called every time a new object is selected in the tree. 
void rtMainWindow::currItemChanged(QTreeWidgetItem * current, QTreeWidgetItem * previous) {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::currItemChanged() start") );
#endif

  rtRenderObject *temp=NULL;
  QWidget * baseWid=NULL;

  if (!current) {
#ifdef DEBUG_VERBOSE_MODE_ON
    rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::currItemChanged() no current item ignore call") );
#endif
    return;
  }

  // Check for a heading. 
  if (current->columnCount() == 1) {
    if (m_currentObjectWidget) {
      m_objectBrowseLayout->removeWidget(m_currentObjectWidget);
      m_currentObjectWidget->setParent(NULL);
      m_currentObjectWidget = NULL;
    }
#ifdef DEBUG_VERBOSE_MODE_ON
    rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::currItemChanged() current item is heading ignore call") );
#endif
    return;
  }

  if (m_currentObjectWidget) {
    m_objectBrowseLayout->removeWidget(m_currentObjectWidget);
    m_currentObjectWidget->setParent(NULL);
  }

  temp = rtApplication::instance().getObjectManager()->getObjectWithID(current->text(1).toInt());
  baseWid = temp->getDataObject()->getBaseWidget();

  if (!baseWid) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("In currItemChanged() baseWid pointer is NULL!") );
    return;
  }

  m_objectBrowseLayout->addWidget(baseWid);
  m_currentObjectWidget = baseWid;

#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::currItemChanged() end") );
#endif
}

//! Called when the content of a tree item has changed.
/*!
  The main element that changes is the state of the check box. This function will add or remove elements form the renderer based on the state of the check box.
  */
void rtMainWindow::itemChanged(QTreeWidgetItem * current, int column) {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::itemChanged() start") );
#endif

  rtRenderObject *temp=NULL;

  if (!current) {
#ifdef DEBUG_VERBOSE_MODE_ON
    rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::itemChanged() no current item ignore call") );
#endif
    return;
  }

  // Check for a heading. 
  if (current->columnCount() == 1) {
#ifdef DEBUG_VERBOSE_MODE_ON
    rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::itemChanged() current item is heading ignore call") );
#endif
    return;
  }

  // Get the object
  temp = rtApplication::instance().getObjectManager()->getObjectWithID(current->text(1).toInt());

  // If the box is checked then add it to the renderer.
  if (current->checkState(column) == Qt::Checked) {
    if ( temp->addToRenderer(m_renderer3D) ) {
      m_renderFlag3D = true;
    }
  } else {
    if ( temp->removeFromRenderer(m_renderer3D) ) {
      m_renderFlag3D = true;
    }
  }

#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::itemChanged() end") );
#endif
}

//! User has double clicked on an item so we need to re-center on it.
/*!
  */
void rtMainWindow::centerOnObject(QTreeWidgetItem *item, int column) {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::centerOnObject() start") );
#endif

  rtRenderObject *temp;

  if (!item) {
    rtApplication::instance().getMessageHandle()->warning(__LINE__, __FILE__, QString("rtMainWindow::centerOnObject() item to center on is NULL"));
    return;
  }

  // Check for a heading.
  if (item->columnCount() == 1) {
#ifdef DEBUG_VERBOSE_MODE_ON
    rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::centerOnObject() item is heading. Ignored.") );
#endif
    return;
  }

  // Get the object
  temp = rtApplication::instance().getObjectManager()->getObjectWithID(item->text(1).toInt());
  if (!temp->getVisible3D()) {
    // Not visible so we have nothing to center on...
    return;
  }
  double loc[6];
  if (temp->getObjectLocation(loc)) {
    // Location is valid
    m_renderer3D->ResetCamera(loc);
    m_renderFlag3D = true;
  }

#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::centerOnObject() end") );
#endif
}

//! Render items can be added directly.
/*!
  Note that this function does not keep track of the objects it adds.
 */
void rtMainWindow::addRenderItem(vtkProp* prop) {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::addRenderItem() start") );
#endif
  if (!prop) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("addRenderItem() Prop to add is NULL. Not Added."));
    return;
  }

  if (!m_renderer3D->HasViewProp(prop)) {
    m_renderer3D->AddViewProp(prop);
    m_renderFlag3D = true;
  }
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::addRenderItem() end") );
#endif
}

//! Render items can be removed directly. 
/*!
  Note that this function does not keep track of the objects it removes.
 */
void rtMainWindow::removeRenderItem(vtkProp* prop) {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::removeRenderItem() start") );
#endif

  if (!prop) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("removeRenderItem() Prop to remove is NULL. Not Added."));
    return;
  }

  if (m_renderer3D->HasViewProp(prop)) {
    m_renderer3D->RemoveViewProp(prop);
    m_renderFlag3D = true;
  }
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::removeRenderItem() end") );
#endif
}



//! Called when the user clicks on a different plugin in the viewer.
/*!
  When a new plugin is selected in the list this function is called.
  @param current The pointer to the element selected or NULL is if none was selected.
  @param previous Pointer to the last selected element.
 */
void rtMainWindow::pluginItemChanged(QTreeWidgetItem * current, QTreeWidgetItem * previous) {
  int id=0;
  bool convOK=false;
  QWidget* plugWidget=NULL;

#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::pluginItemChanged() start") );
#endif

  // Empty the layout.
  if (m_currentPluginWidget) {
    m_pluginWidgetLayout.removeWidget(m_currentPluginWidget);
    m_currentPluginWidget->setParent(NULL);
    m_currentPluginWidget->hide();
    m_currentPluginWidget = NULL;
  }

  // Check if there is a new item to use.
  if (!current) {
#ifdef DEBUG_VERBOSE_MODE_ON
    rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::pluginItemChanged() current tree item is NULL. Ignore.") );
#endif
    return;
  }

  // First column contains the unique ID
  id = current->text(0).toInt(&convOK, 10);

  if (!convOK) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("rtMainWindow::pluginItemChanged() failed to convert string to integer ID: ").append(current->text(0)));
    return;
  }

  plugWidget = rtApplication::instance().getPluginLoader()->getPluginWithID(id)->getWidget();
  m_currentPluginWidget = plugWidget;
  if (plugWidget) {
    m_pluginWidgetLayout.addWidget(plugWidget);
    plugWidget->show();
  }

#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::pluginItemChanged() end") );
#endif
}

//! Try to render the 3D window.
/*!
  This function will only render if the render flag has been set to true. Once the function runs it will reset the flag back to false.
 */
void rtMainWindow::tryRender3D() {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::tryRender3D() start") );
#endif
  if (m_renderFlag3D && m_renderLock.tryAcquire()) {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::tryRender3D() about to render") );
#endif
    m_renWin3D->Render();
    m_renderFlag3D = false;
    m_renderLock.release();
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::tryRender3D() render finished lock released.") );
#endif
  }
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::tryRender3D() end") );
#endif
}

//! Called when the timer decides it is a good time to update the 2D views.
/*!
  Try to update each of the 2D views. 2D views that do not have valid 'current objects' will not be updated.
  */
void rtMainWindow::update2DViews() {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::update2DViews() start") );
#endif
  QList<int> keyL;
  rtRenderObject* renObj=NULL;

  keyL = m_view2DHash.keys();
  for (int ix1=0; ix1<keyL.size(); ix1++) {
    renObj = m_view2DHash.value(keyL[ix1])->getCurrRenObj();
    if (renObj) {
      // Check if we need to update this
      if (renObj->updateNeeded()) {
        renObj->tryUpdate();
        m_view2DHash.value(keyL[ix1])->renderOn();
      }
      // Render it! (Force a render)
      m_view2DHash.value(keyL[ix1])->renderOn();
      m_view2DHash.value(keyL[ix1])->tryRender();

    }
  }
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::update2DViews() end") );
#endif
}

bool rtMainWindow::cameraMoving() {
  if (!m_cameraControl) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("rtMainWindow::cameraMoving() cameraControl is NULL: "));
    return false;
  }
  return m_cameraControl->cameraMoving();
}

double rtMainWindow::getCameraDistance() {
  return m_renderer3D->GetActiveCamera()->GetDistance();
}

void rtMainWindow::getCameraUp(double val[3]) {
  if (!m_cameraControl) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("rtMainWindow::getCameraUp() cameraControl is NULL: "));
    return;
  }
  m_cameraControl->getUpDirection(val);
}

void rtMainWindow::getCameraRight(double val[3]) {
  if (!m_cameraControl) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("rtMainWindow::getCameraRight() cameraControl is NULL: "));
    return;
  }
  m_cameraControl->getRightDirection(val);
}

void rtMainWindow::getCameraForward(double val[3]) {
  if (!m_cameraControl) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("rtMainWindow::getCameraForward() cameraControl is NULL: "));
    return;
  }
  m_cameraControl->getForwardDirection(val);
}

vtkProp* rtMainWindow::getSelectedProp() {
  if (!m_render3DVTKWidget) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("rtMainWindow::getSelectedProp() 3D render widget pointer is NULL: "));
    return NULL;
  }
  return m_render3DVTKWidget->getChosenProp();
}

//! Connect the menu actions to the functions that handle them.
void rtMainWindow::connectSignals() {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::connectSignals() start") );
#endif

  // File Menu
  connect(actionPlugin_File, SIGNAL(triggered()), this, SLOT(loadPluginFile()));
  connect(actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));

  // Object Menu
  connect( actionNewObject, SIGNAL(triggered()), this, SLOT(addNewObject()) );
  connect( actionLoad_Object, SIGNAL(triggered()), this, SLOT(loadObject()) );
  connect( actionSave_Object, SIGNAL(triggered()), this, SLOT(saveObject()) );
  connect( actionDeleteSelected, SIGNAL(triggered()), this, SLOT(removeSelectedObject()) );

  // View Menu
  connect(actionMixed, SIGNAL(triggered()), this, SLOT(viewChangedMixed()));
  connect(action3D_Only, SIGNAL(triggered()), this, SLOT(viewChanged3DOnly()));
  connect(action2D_Only, SIGNAL(triggered()), this, SLOT(viewChanged2DOnly()));
  connect(actionCoordinate_Axes, SIGNAL(triggered()), this, SLOT(showAxesOptions()));
  connect(actionErrorText, SIGNAL(toggled(bool)), this, SLOT(textError(bool)));
  connect(actionWarningText, SIGNAL(toggled(bool)), this, SLOT(textWarning(bool)));
  connect(actionDebugText, SIGNAL(toggled(bool)), this, SLOT(textDebug(bool)));
  connect(actionBenchmarkText, SIGNAL(toggled(bool)), this, SLOT(textBenchmark(bool)));
  connect(actionLogText, SIGNAL(toggled(bool)), this, SLOT(textLog(bool)));
  connect(action3D_Render_Options, SIGNAL(triggered()), this, SLOT(showRenderOptions()));
  connect(actionDefault_View, SIGNAL(triggered()), this , SLOT(cameraDefaultView()));
  connect(actionRobot_Arm, SIGNAL(triggered()), this, SLOT(cameraRobotArmView()));
  connect(actionTimer_Options, SIGNAL(triggered()), this, SLOT(showTimeOptionsDialog()));

  // 2D View controls
  connect(add2DFrameBtn, SIGNAL(clicked()), this, SLOT(add2DFrame()));
  connect(remove2DFrameBtn, SIGNAL(clicked()), this, SLOT(remove2DFrame()));
  connect(removeAll2DFrameBtn, SIGNAL(clicked()), this, SLOT(removeAll2DFrame()));

  // Object Tree
  connect(objectTree, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(currItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
  connect(objectTree, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(itemChanged(QTreeWidgetItem*,int)));
  connect(objectTree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(centerOnObject(QTreeWidgetItem*,int)));

  // Plugin Tree
  connect(pluginTree, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(pluginItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));

  // Menu Bar Only
  connect(actionCamera_Mode, SIGNAL(toggled(bool)), this, SLOT(cameraModeToggled(bool)));
  connect(actionInteraction_Mode, SIGNAL(toggled(bool)), this, SLOT(interactionModeToggled(bool)));
  connect(actionPlacement_Mode, SIGNAL(toggled(bool)), this, SLOT(placeModeToggled(bool)));

#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::connectSignals() end") );
#endif
}

void rtMainWindow::setupObjectTree() {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::setupObjectTree() start") );
#endif

  QHash<rtConstants::rtObjectType, QString>::iterator i;
  QTreeWidgetItem * temp=NULL;

  m_topItems.clear();

  for (i = m_rtObjectTypeNames.begin(); i !=  m_rtObjectTypeNames.end(); ++i) {
    temp = new QTreeWidgetItem();
    temp->setText(0, i.value());
    m_topItems.insert(i.key(), temp);
  }
  objectTree->setColumnWidth(0, 150);
  objectTree->setColumnWidth(1, 60);

#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::setupObjectTree() end") );
#endif
}

//! Assign string names to the object types.
/*!
  With string names the objects can be displayed in the GUI.
*/
void rtMainWindow::populateObjectTypeNames() {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::populateObjectTypeNames() start") );
#endif
  m_rtObjectTypeNames.insert(rtConstants::OT_3DObject, "3D Object");
  m_rtObjectTypeNames.insert(rtConstants::OT_2DObject, "2D Object");
  m_rtObjectTypeNames.insert(rtConstants::OT_Cath, "Catheter");
  m_rtObjectTypeNames.insert(rtConstants::OT_vtkMatrix4x4, "4 by 4 vtkMatrix");
  m_rtObjectTypeNames.insert(rtConstants::OT_vtkPolyData, "Polygon Data");
  m_rtObjectTypeNames.insert(rtConstants::OT_vtkPiecewiseFunction, "Piecewise Function");
  m_rtObjectTypeNames.insert(rtConstants::OT_vtkColorTransferFunction, "Color Function");
  m_rtObjectTypeNames.insert(rtConstants::OT_ImageBuffer, "Image Buffer");
  m_rtObjectTypeNames.insert(rtConstants::OT_2DPointBuffer, "2D Point Buffer");
  m_rtObjectTypeNames.insert(rtConstants::OT_3DPointBuffer, "3D Point Buffer");
  m_rtObjectTypeNames.insert(rtConstants::OT_TextLabel, "Text Label");
  m_rtObjectTypeNames.insert(rtConstants::OT_EPMesh, "EP Mesh");
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::populateObjectTypeNames() end") );
#endif
}

//! Change the view to mixed mode where both the 2D and 3D windows are visible.
void rtMainWindow::viewChangedMixed() {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::viewChangedMixed() start") );
#endif
  if (actionMixed->isChecked()) {
    stackedWidget->setCurrentIndex(0);
    mainUDSplitter->insertWidget(0, frame3DRender);
    mainUDSplitter->insertWidget(1, lowerFrame);
    actionMixed->setChecked(true);
    action2D_Only->setChecked(false);
    action3D_Only->setChecked(false);
  } else {
    actionMixed->setChecked(true);
  }
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::viewChangedMixed() end") );
#endif
}

//! Make only the 3D render window visible.
void rtMainWindow::viewChanged3DOnly() {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::viewChanged3DOnly() start") );
#endif
  if (action3D_Only->isChecked()) {
    stackedWidget->setCurrentIndex(1);
    m_only3DLayout.addWidget(frame3DRender);
    actionMixed->setChecked(false);
    action3D_Only->setChecked(true);
    action2D_Only->setChecked(false);
  } else {
    action3D_Only->setChecked(true);
  }
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::viewChanged3DOnly() start") );
#endif
}

//! Make only the 2D window visible
void rtMainWindow::viewChanged2DOnly() {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::viewChanged2DOnly() start") );
#endif
  if (action2D_Only->isChecked()) {
    stackedWidget->setCurrentIndex(2);
    m_only2DLayout.addWidget(lowerFrame);
    actionMixed->setChecked(false);
    action3D_Only->setChecked(false);
    action2D_Only->setChecked(true);
  } else {
    action2D_Only->setChecked(true);
  }
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::viewChanged2DOnly() end") );
#endif
}

//! Update  the lists for all the 2D windows.
void rtMainWindow::update2DWindowLists(QMultiHash<int, QString>* hash) {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::update2DWindowLists() start") );
#endif
  QList<int> keyL;
  keyL = m_view2DHash.keys();
  for (int ix1=0; ix1<m_view2DHash.size(); ix1++) {
    m_view2DHash[keyL[ix1]]->setStringList(hash);
  }
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::update2DWindowLists() end") );
#endif
}



//! Bring up the dialog that gives the options for the axes widget
void rtMainWindow::showAxesOptions() {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::showAxesOptions() start") );
#endif
  if (!m_axesProperties) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("rtMainWindow::showAxesOptions() m_axesProperties is NULL: "));
    return;
  }

  if (m_axesProperties->exec() == QDialog::Accepted) {
    setViewType( m_axesProperties->getViewType() );
    setCoordType( m_axesProperties->getCoordType() );
  }
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::showAxesOptions() end") );
#endif
}

void rtMainWindow::textError(bool t) {
  rtApplication::instance().getMessageHandle()->m_err = t;
}

void rtMainWindow::textWarning(bool t) {
  rtApplication::instance().getMessageHandle()->m_warn = t;
}

void rtMainWindow::textDebug(bool t) {
  rtApplication::instance().getMessageHandle()->m_debug = t;
}

void rtMainWindow::textBenchmark(bool t) {
  rtApplication::instance().getMessageHandle()->m_bench = t;
}

void rtMainWindow::textLog(bool t) {
  rtApplication::instance().getMessageHandle()->m_log = t;
}

//! Display the render options dialog box.
void rtMainWindow::showRenderOptions() {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::showRenderOptions() start") );
#endif

  rtRenderOptions renOpt;

  renOpt.renQuality(m_stillQuality*100.0f);
  renOpt.renUpdateQuality(m_movingQuality*100.0f);

  // Stereo is broken on OSX
#ifdef Q_WS_MAC
  renOpt.removeStereoFeature();
#else
  if (m_renWin3D->GetStereoRender()) {
    renOpt.setStereoType(m_renWin3D->GetStereoType());
  } else {
    renOpt.setStereoType(0);
  }
#endif

  renOpt.setDirectRender(m_renWin3D->IsDirect());
  renOpt.setGLRender(m_renWin3D->SupportsOpenGL());

  if(renOpt.exec() == QDialog::Accepted) {  
    double renUpQual = renOpt.getRenUpdateQuality();
    double renQual = renOpt.getRenQuality();
    m_stillQuality = renQual/100.0f;
    m_movingQuality = renUpQual/100.0f;

    // Stereo is broken on OSX
#ifdef Q_WS_MAC

#else
    int type = renOpt.getStereoType();
    if (type == 0) {
      m_renWin3D->StereoRenderOff();
    } else {
      m_renWin3D->StereoRenderOn();
      m_renWin3D->SetStereoType(type);
    }
#endif

    m_renderFlag3D = true;
  }

#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::showRenderOptions() end") );
#endif
}

void rtMainWindow::cameraDefaultView() {
  m_cameraControl->setToDefaultPosition();
  m_renderFlag3D = true;
}


void rtMainWindow::cameraRobotArmView() {
  m_cameraControl->setToRobotArmPosition();
  m_renderFlag3D = true;
}

void rtMainWindow::showTimeOptionsDialog() {
  if (rtApplication::instance().getTimeManager())
    rtApplication::instance().getTimeManager()->showTimeOptionsDialog();
}

//! Load a plugin based on an XML file the user chooses. 
void rtMainWindow::loadPluginFile() {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::loadPluginFile() start") );
#endif
  QString fName;

  fName = QFileDialog::getOpenFileName(this, "Select Plugin XML File", ".", "*.XML *.xml");

  QFile file(fName);
  if (!file.exists()) {
    rtApplication::instance().getMessageHandle()->warning(__LINE__, __FILE__, QString("rtMainWindow::loadPluginFile() File does not exist: ").append(fName) );
    return;
  }

  if (!rtApplication::instance().getPluginLoader()->loadPluginsFromConfig(&file)) {
    rtApplication::instance().getMessageHandle()->warning(__LINE__, __FILE__, QString("rtMainWindow::loadPluginFile() Failed to load plugins from: ").append(fName) );
  }
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::loadPluginFile() end") );
#endif
}


//! Add an object of a particular type.
void rtMainWindow::addNewObject() {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::addNewObject() start") );
#endif

  QDialog *objDlg = new QDialog(this);
  Ui::newObjectDialog setupDlg;
  QString name;

  setupDlg.setupUi(objDlg);
  if(objDlg->exec() == QDialog::Accepted) {
    name = setupDlg.objNameLineEdit->text();
    if (name.length() <= 0) name = "Not Named";
    if (setupDlg.objTypeCombo->currentText() == "Piecewise Function") {
      rtApplication::instance().getObjectManager()->addObjectOfType(rtConstants::OT_vtkPiecewiseFunction, name);
    } else if (setupDlg.objTypeCombo->currentText() == "Color Transfer Function") {
      rtApplication::instance().getObjectManager()->addObjectOfType(rtConstants::OT_vtkColorTransferFunction, name);
    }
  }

#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::addNewObject() end") );
#endif
}

void rtMainWindow::loadObject() {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::loadObject() start") );
#endif

  QFile file;
  QStringList names;

  names = QFileDialog::getOpenFileNames(this, "Select Files To Open:");
  for (int ix1=0; ix1<names.size(); ix1++) {
    file.setFileName(names.at(ix1));
    if (file.exists() && file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      QXmlStreamReader reader(&file);
      QXmlStreamReader::TokenType type;
      bool typeRead=false;
      bool nameRead=false;
      rtConstants::rtObjectType objType;
      QString objName="";

      while (!reader.atEnd()) {
        type = reader.readNext();
        if(type == QXmlStreamReader::StartElement && reader.name() == "FileInfo") {
          rtDataObject::loadHeader(&reader, objType, objName);
        }
      }
      if (reader.hasError()) {
        rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("rtMainWindow::loadObject() QXmlStreamReader reports an error: ").append(reader.errorString()) );
      }

      if (objType != rtConstants::OT_None) typeRead = true;
      if (objName != "") nameRead = true;

      rtRenderObject* obj;
      QFile objFile;
      // Both the type and the name have been recognized.
      if (typeRead && nameRead) {
        obj = rtApplication::instance().getObjectManager()->addObjectOfType(objType, objName);
        if (obj) {
          objFile.setFileName(names.at(ix1));
          obj->loadFile(&objFile);
        }
      }

      file.close();
    }
  }
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::loadObject() end") );
#endif
}

void rtMainWindow::saveObject() {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::saveObject() start") );
#endif

  QTreeWidgetItem* current=NULL;
  rtRenderObject* temp=NULL;
  QString fName="";
  QFile file;

  current = objectTree->currentItem();

  if (!current) return;

  // Check for a heading.
  if (current->columnCount() == 1) {
    return;
  }

  // Find the object
  temp = rtApplication::instance().getObjectManager()->getObjectWithID(current->text(1).toInt());
  if (temp) {
    // Find the file name
    fName = QFileDialog::getSaveFileName(this, "Save As...");
    file.setFileName(fName);
    temp->saveFile(&file);
  }
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::saveObject() end") );
#endif
}

//! Remove the currently selected item.
void rtMainWindow::removeSelectedObject() {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::removeSelectedObject() start") );
#endif

  QTreeWidgetItem* current=NULL;
  rtRenderObject* temp=NULL;

  current = objectTree->currentItem();

  if (!current) return;

  // Check for a heading.
  if (current->columnCount() == 1) {
    return;
  }

  // Get the object
  temp = rtApplication::instance().getObjectManager()->getObjectWithID(current->text(1).toInt());
  temp->removeFromRenderer(m_renderer3D);
  m_renderFlag3D = true;
  rtApplication::instance().getObjectManager()->removeObject(current->text(1).toInt());
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::removeSelectedObject() end") );
#endif
}


//! Set the view type for the coordinate axes
void rtMainWindow::setViewType(rtAxesProperties::ViewType vt) {
  switch (vt) {
    case(rtAxesProperties::VT_NONE):
    m_orientationWidget->SetEnabled(0);
    break;
    case(rtAxesProperties::VT_VISIBLE):
    m_orientationWidget->SetEnabled(1);
    m_orientationWidget->InteractiveOff();
    break;
    case(rtAxesProperties::VT_INTERACT):
    m_orientationWidget->SetEnabled(1);
    m_orientationWidget->InteractiveOn();
    break;
  }
  m_renderFlag3D = true;
}

void rtMainWindow::setCoordType(rtAxesProperties::CoordType ct) {
  switch (ct) {
    case (rtAxesProperties::CT_PATIENT):
    m_axesActor->SetXAxisLabelText("Left");
    m_axesActor->SetYAxisLabelText("Posterior");
    m_axesActor->SetZAxisLabelText("Superior");
    break;
    case (rtAxesProperties::CT_VTK):
    m_axesActor->SetXAxisLabelText("X");
    m_axesActor->SetYAxisLabelText("Y");
    m_axesActor->SetZAxisLabelText("Z");
    break;
  }
  m_renderFlag3D = true;
}

void rtMainWindow::cameraModeToggled(bool toggle) {
  if (m_render3DVTKWidget->getInteraction() != customQVTKWidget::CAMERA_MODE && toggle) {
    m_render3DVTKWidget->setInteraction(customQVTKWidget::CAMERA_MODE);
    actionCamera_Mode->setChecked(true);
    actionInteraction_Mode->setChecked(false);
    actionPlacement_Mode->setChecked(false);
  } else if (m_render3DVTKWidget->getInteraction() == customQVTKWidget::CAMERA_MODE && !toggle) {
    // Prevent the user from turning all the options off.
    actionCamera_Mode->setChecked(true);
  }
  emit cameraModeSignal(actionCamera_Mode->isChecked());
}

void rtMainWindow::interactionModeToggled(bool toggle) {
  if (m_render3DVTKWidget->getInteraction() != customQVTKWidget::INTERACTION_MODE && toggle) {
    m_render3DVTKWidget->setInteraction(customQVTKWidget::INTERACTION_MODE);
    actionCamera_Mode->setChecked(false);
    actionInteraction_Mode->setChecked(true);
    actionPlacement_Mode->setChecked(false);
  } else if (m_render3DVTKWidget->getInteraction() == customQVTKWidget::INTERACTION_MODE && !toggle) {
    // Prevent the user from turning all the options off.
    actionInteraction_Mode->setChecked(true);
  }
  emit interactionModeSignal(actionInteraction_Mode->isChecked());
}

void rtMainWindow::placeModeToggled(bool toggle) {
  if (m_render3DVTKWidget->getInteraction() != customQVTKWidget::PLACE_MODE && toggle) {
    m_render3DVTKWidget->setInteraction(customQVTKWidget::PLACE_MODE);
    actionCamera_Mode->setChecked(false);
    actionInteraction_Mode->setChecked(false);
    actionPlacement_Mode->setChecked(true);
  } else if (m_render3DVTKWidget->getInteraction() == customQVTKWidget::PLACE_MODE && !toggle) {
    // Prevent the user from turning all the options off.
    actionPlacement_Mode->setChecked(true);
  }
  emit placeModeSignal(actionPlacement_Mode->isChecked());
}


//! Create a new 2D widget and return the ID
/*! Get the main window to create a new widget where a new 2D plane can be displayed.
  @return The ID of the newly created widget or -1 if the creation failed.
  */
int rtMainWindow::createNew2DWidget() {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::createNew2DWidget() start") );
#endif
  int currID = -1;
  rtOptions2DView* view=NULL;

  for (int ix1=0; ix1<m_max2DWidgets; ix1++) {
    if (!m_view2DHash.contains(ix1)) {
      currID = ix1;
      view = new rtOptions2DView(scrollAreaWidget);
      view->setContainer(scrollAreaWidget);
      m_scrollArea2DImagesLayout.addWidget(view);
      m_view2DHash.insert(currID, view);
      view->setStringList(rtApplication::instance().getObjectManager()->get2DObjectNameHash());
      break;
    }
  }

#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::createNew2DWidget() end") );
#endif
  return currID;
}

//! Remove the widget with a particular ID.
/*!
  @param id The ID of the widget to be removed.
  @return True if the widget was found and removed. False otherwise.
  */
bool rtMainWindow::remove2DWidget(int id) {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::remove2DWidget() start") );
#endif

  if (!m_view2DHash.contains(id)) {
    rtApplication::instance().getMessageHandle()->warning( __LINE__, __FILE__, QString("rtMainWindow::remove2DWidget() No 2D widget with id: ").append(QString::number(id)) );
    return false;
  }

  rtOptions2DView* view;
  view = m_view2DHash.take(id);
  m_scrollArea2DImagesLayout.removeWidget(view);
  delete view;
  return true;
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::remove2DWidget() end") );
#endif
}

//! Get the handle to the widget with a particular ID.
/*! Attempt to get the handle to the 2D widget with a particular ID. If the widget does not exist then NULL will be returned.
  @param id The ID of the desired widget
  @return The handle to the desired widget or NULL if no widget with that ID exists.
  */
rtOptions2DView* rtMainWindow::get2DWidget(int id) {
  if (!m_view2DHash.contains(id)) return NULL;
  return m_view2DHash[id];
}

//! A function that will do the cleanup for all the 2D widgets.
void rtMainWindow::view2DHashCleanup() {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::view2DHashCleanup() start") );
#endif
  QList<int> keyList;

  keyList = m_view2DHash.keys();
  while (!keyList.empty()) {
    remove2DWidget(keyList.takeFirst());
  }
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::view2DHashCleanup() end") );
#endif
}

//! Called when the add button is pressed
void rtMainWindow::add2DFrame() {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::add2DFrame() start") );
#endif

  int res;
  res = createNew2DWidget();

  if (res == -1) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("rtMainWindow::add2DFrame() Could not create new 2D widget.") );
  }

#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::add2DFrame() end") );
#endif
}

//! Called when the remove button is pressed.
/*!
  This function will try to remove all of the selected widgets.
  */
void rtMainWindow::remove2DFrame() {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::remove2DFrame() start") );
#endif
  QList<int> keyList;

  keyList = m_view2DHash.keys();

  for (int ix1=0; ix1<keyList.count(); ix1++) {
    if(get2DWidget(keyList.at(ix1)) && get2DWidget(keyList.at(ix1))->isSelected()) {
      remove2DWidget(keyList.at(ix1));
    }
  }
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::remove2DFrame() end") );
#endif
}

//! Called when the remove all button is pressed.
void rtMainWindow::removeAll2DFrame() {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::removeAll2DFrame() start") );
#endif
  QList<int> keyList;

  keyList = m_view2DHash.keys();

  for (int ix1=0; ix1<keyList.count(); ix1++) {
    remove2DWidget(keyList.at(ix1));
  }
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::removeAll2DFrame() end") );
#endif
}
