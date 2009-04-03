#include <QtGui>
#include <QApplication>
#include "rtMainWindow.h"
#include <iostream>

rtMainWindow::rtMainWindow(QWidget *parent, Qt::WindowFlags flags) {
  setupUi(this);

  m_render3DVTKWidget = new QVTKWidget(this->frame3DRender);
  m_render3DLayout = new QHBoxLayout();

  m_inter3D = m_render3DVTKWidget->GetInteractor();
  m_renWin3D = m_inter3D->GetRenderWindow();
  m_renderer3D = vtkRenderer::New();
  m_renWin3D->AddRenderer(m_renderer3D);

  m_render3DLayout->addWidget(m_render3DVTKWidget);
  this->frame3DRender->setLayout(m_render3DLayout);

  // The GUI must be stretched more than the tabs. 
  mainRLSplitter->setStretchFactor(0, 1);
  mainRLSplitter->setStretchFactor(1, 25);

  // The 3D view must be larger than the 2D view. 
  mainUDSplitter->setStretchFactor(0, 5);
  mainUDSplitter->setStretchFactor(1, 1);

  populateObjectTypeNames();
  connectSignals();
  setupObjectTree();
}

rtMainWindow::~rtMainWindow() {
  if (m_render3DVTKWidget) delete m_render3DVTKWidget;
  if (m_render3DLayout) delete m_render3DLayout;

  if (m_renderer3D) m_renderer3D->Delete();
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
    m_topItems.value(i.value()->getObjectType())->addChild(i.value()->getTreeItem());
  }
}

void rtMainWindow::connectSignals() {

  connect(actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
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
  
}

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
