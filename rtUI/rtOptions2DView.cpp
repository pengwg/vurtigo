#include "rtOptions2DView.h"
#include <QBoxLayout>
#include <QSizePolicy>
#include <QResizeEvent>

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
  m_renLayout->setContentsMargins ( 0,0,0,0 );
  m_inter2D = m_renderWidget->GetInteractor();
  m_renWin2D = m_inter2D->GetRenderWindow();
  m_renderer2D = vtkRenderer::New();
  m_renWin2D->AddRenderer(m_renderer2D);

  m_container=NULL;
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

  numWid = m_container->layout()->count();

  // Spacing.
  spac = 5;
  // Margins
  mar = 3;

  // std::cout << "Resize Event: " << event->size().height() << " " << event->size().width() << std::endl;
  if (event->size().height() != event->size().width()-28-this->verticalLayout->spacing() && m_container) {
    resize(event->size().height()-28-this->verticalLayout->spacing(), event->size().height());

    if (m_container->minimumWidth() != (event->size().height()-28-this->verticalLayout->spacing())*numWid+mar*2+(numWid-1)*spac ) {
      m_container->setMinimumWidth( (event->size().height()-28-this->verticalLayout->spacing())*numWid+mar*2+(numWid-1)*spac);
      m_container->layout()->invalidate();
    }
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
