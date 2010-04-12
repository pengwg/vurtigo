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
#ifndef RT_CAMERA_CONTROL_H
#define RT_CAMERA_CONTROL_H

#include <QObject>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QPoint>
#include <QList>

#include <vtkCamera.h>
#include <vtkMatrix4x4.h>

class customQVTKWidget;

//! Control the camera positioning and motion
/*!
  This class supports the controls for the motion and positioning of the camera in the main 3D view.
  Mouse and keyboard events can be received from the QVTK widget and are then passed to the camera control where the motion of the camera is determined.
  */
class rtCameraControl : public QObject {
  Q_OBJECT
public:

  //! The camera position
  /*!
    The camera view can be defined by three parameters: position, focal point, and the vector that points in the up direction.
    The position is the x,y,z coordinate set where the camera is located in 3D space. The focal point is the 3D point that the camera is facing towards. The vector from the position to the focal point defines the "direction of projection" or "forward direction" for the camera. To finalize the orientation of the camera a vector perpendicular to the "forward direction" must be used to define the up direction for the camera.
    */
  struct CameraPosition {
    //! The camera position (x,y,z).
    double pos[3];
    //! The camera focal point (x,y,z).
    double focal[3];
    //! The vector that points in the camera up direction (x,y,z).
    double up[3];
  };

  //! Constructor
  /*!
    \param cam The vtkCamera type object that is the camera from the main 3D view
    \param eventWidget The QVTK widget where the rendering is performed. This widget also receives all of the mouse events that interact with the 3D scene.
    */
  rtCameraControl(vtkCamera* cam, customQVTKWidget* eventWidget);

  //! Check if the camera is moving at this moment.
  /*!
    It is important for the 3D renderer to know when the camera is moving. To increase the responsiveness of the camera, a reduced quality rendering can be shown while the camera is moving.
    \return true if the camera is currently in motion. False otherwise.
    */
  bool cameraMoving();

  //! Get the direction in which the camera is facing.
  /*!
    Returns the vector from the position of the camera to the focal point.
    \param val Array where the forward direction of the camera will be stored.
    */
  void getForwardDirection(double val[3]);

  //! Get the direction of the up vector.
  /*!
    \param val Array where the up direction will be stored.
    */
  void getUpDirection(double val[3]);

  //! Get the direction of the vector pointing to the right of the camera.
  /*!
    The right vector is just the cross product between the forward vector and the up vector.
    \param val Array where the vector will be stored.
    */
  void getRightDirection(double val[3]);

  //! Get the matrix that represents the position and orientation of the camera.
  /*!
    \return A pointer to the view matrix
    */
  vtkMatrix4x4* getViewMatrix() { return m_camera->GetViewTransformMatrix(); }

  //! Restore the camera to the default position and orientation
  void setToDefaultPosition();
  //! Set the camera to the position and orientation as viewed by the operator controling the robot arm.
  void setToRobotArmPosition();

public slots:
  //! Slot called when the mouse is pressed in the QVTK widget.
  void mousePress(QMouseEvent*);
  //! Slot is called when the mouse is moved. These events only occur when at least one of the mouse buttons is pressed down. (See QMouseEvent).
  void mouseMove(QMouseEvent*);
  //! Slot called when a mouse button is released.
  void mouseRelease(QMouseEvent*);
  //! Slot called when the mouse button is double clicked.
  void mouseDoubleClick(QMouseEvent*);
  //! Slot called when a key is pressed.
  void keyPress(QKeyEvent*);
  //! Slot called when a key is released.
  void keyRelease(QKeyEvent*);
  //! Slot called when the mouse wheel is moved.
  void wheel(QWheelEvent*);

protected:
  //! The pointer to the vtk camera object in the main 3D view.
  vtkCamera* m_camera;

  //! Pointer the QVTK widget.
  customQVTKWidget* m_eventWidget;

  //! The last known position of the mouse.
  /*!
    To determine how much the mouse has been moved across the screen the position of the last mouse pointer location is stored. The movement length and direction is calculated based on this value and the value of the current mouse position. This position is updated on mousePress and mouseMove.
    \sa mousePress(QMouseEvent*)
    \sa mouseMove(QMouseEvent*)
    */
  QPoint m_lastPoint;

  //! True if the left mouse button is down.
  bool m_leftMouseDown;
  //! True if the right mouse button is down.
  bool m_rightMouseDown;
  //! True if the middle mouse button is down.
  bool m_midMouseDown;

  //! A list of saved camera views
  /*!
    The list maintains a set of camera positions and orientations. It can be used to quickly jump to a predefined view.
    */
  QList<CameraPosition> m_cameraPosList;

  //! Connect the signals to the slots
  /*!
    Connect the relevant signals from the QVTK widget to the corresponding slots in this class.
    Signals will not be connected if the pointer to the QVTK widget is NULL.
    */
  void connectEvents();
};

#endif // RT_CAMERA_CONTROL_H
