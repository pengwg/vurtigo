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
  m_forceSquare = false;
}

customQVTKWidget::~customQVTKWidget() {
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
  QVTKWidget::mousePressEvent(event);
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
