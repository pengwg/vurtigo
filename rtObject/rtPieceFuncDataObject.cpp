#include "rtPieceFuncDataObject.h"

//! Constructor
rtPieceFuncDataObject::rtPieceFuncDataObject() {
  setObjectType(rtConstants::OT_vtkPiecewiseFunction);
  m_pieceFunc = vtkPiecewiseFunction::New();
  setupGUI();
}

//! Destructor
rtPieceFuncDataObject::~rtPieceFuncDataObject() {
  cleanupGUI();
  m_pieceFunc->Delete();
}

//! Copy a new piecewise function into this object.
bool rtPieceFuncDataObject::setPiecewiseFunction(vtkPiecewiseFunction* piece) {
  if (!piece) return false;
  m_pieceFunc->DeepCopy(piece);
  return true;
}

//! Take the info from the GUI
void rtPieceFuncDataObject::apply() {

}

//! Send the info to the GUI
void rtPieceFuncDataObject::update() {

}

//! Set the GUI widgets.
void rtPieceFuncDataObject::setupGUI() {
  m_graph = new VtkPiecewiseGraph();
  QWidget *wid = getBaseWidget();
  m_mainLayout = new QBoxLayout(QBoxLayout::LeftToRight, wid);

  connect(m_graph, SIGNAL(functionUpdated()), this, SLOT(Modified()));

  wid->setLayout(m_mainLayout);
  m_mainLayout->addWidget(m_graph);
}

//! Clean the GUI widgets.
void rtPieceFuncDataObject::cleanupGUI() {
  m_mainLayout->removeWidget(m_graph);
  delete m_mainLayout;
  delete m_graph;
}
