#include "rtOptions2DView.h"
#include "rtObjectManager.h"
#include "rtRenderObject.h"

#include <QBoxLayout>
#include <QSizePolicy>
#include <QResizeEvent>
#include <QPalette>
#include <QStringList>

#include "vtkRenderer.h"
#include "vtkProp.h"
#include "vtkCamera.h"

#include <iostream>

rtOptions2DView::rtOptions2DView(QWidget *parent, Qt::WindowFlags flags) {

  setupUi(this);
  m_renLayout = new QHBoxLayout();

  m_renderWidget = new customQVTKWidget(this->scrollAreaWidgetContents);
  m_renderWidget->setSquare(true);
  m_renderWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  this->scrollAreaWidgetContents->setLayout(m_renLayout);

  m_interactor = vtkInteractorStyleRubberBand2D::New();

  m_renLayout->addWidget(m_renderWidget);
  m_renLayout->setContentsMargins ( 1,1,1,1 );
  m_renWin2D = m_renderWidget->GetRenderWindow();
  m_inter2D = m_renderWidget->GetInteractor();
  m_inter2D->SetInteractorStyle(m_interactor);
  m_renderer2D = vtkRenderer::New();
  m_renWin2D->AddRenderer(m_renderer2D);
  m_renderer2D->GetActiveCamera()->ParallelProjectionOn();
  m_container=NULL;

  m_selected = false;
  area2DView->setFrameStyle(QFrame::NoFrame);
  area2DView->setLineWidth(0);

  m_renderFlag = false;
  m_currProp=NULL;
  m_currRenObj=NULL;

  connect(combo2DObjects, SIGNAL(currentIndexChanged(int)), this, SLOT(comboIndexChanged(int)));
}


rtOptions2DView::~rtOptions2DView() {
  m_renderer2D->Delete();
  m_interactor->Delete();

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

  }
}

//! Overwrite to ensure that the size remains a square.
QSize rtOptions2DView::sizeHint() {
  QSize hint;
  hint = this->size();
  hint.setWidth( hint.height()-28-this->verticalLayout->spacing() );
  //std::cout << "Size Hint: " << hint.height() << " " << hint.width() << std::endl;
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
  spac = 6;
  // Margins
  mar = 6;
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
  if (index == -1) return;

  comboText = combo2DObjects->currentText();
  splitter = comboText.split(" ");

  if (splitter.size() <= 1) {
    std::cout << "Could not separate value from name in 2D widget: " << comboText.toStdString() << std::endl;
    return;
  }
  id = splitter.takeFirst().toInt(&convOK);
  if (!convOK) {
    std::cout << "First Value not an Integer: " << comboText.toStdString() << std::endl;
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

  //std::cout << id << "  with  " << name.toStdString() << std::endl;

  m_currRenObj = rtObjectManager::instance().getObjectWithID(id);
  if (!m_currRenObj) {
    std::cout << "Object with ID: " << id << " no longer exists." << std::endl;
    return;
  }

  if (!m_currRenObj->viewWithNameExists(name)) {
    std::cout << "No view with the name of: " << name.toStdString() << std::endl;
    return;
  }
  m_currRenObj->update();
  m_currProp = m_currRenObj->get2DViewWithName(name);


  // Remove the current prop (if any)
  m_renderer2D->RemoveAllViewProps();
  m_renderer2D->AddViewProp(m_currProp);

  m_renderFlag=true;
}
