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

    this->OutlineActor->SetVisibility( true );
    if (this->State == vtkOrientationMarkerWidget::Outside)
    {
      this->OutlineActor->SetVisibility( false );
      this->Moving = 0;
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
    m_leftMouseDown = false;
    if (this->State == vtkOrientationMarkerWidget::Outside) return;

    // finalize any corner adjustments
    this->SquareRenderer();
    this->UpdateOutline();

    this->OutlineActor->SetVisibility( false );

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
