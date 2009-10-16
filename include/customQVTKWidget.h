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
#ifndef CUSTOM_QVTK_WIDGET_H
#define CUSTOM_QVTK_WIDGET_H

#include "QVTKWidget.h"
#include "vtkPropPicker.h"
#include "vtkProp.h"

#include <QObject>

class rtRenderObject;

//! A major modification of the original QVTKWidget
class customQVTKWidget : public QVTKWidget {

  Q_OBJECT

 public:

  enum InteractionMode {
    CAMERA_MODE,
    INTERACTION_MODE,
    PLACE_MODE
  };

  customQVTKWidget(QWidget* parent = NULL, Qt::WFlags f = 0);
  ~customQVTKWidget();

  void setSquare(bool sq) { m_forceSquare = sq; }
  bool isSquare() { return m_forceSquare; }

  InteractionMode getInteraction() { return m_interactionMode; }
  void setInteraction(InteractionMode mode) { m_interactionMode = mode; }
signals:
  void cameraMousePress(QMouseEvent*);
  void cameraMouseMove(QMouseEvent*);
  void cameraMouseRelease(QMouseEvent*);
  void cameraMouseDoubleClick(QMouseEvent*);
  void cameraKeyPress(QKeyEvent*);
  void cameraKeyRelease(QKeyEvent*);
  void cameraWheel(QWheelEvent*);

  void interMousePress(QMouseEvent*);
  void interMouseMove(QMouseEvent*);
  void interMouseRelease(QMouseEvent*);
  void interMouseDoubleClick(QMouseEvent*);
  void interKeyPress(QKeyEvent*);
  void interKeyRelease(QKeyEvent*);
  void interWheel(QWheelEvent*);

  void placeMousePress(QMouseEvent*);
  void placeMouseMove(QMouseEvent*);
  void placeMouseRelease(QMouseEvent*);
  void placeMouseDoubleClick(QMouseEvent*);
  void placeKeyPress(QKeyEvent*);
  void placeKeyRelease(QKeyEvent*);
  void placeWheel(QWheelEvent*);

 protected:

  void paintEvent(QPaintEvent* event);

  virtual void resizeEvent(QResizeEvent* event);
  virtual void mousePressEvent(QMouseEvent* event);
  virtual void mouseMoveEvent(QMouseEvent* event);
  virtual void mouseReleaseEvent(QMouseEvent* event);
  virtual void mouseDoubleClickEvent(QMouseEvent* event);

  virtual void keyPressEvent(QKeyEvent* event);
  virtual void keyReleaseEvent(QKeyEvent* event);

  virtual void wheelEvent(QWheelEvent* event);

  bool m_forceSquare;
  InteractionMode m_interactionMode;
};

#endif
