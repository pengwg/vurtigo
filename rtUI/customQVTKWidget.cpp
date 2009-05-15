#include "customQVTKWidget.h"
#include "rtMainWindow.h"
#include "rtObjectManager.h"
#include "rtRenderObject.h"

#include <QMouseEvent>
#include <QList>

#include <iostream>

#include "vtkRenderer.h"
#include "vtkProp3DCollection.h"
#include "vtkRenderWindowInteractor.h"

customQVTKWidget::customQVTKWidget(QWidget* parent, Qt::WFlags f) : QVTKWidget(parent, f) {
  m_objectPicker = vtkPropPicker::New();
  m_currProp = NULL;
  m_currRenObj = NULL;

  m_forceSquare = false;
}

customQVTKWidget::~customQVTKWidget() {
  if (m_objectPicker) m_objectPicker->Delete();
}

//! Called when the window is resized.
void customQVTKWidget::resizeEvent(QResizeEvent* event) {
  QSize newSize;
  QResizeEvent* e2;
  newSize = event->size();

  if (m_forceSquare && newSize.height()!=newSize.width()) {
    // Force a square
    newSize.setWidth(newSize.height());
    resize (newSize.height(), newSize.height());
    e2 = new QResizeEvent(newSize, event->oldSize());
    QVTKWidget::resizeEvent(e2);
    delete e2;
  } else {
    QVTKWidget::resizeEvent(event);
  }
}

//! Called when one of the mouse buttons is pressed.
void customQVTKWidget::mousePressEvent(QMouseEvent* event) {
  vtkRenderer* tempRen;
  vtkPropCollection* propCol;
  vtkPropCollection* tempPropList = vtkPropCollection::New(); // A copy of the collection from the renderer.
  int renSize[2];
  bool eventUsed = false;
  int numItems;


/*
  tempRen = rtObjectManager::instance().getMainWinHandle()->getRenderer();
  propCol = tempRen->GetViewProps();
  numItems = propCol->GetNumberOfItems();

  propCol->InitTraversal();
  for (int ix1=0; ix1<numItems; ix1++) {
    tempPropList->AddItem( propCol->GetNextProp() );
  }

  rtObjectManager::instance().getMainWinHandle()->getInteractor()->GetSize(renSize);

  while (tempPropList->GetNumberOfItems()>0 && !eventUsed) {

    if ( m_objectPicker->PickProp(event->x(), renSize[1]-event->y()-1, tempRen, tempPropList) ) {
      m_currProp = m_objectPicker->GetViewProp();
      tempPropList->RemoveItem(m_currProp);
    } else {
      // No more props are captured by the ray.
      m_currProp = NULL;
      break;
    }

    // Check that the object is visible and pikable
    if (!m_currProp->GetVisibility() || !m_currProp->GetPickable()) continue;

    m_currRenObj = rtObjectManager::instance().getObjectWith3DProp(m_currProp);

    if (m_currProp && m_currRenObj) {
      eventUsed = m_currRenObj->mousePressEvent(event);
    }
  } */
  tempPropList->RemoveAllItems();
  tempPropList->Delete();

  if (!eventUsed) {
    m_currProp = NULL;
    m_currRenObj = NULL;

    // Pass the message on.
    QVTKWidget::mousePressEvent(event);
  }
}

//! Called as the mouse is moved.
void customQVTKWidget::mouseMoveEvent(QMouseEvent* event) {
  QVTKWidget::mouseMoveEvent(event);
}

//! Called when the click is released.
void customQVTKWidget::mouseReleaseEvent(QMouseEvent* event) {
  QVTKWidget::mouseReleaseEvent(event);
}

//! Called on a double click.
/*!
  The double click event will be called AFTER a mouse press and mouse release event.
  This function calls the event ignore function which passes the event up to the parent.
  */
void customQVTKWidget::mouseDoubleClickEvent(QMouseEvent* event) {
  QVTKWidget::mouseDoubleClickEvent(event);
  event->ignore();
}
