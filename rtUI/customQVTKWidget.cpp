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
#include "customQVTKWidget.h"
#include "rtMainWindow.h"
#include "rtRenderObject.h"
#include "rtMainWindow.h"
#include "rtApplication.h"

#include <QMouseEvent>
#include <QList>

#include <iostream>

#include "vtkRenderer.h"
#include "vtkProp3DCollection.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkPropPicker.h"
#include "vtkCellPicker.h"

customQVTKWidget::customQVTKWidget(QWidget* parent, Qt::WFlags f) : QVTKWidget(parent, f) {
  m_propChosen = NULL;
  m_interactionMode = CAMERA_MODE;
  m_forceSquare = false;

  // This will send mouse move events even if nothing is clicked.
  this->setMouseTracking(true);

  // Set the cursor for the QVTK widgets to the arrow cursor.
  this->setCursor(Qt::ArrowCursor);
}

customQVTKWidget::~customQVTKWidget() {
}

void customQVTKWidget::calculateQtToVtk(int qtCoords[2], double vtkCoords[3]) {
  vtkCellPicker* pick = vtkCellPicker::New();
  vtkRenderer* ren = rtApplication::instance().getMainWinHandle()->getRenderer();
  int xVal, yVal;

  xVal = qtCoords[0];
  yVal = this->height() - qtCoords[1];

  pick->SetTolerance(0.01);
  if (pick->Pick(xVal, yVal, 0.0,  ren) ) {
    pick->GetPickPosition(vtkCoords);
  } else {
    vtkCoords[0] = 0.0;
    vtkCoords[1] = 0.0;
    vtkCoords[2] = 0.0;
  }

  if(pick) pick->Delete();
}

void customQVTKWidget::resizeEvent(QResizeEvent* event) {
  QSize newSize;
  QResizeEvent* e2;
  newSize = event->size();
  //std::cout << newSize.height() << " " << newSize.width() << std::endl;
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

void customQVTKWidget::paintEvent(QPaintEvent* event) {
  rtApplication::instance().getMainWinHandle()->setRenderFlag3D(true);
}


void customQVTKWidget::mousePressEvent(QMouseEvent* event) {


    for (int ix1=0; ix1<rtApplication::instance().getMainWinHandle()->getNumRenWins(); ix1++)
    {
        if (this == rtApplication::instance().getMainWinHandle()->getRenderWidget(ix1))
        {
            emit interMousePress(event,ix1);
            event->ignore();
        }
    }

}

void customQVTKWidget::mouseMoveEvent(QMouseEvent* event) {

  if( event->modifiers() & Qt::ShiftModifier ) {
    // Use the cross cursor when shift is down.
    this->setCursor(Qt::CrossCursor);

    int qtCoords[2];
    qtCoords[0] = event->x();
    qtCoords[1] = event->y();
    calculateQtToVtk(qtCoords, m_currentMousePosition);

    QString result("(");
    result += QString::number(m_currentMousePosition[0]);
    result += QString(", ");
    result += QString::number(m_currentMousePosition[1]);
    result += QString(", ");
    result += QString::number(m_currentMousePosition[2]);
    result += QString(")");

    rtApplication::instance().getMainWinHandle()->statusBar()->showMessage( result );
  } else {
    // Set this back to the default.
    this->setCursor(Qt::ArrowCursor);
  }

  for (int ix1=0; ix1<rtApplication::instance().getMainWinHandle()->getNumRenWins(); ix1++)
  {
      if (this == rtApplication::instance().getMainWinHandle()->getRenderWidget(ix1))
      {
          emit interMouseMove(event,ix1);
          event->ignore();
      }
  }
}

void customQVTKWidget::mouseReleaseEvent(QMouseEvent* event) {

    for (int ix1=0; ix1<rtApplication::instance().getMainWinHandle()->getNumRenWins(); ix1++)
    {
        if (this == rtApplication::instance().getMainWinHandle()->getRenderWidget(ix1))
        {
            emit interMouseRelease(event,ix1);
            event->ignore();
        }
    } 
}

void customQVTKWidget::mouseDoubleClickEvent(QMouseEvent* event) {

    for (int ix1=0; ix1<rtApplication::instance().getMainWinHandle()->getNumRenWins(); ix1++)
    {
        if (this == rtApplication::instance().getMainWinHandle()->getRenderWidget(ix1))
        {
            selectNewProp(event,ix1);
            emit interMouseDoubleClick(event,ix1);
            event->ignore();
        }
    }
}


void customQVTKWidget::keyPressEvent(QKeyEvent* event) {

  if (event->key() == Qt::Key_D) {
    rtApplication::instance().getMainWinHandle()->deselectAll();
  }

  else {
    for (int ix1=0; ix1<rtApplication::instance().getMainWinHandle()->getNumRenWins(); ix1++)
    {
        if (this == rtApplication::instance().getMainWinHandle()->getRenderWidget(ix1))
        {
            emit cameraKeyPress(event,ix1);
            emit interKeyPress(event,ix1);
            event->ignore();
        }
    }
  }
}


void customQVTKWidget::keyReleaseEvent(QKeyEvent* event) {

}


void customQVTKWidget::wheelEvent(QWheelEvent* event) {

    for (int ix1=0; ix1<rtApplication::instance().getMainWinHandle()->getNumRenWins(); ix1++)
    {
        if (this == rtApplication::instance().getMainWinHandle()->getRenderWidget(ix1))
        {
            emit interWheel(event,ix1);
            event->ignore();
        }
    }
}

void customQVTKWidget::selectNewProp(QMouseEvent* event, int window) {
  m_propChosen = NULL;

  if (!event) return;

  vtkPropPicker* picker = vtkPropPicker::New();
  int X = event->x();
  int Y = this->height()-event->y();

  if(picker->PickProp(X, Y, rtApplication::instance().getMainWinHandle()->getRenderer(window))) {
        for (int ix1=0; ix1<rtApplication::instance().getMainWinHandle()->getNumRenWins(); ix1++)
            rtApplication::instance().getMainWinHandle()->getRenderWidget(ix1)->setChosenProp(picker->GetViewProp());
  }
  else
  {
      for (int ix1=0; ix1<rtApplication::instance().getMainWinHandle()->getNumRenWins(); ix1++)
          rtApplication::instance().getMainWinHandle()->getRenderWidget(ix1)->setChosenProp(NULL);
  }

  picker->Delete();
}
