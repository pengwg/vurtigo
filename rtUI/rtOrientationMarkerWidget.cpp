#include "rtOrientationMarkerWidget.h"
#include "rtApplication.h"
#include "rtMainWindow.h"
#include "customQVTKWidget.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkActor2D.h"

rtOrientationMarkerWidget::rtOrientationMarkerWidget(customQVTKWidget* eventWid) {
  m_eventWidget = eventWid;

  connectEvents();
}


rtOrientationMarkerWidget::~rtOrientationMarkerWidget() {
}

void rtOrientationMarkerWidget::connectEvents() {
  if (!m_eventWidget) return;
  QObject::connect(m_eventWidget, SIGNAL(interMousePress(QMouseEvent*,int)), this, SLOT(mousePress(QMouseEvent*)));
  QObject::connect(m_eventWidget, SIGNAL(interMouseMove(QMouseEvent*,int)), this, SLOT(mouseMove(QMouseEvent*)));
  QObject::connect(m_eventWidget, SIGNAL(interMouseRelease(QMouseEvent*,int)), this, SLOT(mouseRelease(QMouseEvent*)));
  QObject::connect(m_eventWidget, SIGNAL(interMouseDoubleClick(QMouseEvent*,int)), this, SLOT(mouseDoubleClick(QMouseEvent*)));
  QObject::connect(m_eventWidget, SIGNAL(interKeyPress(QKeyEvent*,int)), this, SLOT(keyPress(QKeyEvent*)));
  QObject::connect(m_eventWidget, SIGNAL(interKeyRelease(QKeyEvent*,int)), this, SLOT(keyRelease(QKeyEvent*)));
  QObject::connect(m_eventWidget, SIGNAL(interWheel(QWheelEvent*,int)), this, SLOT(wheel(QWheelEvent*)));
}


void rtOrientationMarkerWidget::mousePress(QMouseEvent* ev) {
  if (!GetInteractive()) return;
  QPoint newPt;
  newPt = ev->pos();

  m_winSize = m_eventWidget->size();

  if (ev->button() == Qt::LeftButton) {
    m_leftMouseDown = true;

    // are we over the widget?
    double vp[4];
    this->Renderer->GetViewport( vp );

    this->Renderer->NormalizedDisplayToDisplay( vp[0], vp[1] );
    this->Renderer->NormalizedDisplayToDisplay( vp[2], vp[3] );

    int pos1[2] = { static_cast<int>( vp[0] ), static_cast<int>( vp[1] ) };
    int pos2[2] = { static_cast<int>( vp[2] ), static_cast<int>( vp[3] ) };

    this->StartPosition[0] = ev->x();
    this->StartPosition[1] = m_winSize.height()-ev->y();

    // flag that we are attempting to adjust or move the outline
    this->Moving = 1;
    this->State = this->ComputeStateBasedOnPosition( this->StartPosition[0], this->StartPosition[1], pos1, pos2 );
    this->SetCursor( this->State );

    if (this->State == vtkOrientationMarkerWidget::Outside)
    {
      this->Moving = 0;
    }
    else {
      if (rtApplication::instance().getMainWinHandle()->getRenderWidget(0)->getChosenProp() != NULL)
        rtApplication::instance().getMainWinHandle()->deselectAll();
      for (int ix1=0; ix1<rtApplication::instance().getMainWinHandle()->getNumRenWins(); ix1++)
        rtApplication::instance().getMainWinHandle()->getRenderWidget(ix1)->setChosenProp(this->GetOrientationMarker());
    }
    rtApplication::instance().getMainWinHandle()->setRenderFlag3D(true);
  } else if (ev->button() == Qt::RightButton) {
    m_rightMouseDown = true;
  } else if (ev->button() == Qt::MidButton) {
    m_midMouseDown = true;
  }
}

void rtOrientationMarkerWidget::mouseMove(QMouseEvent* ev) {
  if (!GetInteractive()) return;

  if (rtApplication::instance().getMainWinHandle()->getRenderWidget(0)->
        getChosenProp() != this->GetOrientationMarker())
  {
    return;
  }

  m_winSize = m_eventWidget->size();

  if (m_leftMouseDown) {
    // compute some info we need for all cases
    int X = ev->x();
    int Y = m_winSize.height()-ev->y();

    double vp[4];
    this->Renderer->GetViewport( vp );

    // compute display bounds of the widget to see if we are inside or outside
    this->Renderer->NormalizedDisplayToDisplay( vp[0], vp[1] );
    this->Renderer->NormalizedDisplayToDisplay( vp[2], vp[3] );

    int pos1[2] = { static_cast<int>( vp[0] ), static_cast<int>( vp[1] ) };
    int pos2[2] = { static_cast<int>( vp[2] ), static_cast<int>( vp[3] ) };

    int state = this->ComputeStateBasedOnPosition( X, Y, pos1, pos2);
    this->State = this->Moving ? this->State : state;
    this->SetCursor( this->State );

    if (this->State == vtkOrientationMarkerWidget::Outside || !this->Moving)
    {
      rtApplication::instance().getMainWinHandle()->setRenderFlag3D(true);
      return;
    }

    // based on the state set when the left mouse button is clicked,
    // adjust the renderer's viewport
    switch (this->State)
    {
    case vtkOrientationMarkerWidget::AdjustingP1:
      this->ResizeBottomLeft( X, Y );
      break;
    case vtkOrientationMarkerWidget::AdjustingP2:
      this->ResizeBottomRight( X, Y );
      break;
    case vtkOrientationMarkerWidget::AdjustingP3:
      this->ResizeTopRight( X, Y );
      break;
    case vtkOrientationMarkerWidget::AdjustingP4:
      this->ResizeTopLeft( X, Y );
      break;
    case vtkOrientationMarkerWidget::Translating:
      this->MoveWidget( X, Y );
      break;
    }

    this->UpdateOutline();
    rtApplication::instance().getMainWinHandle()->setRenderFlag3D(true);
  }
}

void rtOrientationMarkerWidget::mouseRelease(QMouseEvent* ev) {
  if (!GetInteractive()) return;

  m_winSize = m_eventWidget->size();

  if (ev->button() == Qt::LeftButton) {

    if (rtApplication::instance().getMainWinHandle()->getRenderWidget(0)->
          getChosenProp() == this->GetOrientationMarker())
    {
      for (int ix1=0; ix1<rtApplication::instance().getMainWinHandle()->getNumRenWins(); ix1++)
         rtApplication::instance().getMainWinHandle()->getRenderWidget(ix1)->setChosenProp(NULL);
    }

    m_leftMouseDown = false;
    if (this->State == vtkOrientationMarkerWidget::Outside) return;

    // finalize any corner adjustments
    this->SquareRenderer();
    this->UpdateOutline();

    // stop adjusting
    this->State = vtkOrientationMarkerWidget::Outside;
    this->Moving = 0;

    this->RequestCursorShape( VTK_CURSOR_DEFAULT );
    rtApplication::instance().getMainWinHandle()->setRenderFlag3D(true);
  } else if (ev->button() == Qt::RightButton) {
    m_rightMouseDown = false;
  } else if (ev->button() == Qt::MidButton) {
    m_midMouseDown = false;
  }
}

void rtOrientationMarkerWidget::mouseDoubleClick(QMouseEvent* ev) {
  if (!GetInteractive()) return;
}

void rtOrientationMarkerWidget::keyPress(QKeyEvent* ev) {
  if (!GetInteractive()) return;
}

void rtOrientationMarkerWidget::keyRelease(QKeyEvent* ev) {
  if (!GetInteractive()) return;
}

void rtOrientationMarkerWidget::wheel(QWheelEvent* ev) {
  if (!GetInteractive()) return;
}
