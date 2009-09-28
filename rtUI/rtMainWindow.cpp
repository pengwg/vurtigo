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

#include <iostream>
#include <cmath>

#include "rtMainWindow.h"
#include "rtObjectManager.h"
#include "rtRenderObject.h"
#include "rtDataObject.h"
#include "rtPluginLoader.h"
#include "rtMessage.h"
#include "rtRenderOptions.h"
#include "ui_newObjectDialog.h"

#include "vtkPropAssembly.h"
#include "vtkCamera.h"

#include "version.h"

rtMainWindow::rtMainWindow(QWidget *parent, Qt::WindowFlags flags) {
  setupUi(this);

  m_cellPicker = vtkCellPicker::New();

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
  m_renWin3D->StereoCapableWindowOn();
  m_renderer3D = vtkRenderer::New();
  m_renWin3D->AddRenderer(m_renderer3D);

  m_inter3D->SetDesiredUpdateRate(3.0f);
  m_inter3D->SetStillUpdateRate(1.0f);

  m_axesActor = vtkAxesActor::New();
  m_propAssembly = vtkPropAssembly::New();
  m_orientationWidget = vtkOrientationMarkerWidget::New();

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

  m_view2DHash.clear();
  m_max2DWidgets = 100;

  scrollAreaWidget->setLayout(&m_scrollArea2DImagesLayout);

  pluginBrowseFrame->setLayout(&m_pluginWidgetLayout);

  viewChangedMixed();
  populateObjectTypeNames();
  connectSignals();
  setupObjectTree();
}

//! Destructor
/*!
  Removes the current widget from the viewer at the bottom.
  Also deletes objects that were created in this class. 
 */
rtMainWindow::~rtMainWindow() {
  m_renderFlag3D = false;

  m_cellPicker->Delete();

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
  if (m_orientationWidget) m_orientationWidget->Delete();
  if (m_axesProperties) delete m_axesProperties;
}

vtkRenderWindow* rtMainWindow::getRenderWindow() {
  return m_renWin3D;
}

vtkRenderWindowInteractor* rtMainWindow::getInteractor() {
  return m_inter3D;
}

//! Get the vtkRenderer object for the 3D view. 
vtkRenderer* rtMainWindow::getRenderer() {
  return m_renderer3D;
}

//! Get the object tree
QTreeWidget* rtMainWindow::getObjectTree() {
  return objectTree;
}

//! Called with the newest set of render objects. 
void rtMainWindow::updateObjectList(QHash<int, rtRenderObject*>* hash) {
  QHash<int, rtRenderObject*>::iterator i;
  QHash<rtConstants::rtObjectType, QTreeWidgetItem *>::iterator wIter;

  // Remove all the items first from the GUI.
  while (objectTree->takeTopLevelItem(0));

  for (wIter = m_topItems.begin(); wIter!=m_topItems.end(); ++wIter){
    objectTree->addTopLevelItem(wIter.value());
  }

  for (i = hash->begin(); i != hash->end(); ++i) {
    i.value()->updateTreeItem();
    if ( m_topItems.contains(i.value()->getObjectType()) ) {
      m_topItems.value(i.value()->getObjectType())->addChild(i.value()->getTreeItem());
    } else {
      qWarning("Warning: Could not find category: %d for object: %#x", i.value()->getObjectType(), i.value());
    }
  }
}

//! Empty the object list from the GUI
void rtMainWindow::clearObjectList() {
  while (objectTree->takeTopLevelItem(0));
}

//! Update the GUI with a new set of pligins.
void rtMainWindow::updatePluginList(QHash<int, QTreeWidgetItem*>* hash) {
  QHash<int, QTreeWidgetItem*>::iterator i;

  while (pluginTree->takeTopLevelItem(0));

  for (i = hash->begin(); i != hash->end(); ++i) {
    pluginTree->addTopLevelItem(i.value());
  }
}

//! Empty the plugin list from the GUI.
void rtMainWindow::clearPluginList() {
  while (pluginTree->takeTopLevelItem(0));
}

//! This slot is called every time a new object is selected in the tree. 
void rtMainWindow::currItemChanged(QTreeWidgetItem * current, QTreeWidgetItem * previous) {
  rtRenderObject *temp;
  QWidget * baseWid;

  if (!current) return;
  
  // Check for a heading. 
  if (current->columnCount() == 1) {
    if (m_currentObjectWidget) {
      m_objectBrowseLayout->removeWidget(m_currentObjectWidget);
      m_currentObjectWidget->setParent(NULL);
      m_currentObjectWidget = NULL;
    }
    return;
  }

  if (m_currentObjectWidget) {
    m_objectBrowseLayout->removeWidget(m_currentObjectWidget);
    m_currentObjectWidget->setParent(NULL);
  }

  temp = rtObjectManager::instance().getObjectWithID(current->text(1).toInt());
  baseWid = temp->getDataObject()->getBaseWidget();

  //std::cout << "Base Widget: " << baseWid << std::endl;

  m_objectBrowseLayout->addWidget(baseWid);
  m_currentObjectWidget = baseWid;
}

//! Called when the content of a tree item has changed.
/*!
  The main element that changes is the state of the check box. This function will add or remove elements form the renderer based on the state of the check box.
  */
void rtMainWindow::itemChanged(QTreeWidgetItem * current, int column) {
  rtRenderObject *temp;

  if (!current) return;

  // Check for a heading. 
  if (current->columnCount() == 1) {
    return;
  }

  // Get the object
  temp = rtObjectManager::instance().getObjectWithID(current->text(1).toInt());

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
}

//! User has double clicked on an item so we need to re-center on it.
/*!
  @todo Actually re-center on the object.
  */
void rtMainWindow::centerOnObject(QTreeWidgetItem *item, int column) {
  rtRenderObject *temp;

  if (!item) return;

  // Check for a heading.
  if (item->columnCount() == 1) {
    return;
  }

  // Get the object
  temp = rtObjectManager::instance().getObjectWithID(item->text(1).toInt());
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

}

//! Render items can be added directly.
/*!
  Note that this function does not keep track of the objects it adds.
 */
void rtMainWindow::addRenderItem(vtkProp* prop) {
  if (!m_renderer3D->HasViewProp(prop)) {
    m_renderer3D->AddViewProp(prop);
    m_renderFlag3D = true;
  }
}

//! Render items can be removed directly. 
/*!
  Note that this function does not keep track of the objects it removes.
 */
void rtMainWindow::removeRenderItem(vtkProp* prop) {
  if (m_renderer3D->HasViewProp(prop)) {
    m_renderer3D->RemoveViewProp(prop);
    m_renderFlag3D = true;
  }
}



//! Called when the user clicks on a different plugin in the viewer.
/*!
  When a new plugin is selected in the list this function is called.
  @param current The pointer to the element selected or NULL is if none was selected.
  @param previous Pointer to the last selected element.
 */
void rtMainWindow::pluginItemChanged(QTreeWidgetItem * current, QTreeWidgetItem * previous) {
  int id;
  bool convOK;
  QWidget* plugWidget;

  // Empty the layout.
  if (m_currentPluginWidget) {
    m_pluginWidgetLayout.removeWidget(m_currentPluginWidget);
    m_currentPluginWidget->setParent(NULL);
    m_currentPluginWidget->hide();
    m_currentPluginWidget = NULL;
  }

  // Check if there is a new item to use.
  if (!current) return;

  // First column contains the unique ID
  id = current->text(0).toInt(&convOK, 10);

  if (!convOK) return;

  plugWidget = rtPluginLoader::instance().getPluginWithID(id)->getWidget();
  m_currentPluginWidget = plugWidget;
  if (plugWidget) {
    m_pluginWidgetLayout.addWidget(plugWidget);
    plugWidget->show();
  }
}

//! Try to render the 3D window.
/*!
  This function will only render if the render flag has been set to true. Once the function runs it will reset the flag back to false.
 */
void rtMainWindow::tryRender3D() {
  if (m_renderFlag3D && m_renderLock.tryAcquire()) {
    m_renWin3D->Render();
    m_renderFlag3D = false;
    m_renderLock.release();
  }
}

//! Called when the timer decides it is a good time to update the 2D views.
/*!
  Try to update each of the 2D views. 2D views that do not have valid 'current objects' will not be updated.
  */
void rtMainWindow::update2DViews() {
  QList<int> keyL;
  rtRenderObject* renObj;

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
}

//! Connect the menu actions to the functions that handle them.
void rtMainWindow::connectSignals() {
  // File Menu
  connect(actionPlugin_File, SIGNAL(triggered()), this, SLOT(loadPluginFile()));
  connect(actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));

  // Object Menu
  connect( actionNewObject, SIGNAL(triggered()), this, SLOT(addNewObject()) );
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

  // Help Menu
  connect(actionContents, SIGNAL(triggered()), this, SLOT(viewContents()));
  connect(actionAbout, SIGNAL(triggered()), this, SLOT(viewAbout()));

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

}

void rtMainWindow::setupObjectTree() {
  QHash<rtConstants::rtObjectType, QString>::iterator i;
  QTreeWidgetItem * temp;

  m_topItems.clear();

  for (i = m_rtObjectTypeNames.begin(); i !=  m_rtObjectTypeNames.end(); ++i) {
    temp = new QTreeWidgetItem();
    temp->setText(0, i.value());
    m_topItems.insert(i.key(), temp);
  }
  objectTree->setColumnWidth(0, 150);
  objectTree->setColumnWidth(1, 60);
}

//! Assign string names to the object types.
/*!
  With string names the objects can be displayed in the GUI.
*/
void rtMainWindow::populateObjectTypeNames() {
  m_rtObjectTypeNames.insert(rtConstants::OT_4DObject, "4D Object");
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
}

//! Change the view to mixed mode where both the 2D and 3D windows are visible.
void rtMainWindow::viewChangedMixed() {
  stackedWidget->setCurrentIndex(0);
  mainUDSplitter->insertWidget(0, frame3DRender);
  mainUDSplitter->insertWidget(1, lowerFrame);
}

//! Make only the 3D render window visible.
void rtMainWindow::viewChanged3DOnly() {
  stackedWidget->setCurrentIndex(1);
  m_only3DLayout.addWidget(frame3DRender);
}

//! Make only the 2D window visible
void rtMainWindow::viewChanged2DOnly() {
  stackedWidget->setCurrentIndex(2);
  m_only2DLayout.addWidget(lowerFrame);
}

//! Update  the lists for all the 2D windows.
void rtMainWindow::update2DWindowLists(QMultiHash<int, QString>* hash) {
  QList<int> keyL;
  keyL = m_view2DHash.keys();
  for (int ix1=0; ix1<m_view2DHash.size(); ix1++) {
    m_view2DHash[keyL[ix1]]->setStringList(hash);
  }
}



//! Bring up the dialog that gives the options for the axes widget
void rtMainWindow::showAxesOptions() {
  if (!m_axesProperties) return;

  if (m_axesProperties->exec() == QDialog::Accepted) {
    setViewType( m_axesProperties->getViewType() );
    setCoordType( m_axesProperties->getCoordType() );
  }
}

void rtMainWindow::textError(bool t) {
  rtMessage::instance().m_err = t;
}

void rtMainWindow::textWarning(bool t) {
  rtMessage::instance().m_warn = t;
}

void rtMainWindow::textDebug(bool t) {
  rtMessage::instance().m_debug = t;
}

void rtMainWindow::textBenchmark(bool t) {
  rtMessage::instance().m_bench = t;
}

void rtMainWindow::textLog(bool t) {
  rtMessage::instance().m_log = t;
}

//! Display the render options dialog box.
void rtMainWindow::showRenderOptions() {
  rtRenderOptions renOpt;

  if (m_inter3D->GetStillUpdateRate() != 0) {
    renOpt.renQuality( floor((1.0f/sqrt(m_inter3D->GetStillUpdateRate()+1.0f))*100.0f) );
  } else {
    renOpt.renQuality( 100 );
  }
  if (m_inter3D->GetDesiredUpdateRate() != 0) {
    renOpt.renUpdateQuality( floor((1.0f/sqrt(m_inter3D->GetDesiredUpdateRate()+1.0f))*100.0f) );
  } else {
    renOpt.renUpdateQuality( 100 );
  }

  if (m_renWin3D->GetStereoRender()) {
    renOpt.setStereoType(m_renWin3D->GetStereoType());
  } else {
    renOpt.setStereoType(0);
  }

  renOpt.setDirectRender(m_renWin3D->IsDirect());
  renOpt.setGLRender(m_renWin3D->SupportsOpenGL());

  if(renOpt.exec() == QDialog::Accepted) {  
    double renUpQual = renOpt.getRenUpdateQuality();
    double renQual = renOpt.getRenQuality();
    m_inter3D->SetDesiredUpdateRate( (100.0f/renUpQual)*(100.0f/renUpQual)-1 );
    m_inter3D->SetStillUpdateRate( (100.0f/renQual)*(100.0f/renQual)-1 );

    int type = renOpt.getStereoType();
    if (type == 0) {
      m_renWin3D->StereoRenderOff();
    } else {
      m_renWin3D->StereoRenderOn();
      m_renWin3D->SetStereoType(type);
    }

    m_renderFlag3D = true;
  }

}


void rtMainWindow::viewContents() {

}


void rtMainWindow::viewAbout() {
  Ui::rtAboutDialog setup;
  QDialog aboutD;
  QString ver;

  ver.append("Version: ");
  ver.append(QString::number(VURTIGO_MAJOR_VERSION));
  ver.append(".");
  ver.append(QString::number(VURTIGO_MINOR_VERSION));

  setup.setupUi(&aboutD);
  setup.versionLabel->setText( ver );
  aboutD.exec();

}

//! Load a plugin based on an XML file the user chooses. 
void rtMainWindow::loadPluginFile() {
  QString fName;

  fName = QFileDialog::getOpenFileName(this, "Select Plugin XML File", ".", "*.XML *.xml");

  QFile file(fName);
  if (!file.exists()) return;
  if (!rtPluginLoader::instance().loadPluginsFromConfig(&file)) {
    std::cout << "Failed to load plugins from: " << fName.toStdString() << std::endl;
  }
}

//! Add an object of a particular type.
void rtMainWindow::addNewObject() {
  QDialog *objDlg = new QDialog(this);
  Ui::newObjectDialog setupDlg;
  QString name;

  setupDlg.setupUi(objDlg);
  if(objDlg->exec() == QDialog::Accepted) {
    name = setupDlg.objNameLineEdit->text();
    if (name.length() <= 0) name = "Not Named";
    if (setupDlg.objTypeCombo->currentText() == "Piecewise Function") {
      rtObjectManager::instance().addObjectOfType(rtConstants::OT_vtkPiecewiseFunction, name);
    } else if (setupDlg.objTypeCombo->currentText() == "Color Transfer Function") {
      rtObjectManager::instance().addObjectOfType(rtConstants::OT_vtkColorTransferFunction, name);
    }
  }
}

//! Remove the currently selected item.
void rtMainWindow::removeSelectedObject() {
  QTreeWidgetItem* current;
  rtRenderObject* temp;

  current = objectTree->currentItem();

  if (!current) return;

  // Check for a heading.
  if (current->columnCount() == 1) {
    return;
  }

  // Get the object
  temp = rtObjectManager::instance().getObjectWithID(current->text(1).toInt());
  temp->removeFromRenderer(m_renderer3D);
  m_renderFlag3D = true;
  rtObjectManager::instance().removeObject(current->text(1).toInt());
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

//! Set the coordinate system that is shown by the axes.
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

//! Create a new 2D widget and return the ID
/*! Get the main window to create a new widget where a new 2D plane can be displayed.
  @return The ID of the newly created widget or -1 if the creation failed.
  */
int rtMainWindow::createNew2DWidget() {
  int currID = -1;
  rtOptions2DView* view;

  for (int ix1=0; ix1<m_max2DWidgets; ix1++) {
    if (!m_view2DHash.contains(ix1)) {
      currID = ix1;
      view = new rtOptions2DView(scrollAreaWidget);
      view->setContainer(scrollAreaWidget);
      m_scrollArea2DImagesLayout.addWidget(view);
      m_view2DHash.insert(currID, view);
      view->setStringList(rtObjectManager::instance().get2DObjectNameHash());
      break;
    }
  }
  return currID;
}

//! Remove the widget with a particular ID.
/*!
  @param id The ID of the widget to be removed.
  @return True if the widget was found and removed. False otherwise.
  */
bool rtMainWindow::remove2DWidget(int id) {
  if (!m_view2DHash.contains(id)) return false;

  rtOptions2DView* view;
  view = m_view2DHash.take(id);
  m_scrollArea2DImagesLayout.removeWidget(view);
  delete view;
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
  QList<int> keyList;

  keyList = m_view2DHash.keys();
  while (!keyList.empty()) {
    remove2DWidget(keyList.takeFirst());
  }

}

//! Called when the add button is pressed
void rtMainWindow::add2DFrame() {
  int res;
  res = createNew2DWidget();

  if (res == -1) {
    std::cout << "Error: Could not create new 2D widget" << std::endl;
  }

}

//! Called when the remove button is pressed.
/*!
  This function will try to remove all of the selected widgets.
  */
void rtMainWindow::remove2DFrame() {
  QList<int> keyList;

  keyList = m_view2DHash.keys();

  for (int ix1=0; ix1<keyList.count(); ix1++) {
    if(get2DWidget(keyList.at(ix1)) && get2DWidget(keyList.at(ix1))->isSelected()) {
      remove2DWidget(keyList.at(ix1));
    }
  }
}

//! Called when the remove all button is pressed.
void rtMainWindow::removeAll2DFrame() {
  QList<int> keyList;

  keyList = m_view2DHash.keys();

  for (int ix1=0; ix1<keyList.count(); ix1++) {
    remove2DWidget(keyList.at(ix1));
  }

}
