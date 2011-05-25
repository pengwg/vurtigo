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

#include <vtkRenderWindow.h>
#include <vtkCamera.h>
#include <vtkCaptionActor2D.h>
#include <vtkPropAssembly.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkAxesActor.h>
#include <vtkProp.h>

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
#include "rtRegistration.h"
#include "pointPlacementDialog.h"
#include "rtOrientationMarkerWidget.h"


rtMainWindow::rtMainWindow(QWidget *parent, Qt::WindowFlags flags) {

#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::rtMainWindow() start") );
#endif

  setupUi(this);

  m_helpManager = NULL;
  m_currentObjectWidget = NULL;
  m_currentPluginWidget = NULL;
  m_renderFlag3D = false;
  m_renderLock.release(); // Must start with one resource.

  m_render3DLayout = new QHBoxLayout();
  m_render3DLayout->setContentsMargins(0,0,0,0);

  addNewRenderWindow();
  m_numRenWin = 1;

  m_visTable = new QTableWidget;

  m_movingQuality = 0.5f;
  m_stillQuality = 1.0f;

  //initialize dialogs
  m_registrationDialog = NULL;
  m_pointPlacementDialog = NULL;

  m_axesProperties = new rtAxesProperties();

  if (m_axesProperties) {
    setViewType( m_axesProperties->getViewType() );
    setCoordType( m_axesProperties->getCoordType() );
  }


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

  int ix1;
  for (ix1=0; ix1<m_cameraControl.size(); ix1++)
  {
      if (m_cameraControl[ix1]) delete m_cameraControl[ix1];
      //if (m_localCameraControl) delete m_localCameraControl;
      if (m_render3DVTKWidget[ix1]) delete m_render3DVTKWidget[ix1];
      if (m_renderer3D[ix1]) m_renderer3D[ix1]->Delete();
      if (m_axesActor[ix1]) m_axesActor[ix1]->Delete();
      if (m_propAssembly[ix1]) m_propAssembly[ix1]->Delete();
      if (m_orientationWidget[ix1]) {
        m_orientationWidget[ix1]->SetInteractor(NULL);
        m_orientationWidget[ix1]->Delete();
        m_orientationWidget[ix1] = NULL;
      }

  }

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


  if (m_render3DLayout) delete m_render3DLayout;
  if (m_objectBrowseLayout) delete m_objectBrowseLayout;


  //if (m_localRenderer3D) m_localRenderer3D->Delete();

  if (m_axesProperties) delete m_axesProperties;
  if (m_helpManager) delete m_helpManager;
  if (m_registrationDialog) delete m_registrationDialog;
  if (m_pointPlacementDialog) delete m_pointPlacementDialog;
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

vtkRenderWindow* rtMainWindow::getRenderWindow(int window) {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::getRenderWindow(): ").append( QString::number((long)m_renWin3D, 16) ) );
#endif
  return m_renWin3D[window];
}

//! Get the vtkRenderer object for the 3D view. 
vtkRenderer* rtMainWindow::getRenderer(int window) {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::getRenderer(): ").append( QString::number((long)m_renderer3D, 16) ) );
#endif
  return m_renderer3D[window];
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
  QHash<QString, QTreeWidgetItem *>::iterator wIter;
  bool objExists;

  if (!hash) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("In updateObjectList() hash pointer is NULL!") );
    return;
  }

  // Update all the items.
  for (i = hash->begin(); i != hash->end(); ++i) {
    i.value()->updateTreeItem();
  }


  // Setup the items
  for (wIter = m_topItems.begin(); wIter!=m_topItems.end(); ++wIter){
      if (objectTree->findItems(wIter.value()->text(0),Qt::MatchExactly).isEmpty())
          objectTree->addTopLevelItem(wIter.value());
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
      warn.append( i.value()->getObjectType() );
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

  if (m_visTable->isVisible()) showObjectVisibilities();

#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::currItemChanged() end") );
#endif
}

//! Called when the content of a tree item has changed.
/*!
  The main element that changes is the state of the check box. This function will add or remove elements form the renderer based on the state of the check box.
  */
void rtMainWindow::itemChanged(QTreeWidgetItem * current, int column, bool global) {
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

  // Check that the changed element was indeed the checkbox.
  if (column != 2) return;

  // Get the object
  temp = rtApplication::instance().getObjectManager()->getObjectWithID(current->text(1).toInt());

  // If the box is checked then add it to the renderer.
  if (current->checkState(column) == Qt::Checked) {
    for (int ix1=0; ix1<temp->getVisible3D().size(); ix1++)
      {
        if (global || temp->getVisible3D().at(ix1))
        {
            if ( temp->addToRenderer(m_renderer3D[ix1],ix1) ) {
                m_renderFlag3D = true;
            }
        }

    }
  } else {
      for (int ix1=0; ix1<temp->getVisible3D().size(); ix1++)
      {
          if ( temp->removeFromRenderer(m_renderer3D[ix1],ix1) ) {
              m_renderFlag3D = true;
          }

      }
      temp->deselect();
  }

   if (m_visTable->isVisible()) showObjectVisibilities();

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
  if (!temp->getVisible3D().contains(true)) {
    // Not visible so we have nothing to center on...
    return;
  }
  double loc[6];
  if (temp->getObjectLocation(loc)) {
    // Location is valid
      if (m_renderer3D.size() != temp->getVisible3D().size())
          std::cout << "rtMainWindow::centerOnObject number of visible3D flags in the current object doesn't match the numRenWin \n";
    for (int ix1=0; ix1<m_renderer3D.size(); ix1++)
    {
        if (temp->getVisible3D().at(ix1))
            m_renderer3D[ix1]->ResetCamera(loc);
    }
    //m_localRenderer3D->ResetCamera(loc);
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
void rtMainWindow::addRenderItem(vtkProp* prop, int window) {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::addRenderItem() start") );
#endif
  if (!prop) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("addRenderItem() Prop to add is NULL. Not Added."));
    return;
  }

  //if ( !m_renderer3D->HasViewProp(prop) || !m_localRenderer3D->HasViewProp(prop) ) {
  if ( !m_renderer3D[window]->HasViewProp(prop) ) {
    m_renderer3D[window]->AddViewProp(prop);
    //m_localRenderer3D->AddViewProp(prop);
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
void rtMainWindow::removeRenderItem(vtkProp* prop, int window) {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::removeRenderItem() start") );
#endif

  if (!prop) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("removeRenderItem() Prop to remove is NULL. Not Added."));
    return;
  }

  //if (m_renderer3D->HasViewProp(prop) || m_localRenderer3D->HasViewProp(prop)) {
  if (m_renderer3D[window]->HasViewProp(prop) ) {
    m_renderer3D[window]->RemoveViewProp(prop);
   // m_localRenderer3D->RemoveViewProp(prop);
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
    for (int ix1=0; ix1<m_renWin3D.size(); ix1++)
        m_renWin3D[ix1]->Render();
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

bool rtMainWindow::cameraMoving(int window) {
  if (m_cameraControl.empty()) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("rtMainWindow::cameraMoving() cameraControl is NULL: "));
    return false;
  }
  return m_cameraControl[window]->cameraMoving();
}

double rtMainWindow::getCameraDistance(int window) {
  return m_renderer3D[window]->GetActiveCamera()->GetDistance();
}

void rtMainWindow::getCameraUp(double val[3],int window) {
  if (m_cameraControl.empty()) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("rtMainWindow::getCameraUp() cameraControl is NULL: "));
    return;
  }
  m_cameraControl[window]->getUpDirection(val);
}

void rtMainWindow::getCameraRight(double val[3], int window) {
  if (m_cameraControl.empty()) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("rtMainWindow::getCameraRight() cameraControl is NULL: "));
    return;
  }
  m_cameraControl[window]->getRightDirection(val);
}

void rtMainWindow::getCameraForward(double val[3], int window) {
  if (m_cameraControl.empty()) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("rtMainWindow::getCameraForward() cameraControl is NULL: "));
    return;
  }
  m_cameraControl[window]->getForwardDirection(val);
}

vtkProp* rtMainWindow::getSelectedProp(int window) {
  if (m_render3DVTKWidget.isEmpty()) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("rtMainWindow::getSelectedProp() 3D render widget pointer is NULL: "));
    return NULL;
  }

  if (m_render3DVTKWidget[window]->getChosenProp())
      return m_render3DVTKWidget[window]->getChosenProp();


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
  connect (actionRename_Object, SIGNAL(triggered()), this, SLOT(renameObject()));
  connect( actionDeleteSelected, SIGNAL(triggered()), this, SLOT(removeSelectedObject()) );
  connect( actionRegistration, SIGNAL(triggered()), this, SLOT(registerDialog()));
  connect (actionPoint_Placement, SIGNAL(triggered()), this,SLOT(showPointPlacement()));

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
  connect(actionObject_Visibilities, SIGNAL(triggered()),this,SLOT(showObjectVisibilities()));
  connect(m_visTable, SIGNAL(cellChanged(int,int)),this,SLOT(visTableChanged(int,int)));

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
  connect(actionAddRenWin, SIGNAL(triggered()), this, SLOT(addRenWinPressed()));
  connect(actionRemoveRenWin, SIGNAL(triggered()),this,SLOT(remRenWinPressed()));

  //whatsThis
  connect(actionWhatsThis,SIGNAL(triggered()),this,SLOT(activateWhatsThis()));

  //Context Menu
  connect(objectTree, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(objectTreeContextMenu(QPoint)));

  //controls
  connect(actionControls,SIGNAL(triggered()),this,SLOT(showControls()));


#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::connectSignals() end") );
#endif
}

void rtMainWindow::setupObjectTree() {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::setupObjectTree() start") );
#endif

  QHash<QString, QString>::iterator i;
  QTreeWidgetItem * temp=NULL;

  m_topItems.clear();

  for (i = m_rtObjectTypeNames.begin(); i !=  m_rtObjectTypeNames.end(); ++i) {
    temp = new QTreeWidgetItem();
    temp->setText(0, i.value());
    m_topItems.insert(i.key(), temp);
  }
  objectTree->setColumnWidth(0, 150);
  objectTree->setColumnWidth(1, 60);

  objectTree->setContextMenuPolicy(Qt::CustomContextMenu);

#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::setupObjectTree() end") );
#endif
}

void rtMainWindow::objectTreeContextMenu(QPoint pos)
{
    if (objectTree->currentItem()->parent())
    {
        QMenu menu;
        menu.addAction(actionSave_Object);
        menu.addAction(actionRename_Object);
        if (!rtApplication::instance().getObjectManager()->isPermanent(objectTree->currentItem()->text(1).toInt()))
            menu.addAction(actionDeleteSelected);
        menu.addSeparator();
        menu.addAction(actionObject_Visibilities);
        menu.exec(QCursor::pos());
    }
}

//! Assign string names to the object types.
/*!
  With string names the objects can be displayed in the GUI.
*/
void rtMainWindow::populateObjectTypeNames() {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::populateObjectTypeNames() start") );
#endif
  m_rtObjectTypeNames.insert("OT_3DObject", "3D Object");
  m_rtObjectTypeNames.insert("OT_2DObject", "2D Object");
  m_rtObjectTypeNames.insert("OT_Cath", "Catheter");
  m_rtObjectTypeNames.insert("OT_vtkMatrix4x4", "4 by 4 vtkMatrix");
  m_rtObjectTypeNames.insert("OT_vtkPolyData", "Polygon Data");
  m_rtObjectTypeNames.insert("OT_vtkPiecewiseFunction", "Piecewise Function");
  m_rtObjectTypeNames.insert("OT_vtkColorTransferFunction", "Color Function");
  m_rtObjectTypeNames.insert("OT_ImageBuffer", "Image Buffer");
  m_rtObjectTypeNames.insert("OT_2DPointBuffer", "2D Point Buffer");
  m_rtObjectTypeNames.insert("OT_3DPointBuffer", "3D Point Buffer");
  m_rtObjectTypeNames.insert("OT_TextLabel", "Text Label");
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

void rtMainWindow::addRenWinPressed()
{
    if (m_numRenWin < m_render3DVTKWidget.size())
        m_render3DVTKWidget[m_numRenWin]->show();
    else
        addNewRenderWindow();

    m_numRenWin++;
    emit newRenderSignal();
    refreshRenderItems(true);
    if (m_visTable->isVisible()) showObjectVisibilities();
    if (m_axesProperties) {
        setViewType( m_axesProperties->getViewType() );
        setCoordType( m_axesProperties->getCoordType() );
    }

    m_renderFlag3D = true;
}

void rtMainWindow::remRenWinPressed()
{
    if (m_numRenWin > 1)
    {
        m_render3DVTKWidget[m_numRenWin-1]->hide();
        m_numRenWin--;
        emit removeRenderSignal();
        refreshRenderItems(false);
        if (m_visTable->isVisible()) showObjectVisibilities();
    }
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

void rtMainWindow::setCoordManual(int ct) {

    m_axesProperties->setCoordType(ct);
    setCoordType( m_axesProperties->getCoordType() );
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

void rtMainWindow::registerDialog() {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::registerDialog() start") );
#endif
  if (!m_registrationDialog) m_registrationDialog = new rtRegistration(this);
  m_registrationDialog->placementOn();
  m_registrationDialog->setupAllCombos();
  m_registrationDialog->show();


#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::registerDialog() end") );
#endif
}

void rtMainWindow::showPointPlacement() {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::showPointPlacement() start") );
#endif
  // create point placement dialog
  if (!m_pointPlacementDialog) m_pointPlacementDialog = new pointPlacementDialog(this);
   m_pointPlacementDialog->placementOn();
   m_pointPlacementDialog->setupCombo();
   m_pointPlacementDialog->show();

#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::showPointPlacement() end") );
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
  for (int ix1=0; ix1<m_renWin3D.size(); ix1++)
  {
      if (m_renWin3D[ix1]->GetStereoRender()) {
          renOpt.setStereoType(m_renWin3D[ix1]->GetStereoType());
      } else {
          renOpt.setStereoType(0);
      }
  }
#endif

  for (int ix1=0; ix1<m_renWin3D.size(); ix1++)
  {
      renOpt.setDirectRender(m_renWin3D[ix1]->IsDirect());
      renOpt.setGLRender(m_renWin3D[ix1]->SupportsOpenGL());
  }

  if(renOpt.exec() == QDialog::Accepted) {  
    double renUpQual = renOpt.getRenUpdateQuality();
    double renQual = renOpt.getRenQuality();
    m_stillQuality = renQual/100.0f;
    m_movingQuality = renUpQual/100.0f;

    // Stereo is broken on OSX
#ifdef Q_WS_MAC

#else
    int type = renOpt.getStereoType();
    for (int ix1=0; ix1<m_renWin3D.size(); ix1++)
    {
        if (type == 0) {
            m_renWin3D[ix1]->StereoRenderOff();
        } else {
            m_renWin3D[ix1]->StereoRenderOn();
            m_renWin3D[ix1]->SetStereoType(type);
        }
    }
#endif

    m_renderFlag3D = true;
  }

#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::showRenderOptions() end") );
#endif
}

void rtMainWindow::cameraDefaultView() {
    for (int ix1=0; ix1<m_cameraControl.size(); ix1++)
    {
        m_cameraControl[ix1]->setToDefaultPosition();
    }
  m_renderFlag3D = true;
}


void rtMainWindow::cameraRobotArmView() {
    for (int ix1=0; ix1<m_cameraControl.size(); ix1++)
    {
        m_cameraControl[ix1]->setToRobotArmPosition();
    }
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
  fName = QFileDialog::getOpenFileName(this, "Select Plugin XML File", m_lastPluginDir.path(), "*.XML *.xml");

  QFile file(fName);
  if (!file.exists()) {
    rtApplication::instance().getMessageHandle()->warning(__LINE__, __FILE__, QString("rtMainWindow::loadPluginFile() File does not exist: ").append(fName) );
    return;
  }
  m_lastPluginDir = QFileInfo(fName).dir();
  m_lastPluginDir.cdUp();

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
      rtApplication::instance().getObjectManager()->addObjectOfType("OT_vtkPiecewiseFunction", name);
    } else if (setupDlg.objTypeCombo->currentText() == "Color Transfer Function") {
      rtApplication::instance().getObjectManager()->addObjectOfType("OT_vtkColorTransferFunction", name);
    }
  }

#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::addNewObject() end") );
#endif
}

QList<int> rtMainWindow::loadObject(QString fname) {
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::loadObject() start") );
#endif

  QFile file;
  QStringList names;
  QList<int> objIDs;
  if (fname == "")
      names = QFileDialog::getOpenFileNames(this, "Select Files To Open:");
  else
      names.append(fname);
  for (int ix1=0; ix1<names.size(); ix1++) {
    file.setFileName(names.at(ix1));
    if (file.exists() && file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      QXmlStreamReader reader(&file);
      QXmlStreamReader::TokenType type;
      bool typeRead=false;
      bool nameRead=false;
      QString objType;
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

      if (rtApplication::instance().getObjectManager()->hasType(objType))
          typeRead = true;
      else
          QMessageBox::critical(this,"Object Type Not Found", "Could not find the object type in Vurtigo.\n If the object type is from a Plugin, please load the plugin first and retry.");
      if (objName != "")
          nameRead = true;
      else
          QMessageBox::critical(this,"Unnamed Object", "Object has no name, aborting load.");

      rtRenderObject* obj;
      QFile objFile;
      // Both the type and the name have been recognized.
      if (typeRead && nameRead) {
        obj = rtApplication::instance().getObjectManager()->addObjectOfType(objType, objName);
        if (obj) {
          objFile.setFileName(names.at(ix1));
          QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
          if (!obj->loadFile(&objFile))
              rtApplication::instance().getObjectManager()->removeObject(obj->getDataObject()->getId());
          else
              objIDs.append(obj->getDataObject()->getId());
          QApplication::restoreOverrideCursor();
        }
      }

      file.close();
    }
  }
  return objIDs;
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
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    temp->saveFile(&file);
    QApplication::restoreOverrideCursor();
  }
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::saveObject() end") );
#endif
}

//! Rename the currrently selected item
void rtMainWindow::renameObject()
{
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::renameObject() start") );
#endif

  QTreeWidgetItem* current=NULL;

  current = objectTree->currentItem();

  if (!current) return;

  // Check for a heading.
  if (current->columnCount() == 1) {
    return;
  }

  bool ok;
  QString oldName = rtApplication::instance().getObjectManager()->getObjectWithID(current->text(1).toInt())->getDataObject()->getObjName();
  QString newName = QInputDialog::getText(this,"Rename Object","New name of object: ",QLineEdit::Normal,oldName,&ok);
  if (ok && !newName.isEmpty())
  {
      rtApplication::instance().getObjectManager()->renameObjectWithID(current->text(1).toInt(),newName);
  }

  m_renderFlag3D = true;
#ifdef DEBUG_VERBOSE_MODE_ON
  rtApplication::instance().getMessageHandle()->debug( QString("rtMainWindow::renameObject() end") );
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
  for (int ix1=0; ix1<m_renderer3D.size(); ix1++)
  {
      temp->removeFromRenderer(m_renderer3D[ix1],ix1);
     // temp->removeFromRenderer(m_localRenderer3D);
  }
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
    for (int ix1=0; ix1<m_orientationWidget.size(); ix1++)
      m_orientationWidget[ix1]->SetEnabled(0);
    break;
    case(rtAxesProperties::VT_VISIBLE):
    for (int ix1=0; ix1<m_orientationWidget.size(); ix1++)
    {
        m_orientationWidget[ix1]->SetEnabled(1);
        m_orientationWidget[ix1]->InteractiveOff();
    }
    break;
    case(rtAxesProperties::VT_INTERACT):
    for (int ix1=0; ix1<m_orientationWidget.size(); ix1++)
    {
        m_orientationWidget[ix1]->SetEnabled(1);
        m_orientationWidget[ix1]->InteractiveOn();
    }
    break;
  }
  m_renderFlag3D = true;
}

void rtMainWindow::setCoordType(rtAxesProperties::CoordType ct) {
  for (int ix1=0; ix1<m_axesActor.size(); ix1++)
  {
  switch (ct) {
    case (rtAxesProperties::CT_HFS):
    m_axesActor[ix1]->SetXAxisLabelText("Left");
    m_axesActor[ix1]->SetYAxisLabelText("Posterior");
    m_axesActor[ix1]->SetZAxisLabelText("Superior");
    break;
    case (rtAxesProperties::CT_FFS):
    m_axesActor[ix1]->SetXAxisLabelText("Right");
    m_axesActor[ix1]->SetYAxisLabelText("Posterior");
    m_axesActor[ix1]->SetZAxisLabelText("Inferior");
    break;
    case (rtAxesProperties::CT_HFP):
    m_axesActor[ix1]->SetXAxisLabelText("Right");
    m_axesActor[ix1]->SetYAxisLabelText("Anterior");
    m_axesActor[ix1]->SetZAxisLabelText("Superior");
    break;
    case (rtAxesProperties::CT_FFP):
    m_axesActor[ix1]->SetXAxisLabelText("Left");
    m_axesActor[ix1]->SetYAxisLabelText("Anterior");
    m_axesActor[ix1]->SetZAxisLabelText("Inferior");
    break;
    case (rtAxesProperties::CT_HFDR):
    m_axesActor[ix1]->SetXAxisLabelText("Posterior");
    m_axesActor[ix1]->SetYAxisLabelText("Right");
    m_axesActor[ix1]->SetZAxisLabelText("Superior");
    break;
    case (rtAxesProperties::CT_FFDR):
    m_axesActor[ix1]->SetXAxisLabelText("Anterior");
    m_axesActor[ix1]->SetYAxisLabelText("Right");
    m_axesActor[ix1]->SetZAxisLabelText("Inferior");
    break;
    case (rtAxesProperties::CT_HFDL):
    m_axesActor[ix1]->SetXAxisLabelText("Anterior");
    m_axesActor[ix1]->SetYAxisLabelText("Left");
    m_axesActor[ix1]->SetZAxisLabelText("Superior");
    break;
    case (rtAxesProperties::CT_FFDL):
    m_axesActor[ix1]->SetXAxisLabelText("Posterior");
    m_axesActor[ix1]->SetYAxisLabelText("Left");
    m_axesActor[ix1]->SetZAxisLabelText("Inferior");
    break;
    case (rtAxesProperties::CT_VTK):
    m_axesActor[ix1]->SetXAxisLabelText("X");
    m_axesActor[ix1]->SetYAxisLabelText("Y");
    m_axesActor[ix1]->SetZAxisLabelText("Z");
    break;
  }
  
 // enlarge axis label text to make it readable
  m_axesActor[ix1]->GetXAxisCaptionActor2D()->GetTextActor()->SetTextScaleMode(vtkTextActor::TEXT_SCALE_MODE_NONE);
  m_axesActor[ix1]->GetXAxisCaptionActor2D()->GetTextActor()->GetTextProperty()->SetFontSize(12);
  m_axesActor[ix1]->GetXAxisCaptionActor2D()->GetTextActor()->GetTextProperty()->SetBold(false);

  m_axesActor[ix1]->GetYAxisCaptionActor2D()->GetTextActor()->SetTextScaleMode(vtkTextActor::TEXT_SCALE_MODE_NONE);
  m_axesActor[ix1]->GetYAxisCaptionActor2D()->GetTextActor()->GetTextProperty()->SetFontSize(12);
  m_axesActor[ix1]->GetYAxisCaptionActor2D()->GetTextActor()->GetTextProperty()->SetBold(false);

  m_axesActor[ix1]->GetZAxisCaptionActor2D()->GetTextActor()->SetTextScaleMode(vtkTextActor::TEXT_SCALE_MODE_NONE);
  m_axesActor[ix1]->GetZAxisCaptionActor2D()->GetTextActor()->GetTextProperty()->SetFontSize(12);
  m_axesActor[ix1]->GetZAxisCaptionActor2D()->GetTextActor()->GetTextProperty()->SetBold(false);
  
 // EthanB: It's a shame they only allow arrows/labels on +X/+Y/+Z, as I'd really rather have a visual depiction of "Right", "Anterior", and "Superior"
}
  
  m_renderFlag3D = true;
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

void rtMainWindow::addNewRenderWindow()
{
    customQVTKWidget *newWidget =  new customQVTKWidget(this->frame3DRender);

    newWidget->setMinimumSize(300,300);

    m_render3DVTKWidget.append(newWidget);

    vtkRenderWindow *newWindow = newWidget->GetRenderWindow();

    // Stereo rendering causes bugs in OSX.
  #ifdef Q_WS_MAC

  #else
    newWindow->StereoCapableWindowOn();
  #endif

    m_renWin3D.append( newWindow );


    // Create the renderer
    vtkRenderer *newRenderer = vtkRenderer::New();
    m_renderer3D.append( newRenderer );
    newWindow->AddRenderer(newRenderer);
    newRenderer->SetViewport(0.0, 0.0, 1.0, 1.0);

    rtCameraControl *newCameraControl = new rtCameraControl( newRenderer->GetActiveCamera(), newRenderer, newWidget );
    m_cameraControl.append(newCameraControl);
    newCameraControl->setToDefaultPosition();


    vtkAxesActor *newAxes = vtkAxesActor::New();
    m_axesActor.append(newAxes);
    vtkPropAssembly *newProps = vtkPropAssembly::New();
    m_propAssembly.append(newProps );
    rtOrientationMarkerWidget *newOriWidget = new rtOrientationMarkerWidget(newWidget);
    m_orientationWidget.append(newOriWidget);

    newProps->AddPart( m_axesActor.last() );
    newOriWidget->SetOrientationMarker(newProps);

    // This is a bit of a hack.
    // Vurtigo does not use the vtk style interactor to get mouse events.
    // However, the vtk Widget that is used for orientation does need it otherwise a seg-fault occurs.
    // Note that the interactor is disabled!
    vtkRenderWindowInteractor* inter3D = newWidget->GetInteractor();
    inter3D->Disable();
    newOriWidget->SetInteractor( inter3D );
    // End hack

    newOriWidget->SetViewport( 0.75, 0.0, 1.0, 0.25 );

    m_render3DLayout->addWidget(newWidget);
}

void rtMainWindow::refreshRenderItems(bool flag)
{
    rtRenderObject *temp=NULL;
    QList<bool> visibles;
    for (int ix1=0; ix1<objectTree->topLevelItemCount(); ix1++)
    {
        for (int ix2=0; ix2<objectTree->topLevelItem(ix1)->childCount(); ix2++)
        {
            temp = rtApplication::instance().getObjectManager()->getObjectWithID(objectTree->topLevelItem(ix1)->child(ix2)->text(1).toInt());
            visibles = temp->getVisible3D();
            if (flag)
                visibles.append(true);
            else
                visibles.takeLast();
            temp->setVisible3DAll(visibles);
            itemChanged(objectTree->topLevelItem(ix1)->child(ix2),2,false);
        }
      }
      m_renderFlag3D = true;
}

void rtMainWindow::showObjectVisibilities()
{
    QTreeWidgetItem* current=NULL;
    rtRenderObject* temp=NULL;

    current = objectTree->currentItem();

    if (!current) return;

    // Check for a heading.
    if (current->columnCount() == 1) {
      return;
    }

    // Find the object
    temp = rtApplication::instance().getObjectManager()->getObjectWithID(current->text(1).toInt());
    //rows,cols
    m_visTable->clear();
    m_visTable->clearContents();
    // get the visibilities
    QList<bool> objVis = temp->getVisible3D();

    // setup the table
    m_visTable->setRowCount(1);
    m_visTable->setColumnCount(objVis.size());
    QTableWidgetItem *tempItem;
    for (int ix1=0; ix1<objVis.size(); ix1++)
    {
        tempItem = new QTableWidgetItem();
        tempItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        if (objVis[ix1])
        {
            tempItem->setCheckState(Qt::Checked);
            tempItem->setText("ON");

        }
        else if (!objVis[ix1])
        {
            tempItem->setCheckState(Qt::Unchecked);
            tempItem->setText("OFF");
        }

        m_visTable->setItem(0,ix1,tempItem);
    }
    m_visTable->setMaximumHeight(80);
    m_visTable->setWindowTitle(temp->getDataObject()->getObjName() + " - Visibilities");
    QStringList vhead;
    vhead.append("Visible?");
    m_visTable->setVerticalHeaderLabels(vhead);
    m_visTable->show();
}

void rtMainWindow::visTableChanged(int row,int col)
{
    if (row == 0)
    {
        rtRenderObject* temp = rtApplication::instance().getObjectManager()->getObjectWithID(objectTree->currentItem()->text(1).toInt());
        QTableWidgetItem *item = m_visTable->item(row,col);
        if (item->checkState()==Qt::Checked)
        {
            item->setText("ON");
            temp->setVisible3D(col,true);
            // if we turn one on, set global check on as well
            setGlobalOn(objectTree->currentItem());
            if (temp->addToRenderer(m_renderer3D[col],col))
                m_renderFlag3D = true;

        }
        else if (item->checkState()==Qt::Unchecked)
        {
            item->setText("OFF");
            temp->setVisible3D(col,false);
            temp->deselect();
            // if we just unchecked them all
            if (!temp->getVisible3D().contains(true))
                objectTree->currentItem()->setCheckState(2,Qt::Unchecked);
            if (temp->removeFromRenderer(m_renderer3D[col],col))
                m_renderFlag3D = true;
        }
    }
}

void rtMainWindow::showControls()
{
    QString text;
    text = " 3D View Controls: \n";
    text +=" --------------------- \n\n\n";
    text +=" Shift - Show Cursor Coordinates \n\n";
    text +=" Left-Click + Drag - Rotate Camera \n\n";
    text +=" Right-Click + Drag - Translate Camera \n\n";
    text +=" PageUp / PageDown - Roll Camera \n\n";
    text +=" Scroll Wheel - Move Camera Forwards/Backwards \n\n";
    text +=" Double-Click On Object- Select Object \n\n";
    text +=" Double-Click Not On Object - Deselect Object \n\n";
    text +=" D-Key - Deselect All Objects \n\n\n";
    text +=" Object Interaction: \n";
    text +=" ----------------------- \n\n\n";
    text +=" Left-Click+Drag on Ring - Rotate Object (along ring direction) \n\n";
    text +=" Left-Click+Drag on Centre Sphere - Translate Object \n\n";
    text +=" Scroll Wheel - Translate Object Towards/Away from Camera";

    QMessageBox::information(0,"3D View Controls",text);

}

void rtMainWindow::setGlobalOn(QTreeWidgetItem *item)
{
    // disconnect the signal because we just want to toggle the checkbox
    disconnect(objectTree, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(itemChanged(QTreeWidgetItem*,int)));
    item->setCheckState(2,Qt::Checked);
    connect(objectTree, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(itemChanged(QTreeWidgetItem*,int)));

}

void rtMainWindow::addNewObjectType(QString type)
{
    m_rtObjectTypeNames.insert(type,type);
    QTreeWidgetItem *temp = new QTreeWidgetItem();
    temp->setText(0, type);
    m_topItems.insert(type, temp);
}
