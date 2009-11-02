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

  //! The the prop that was chosen by the last double click.
  /*!
    In interactive mode the user can double click on an object to select it. If no object is selected then NULL is returned.
    \return The pointer to the prop chosen or NULL if nothing is chosen.
    */
  vtkProp* getChosenProp() { return m_propChosen; }

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

  //! The widget is being repainted.
  /*!
    This function effectively resets the render flag to true for the renderer.
    */
  void paintEvent(QPaintEvent* event);

  //! Called when the window is resized.
  /*!
    This function is primarily used to adjust the dimensions of the render window. If the square condidion is true then this function will resize the window to make it a square.
    \param event The resize event which contains the new size of the widget.
    \sa m_forceSquare
    */
  virtual void resizeEvent(QResizeEvent* event);

  //! Called when the mouse button is pressed.
  virtual void mousePressEvent(QMouseEvent* event);

  //! Called if at least one mouse button is held down AND the mouse is being moved.
  virtual void mouseMoveEvent(QMouseEvent* event);

  //! Called when the click is released.
  virtual void mouseReleaseEvent(QMouseEvent* event);


  //! Called on a double click.
  /*!
  The double click event will be called AFTER a mouse press and mouse release event.
  This function calls the event ignore function which passes the event up to the parent.
  */
  virtual void mouseDoubleClickEvent(QMouseEvent* event);

  //! Called when a key is pressed.
  virtual void keyPressEvent(QKeyEvent* event);

  //! Called when a key is released.
  virtual void keyReleaseEvent(QKeyEvent* event);

  //! The mouse wheel was moved.
  virtual void wheelEvent(QWheelEvent* event);

  //! Use prop picking to select a new prop based on a mouse double-click.
  void selectNewProp(QMouseEvent* event);

  //! The prop that is currently chosen. NULL if no prop is chosen.
  vtkProp* m_propChosen;

  //! True if this widget is restricted to be a square.
  bool m_forceSquare;

  //! The type of interaction the user has selected.
  InteractionMode m_interactionMode;
};

#endif
