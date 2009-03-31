#include <QtGui>
#include <QApplication>
#include "rtMainWindow.h"

rtMainWindow::rtMainWindow(QWidget *parent, Qt::WindowFlags flags) {
  setupUi(this);

  m_render3DVTKWidget = new QVTKWidget(this->frame3DRender);
  m_render3DLayout = new QHBoxLayout();

  m_inter3D = m_render3DVTKWidget->GetInteractor();
  m_renWin3D = m_inter3D->GetRenderWindow();

  m_render3DLayout->addWidget(m_render3DVTKWidget);
  this->frame3DRender->setLayout(m_render3DLayout);

  // The GUI must be stretched more than the tabs. 
  mainRLSplitter->setStretchFactor(0, 1);
  mainRLSplitter->setStretchFactor(1, 25);

  // The 3D view must be larger than the 2D view. 
  mainUDSplitter->setStretchFactor(0, 5);
  mainUDSplitter->setStretchFactor(1, 1);

  connectSignals();
}

rtMainWindow::~rtMainWindow() {
  if (m_render3DVTKWidget) delete m_render3DVTKWidget;
  if (m_render3DLayout) delete m_render3DLayout;
}

void rtMainWindow::connectSignals() {

  connect(actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
}
