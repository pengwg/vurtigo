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
#include <QMouseEvent>
#include <QWheelEvent>

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

  //! Set the chosen prop
  void setChosenProp(vtkProp* prop) {m_propChosen = prop;}

  //! Calculate the position of Qt X,Y into vtk X,Y,Z
  void calculateQtToVtk(int qtCoords[2], double vtkCoords[3]);

  //! Get the current mouse position
  void getCurrentMousePosition(double pos[3]) {
    pos[0] = m_currentMousePosition[0];
    pos[1] = m_currentMousePosition[1];
    pos[2] = m_currentMousePosition[2];
  }

  //if we are processing it for the first time, do it once and then ignore all subsequent calls for this event (prevents multiple objects
  // from requesting the camera to handle all their non-interactive mouse events)

  //! Want Camera to take over processing this mouse press event
  void camTakeOverMousePress(QMouseEvent *ev,int window) { if (ev->isAccepted()) { emit cameraMousePress(ev,window); ev->ignore();} }

  //! Want Camera to take over processing this mouse move event
  void camTakeOverMouseMove(QMouseEvent *ev,int window) { if (ev->isAccepted()) { emit cameraMouseMove(ev,window); ev->ignore();} }

  //! Want Camera to take over processing this mouse release event
  void camTakeOverMouseRelease(QMouseEvent *ev,int window) {if (ev->isAccepted()) { emit cameraMouseRelease(ev,window); ev->ignore();} }

  //! Want Camera to take over processing this mouse wheel event
  void camTakeOverMouseWheel(QWheelEvent *ev,int window) { if (ev->isAccepted()) { emit cameraWheel(ev,window); ev->ignore();} }

signals:
  void cameraMousePress(QMouseEvent*,int);
  void cameraMouseMove(QMouseEvent*,int);
  void cameraMouseRelease(QMouseEvent*,int);
  void cameraMouseDoubleClick(QMouseEvent*,int);
  void cameraKeyPress(QKeyEvent*,int);
  void cameraKeyRelease(QKeyEvent*,int);
  void cameraWheel(QWheelEvent*,int);

  void interMousePress(QMouseEvent*,int);
  void interMouseMove(QMouseEvent*,int);
  void interMouseRelease(QMouseEvent*,int);
  void interMouseDoubleClick(QMouseEvent*,int);
  void interKeyPress(QKeyEvent*,int);
  void interKeyRelease(QKeyEvent*,int);
  void interWheel(QWheelEvent*,int);

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
  void selectNewProp(QMouseEvent* event, int window);

  //! The prop that is currently chosen. NULL if no prop is chosen.
  vtkProp* m_propChosen;

  //! True if this widget is restricted to be a square.
  bool m_forceSquare;

  //! The type of interaction the user has selected.
  InteractionMode m_interactionMode;

  //! The current mouse position in x,y,z
  double m_currentMousePosition[3];
};

#endif
