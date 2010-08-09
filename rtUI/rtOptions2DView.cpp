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
#include "rtOptions2DView.h"
#include "rtObjectManager.h"
#include "rtRenderObject.h"
#include "rtMessage.h"
#include "rtApplication.h"

#include <QBoxLayout>
#include <QSizePolicy>
#include <QResizeEvent>
#include <QPalette>
#include <QStringList>

#include <vtkActor2D.h>
#include <vtkRenderer.h>
#include <vtkProp.h>
#include <vtkCamera.h>

#include <iostream>

rtOptions2DView::rtOptions2DView(QWidget *parent, Qt::WindowFlags flags) {

  setupUi(this);
  m_renLayout = new QHBoxLayout();

  m_renderWidget = new customQVTKWidget(this->scrollAreaWidgetContents);
  m_renderWidget->setSquare(true);
  m_renderWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

  this->scrollAreaWidgetContents->setLayout(m_renLayout);

  m_renLayout->addWidget(m_renderWidget);
  m_renLayout->setContentsMargins ( 1,1,1,1 );
  m_renWin2D = m_renderWidget->GetRenderWindow();
  m_renderer2D = vtkRenderer::New();
  m_renWin2D->AddRenderer(m_renderer2D);
  m_renderer2D->GetActiveCamera()->ParallelProjectionOn();
  m_container=NULL;

  m_selected = false;
  area2DView->setFrameStyle(QFrame::NoFrame);
  area2DView->setLineWidth(0);

  // Render the empty image
  m_renderer2D->RemoveAllViewProps();
  m_renderFlag = true;
  m_currProp=NULL;
  m_currRenObj=NULL;

  connect(combo2DObjects, SIGNAL(currentIndexChanged(int)), this, SLOT(comboIndexChanged(int)));
}


rtOptions2DView::~rtOptions2DView() {
  m_renderer2D->Delete();

  if (m_renderWidget) delete m_renderWidget;
}

//! Set a new string list for the QComboBox
/*!
  @param textList A hash of object IDs and a series of names for each 2D object
  */
void rtOptions2DView::setStringList(QHash<int, QString> *textList) {
  QString currTxt;
  int currIdx;
  QStringList items;
  QList<int> keyL;

  items.clear();
  currIdx = combo2DObjects->currentIndex();
  currTxt = combo2DObjects->currentText();

  // Remove the old entries.
  combo2DObjects->clear();
  keyL = textList->keys();
  for (int ix1=0; ix1<textList->size(); ix1++) {
    items.append(QString::number(keyL[ix1]) + QString(" ") + textList->values()[ix1]);
  }

  // Add the new items.
  combo2DObjects->addItems(items);
  if (currIdx > -1 && combo2DObjects->findText(currTxt)>-1) {
    combo2DObjects->setCurrentIndex(combo2DObjects->findText(currTxt));
  } else {
    combo2DObjects->setCurrentIndex(0);
  }

}

//! Try to re-render (refresh) this 2D window.
void rtOptions2DView::tryRender() {
  if (m_renderFlag && m_currProp) {
    m_renderFlag = false;
    m_currProp->Modified();
    m_renWin2D->Modified();
    m_renWin2D->Render();
  } else if (m_renderFlag) {
    m_renderFlag = false;
    m_renWin2D->Modified();
    m_renWin2D->Render();
  }
}

//! Overwrite to ensure that the size remains a square.
QSize rtOptions2DView::sizeHint() {
  QSize hint;
  hint = this->size();
  hint.setWidth( hint.height()-28-this->verticalLayout->spacing() );
  return hint;
}

//! When this widget is resized we need to ensure that it remains a square.
void rtOptions2DView::resizeEvent ( QResizeEvent * event ) {
  int numWid;
  int spac;
  int mar;
  int sizeOptionsAbove;

  numWid = m_container->layout()->count();

  // Spacing.
  spac = 5;
  // Margins
  mar = 5;
  // Size of the options above the render window
  sizeOptionsAbove = 28;


  // std::cout << "Resize Event: " << event->size().height() << " " << event->size().width() << std::endl;
  if (event->size().height() != event->size().width()-sizeOptionsAbove-this->verticalLayout->spacing() && m_container) {
    resize(event->size().height()-sizeOptionsAbove-this->verticalLayout->spacing(), event->size().height());

    if (m_container->minimumWidth() != (event->size().height()-sizeOptionsAbove-this->verticalLayout->spacing())*numWid+mar*2+(numWid-1)*spac ) {
      m_container->setMinimumWidth( (event->size().height()-sizeOptionsAbove-this->verticalLayout->spacing())*numWid+mar*2+(numWid-1)*spac);
      m_container->setMaximumWidth( (event->size().height()-sizeOptionsAbove-this->verticalLayout->spacing())*numWid+mar*2+(numWid-1)*spac);
      m_container->layout()->invalidate();
    }
  }
}

//! The mouse double clicked on the widget.
/*!
  The widget has been selected. (or de-selected)
  */
void rtOptions2DView::mouseDoubleClickEvent(QMouseEvent* event) {
  QPalette textCol;
  QColor myRed(255, 0, 0);

  if (m_selected) {
    m_selected = false;
    area2DView->setFrameStyle(QFrame::NoFrame);
    area2DView->setLineWidth(0);
  } else {
    m_selected = true;
    textCol.setColor(QPalette::WindowText, myRed);
    area2DView->setPalette(textCol);
    area2DView->setFrameStyle(QFrame::Box | QFrame::Plain);
    area2DView->setLineWidth(4);
  }
}

//! Called when the widget is moved.
void rtOptions2DView::moveEvent ( QMoveEvent * event ) {

}


//! The user has changed the index value for the item combo box
void rtOptions2DView::comboIndexChanged(int index) {
  QStringList splitter;
  int id;
  bool convOK;
  QString name;
  QString comboText;

  // No selection.
  if (index == -1) {
    m_renderer2D->RemoveAllViewProps();
    m_currProp = NULL;
    m_currRenObj = NULL;
    m_renderFlag=true;
    return;
  }

  comboText = combo2DObjects->currentText();
  splitter = comboText.split(" ");

  if (splitter.size() <= 1) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Could not separate value from name in 2D widget: ").append(comboText));
    return;
  }
  id = splitter.takeFirst().toInt(&convOK);
  if (!convOK) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("First Value not an Integer: ").append(comboText));
    return;
  }

  // Check for the NONE object with an ID of -1.
  if (id < 0) {
    m_renderer2D->RemoveAllViewProps();
    m_currProp = NULL;
    m_currRenObj = NULL;
    m_renderFlag=true;
    return;
  }

  name = splitter.join("");

  m_currRenObj = rtApplication::instance().getObjectManager()->getObjectWithID(id);
  if (!m_currRenObj) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Object with ID not found: ").append(QString::number(id)));
    return;
  }

  if (!m_currRenObj->viewWithNameExists(name)) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("No view with the name of: ").append(name));
    return;
  }
  m_currRenObj->tryUpdate();
  m_currProp = m_currRenObj->get2DViewWithName(name);

  // Remove the current prop (if any)
  m_renderer2D->RemoveAllViewProps();
  m_renderer2D->AddViewProp(m_currProp);

  m_renderFlag=true;
}
