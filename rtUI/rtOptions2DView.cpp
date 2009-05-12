#include "rtOptions2DView.h"
#include <QBoxLayout>
#include <QSizePolicy>
#include <QResizeEvent>
#include <QPalette>

#include "vtkRenderer.h"

#include <iostream>

rtOptions2DView::rtOptions2DView(QWidget *parent, Qt::WindowFlags flags) {

  setupUi(this);
  m_renLayout = new QHBoxLayout();

  m_renderWidget = new customQVTKWidget(this->scrollAreaWidgetContents);
  m_renderWidget->setSquare(true);
  m_renderWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  this->scrollAreaWidgetContents->setLayout(m_renLayout);

  m_renLayout->addWidget(m_renderWidget);
  m_renLayout->setContentsMargins ( 1,1,1,1 );
  m_inter2D = m_renderWidget->GetInteractor();
  m_renWin2D = m_inter2D->GetRenderWindow();
  m_renderer2D = vtkRenderer::New();
  m_renWin2D->AddRenderer(m_renderer2D);

  m_container=NULL;

  m_selected = false;
  area2DView->setFrameStyle(QFrame::NoFrame);
  area2DView->setLineWidth(0);
}


rtOptions2DView::~rtOptions2DView() {
  m_renderer2D->Delete();

  if (m_renderWidget) delete m_renderWidget;
}

QSize rtOptions2DView::sizeHint() {
  QSize hint;
  hint = this->size();
  hint.setWidth( hint.height()-28-this->verticalLayout->spacing() );
  std::cout << "Size Hint: " << hint.height() << " " << hint.width() << std::endl;
  return hint;
}

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


void rtOptions2DView::moveEvent ( QMoveEvent * event ) {
  /*int numWid;
  int loc;

  numWid = this->parentWidget()->layout()->count();
  loc = this->parentWidget()->layout()->indexOf(this);

  if (event->pos().x() != this->size().width()*loc+8) {
    this->move(8+this->size().width()*loc, event->pos().y());
  }
  std::cout << "move" << std::endl;*/
}
