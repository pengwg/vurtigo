/***************************************************************************
Vurtigo: Visualization software for interventional applications in medicine


Copyright (c) 2011, Sunnybrook Research Institute
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Sunnybrook Research Institute nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Sunnybrook Research Institute BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************************/
#ifndef RT_CAMERA_CONTROL_H
#define RT_CAMERA_CONTROL_H

#include <QObject>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QPoint>
#include <QList>
#include <QTimer>

#include <vtkCamera.h>
#include <vtkMatrix4x4.h>
#include <vtkRenderer.h>

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
    \param renderer The renderer associated with this camera.
    \param eventWidget The QVTK widget where the rendering is performed. This widget also receives all of the mouse events that interact with the 3D scene.
    */
  rtCameraControl(vtkCamera* cam, vtkRenderer* renderer, customQVTKWidget* eventWidget);

  //! Check if the camera is moving at this moment.
  /*!
    It is important for the 3D renderer to know when the camera is moving. To increase the responsiveness of the camera, a reduced quality rendering can be shown while the camera is moving.
    \return true if the camera is currently in motion. False otherwise.
    */
  bool cameraMoving();

  //! Tell the camera control that the camera is in motion.
  inline void startCameraMotion() { m_cameraInMotion = true; }

  //! Tell the camera control that the camera is no longer in motion.
  inline void stopCameraMotion() { m_cameraInMotion = false; }

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

  //! Set the view angle for the camera.
  /*!
    \param angle The new view angle to set. Should be in the range (0, 180).
    */
  void setViewAngle(double angle);
  void setViewAngle(double elevation, double azimuth);

  //! Gt the view angle for the camera.
  /*!
    \return The view angle. Should be in the range (0, 180).
    */
  double getViewAngle();

public slots:
  //! Slot called when the mouse is pressed in the QVTK widget.
  void mousePress(QMouseEvent*,int);
  //! Slot is called when the mouse is moved. These events only occur when at least one of the mouse buttons is pressed down. (See QMouseEvent).
  void mouseMove(QMouseEvent*,int);
  //! Slot called when a mouse button is released.
  void mouseRelease(QMouseEvent*,int);
  //! Slot called when the mouse button is double clicked.
  void mouseDoubleClick(QMouseEvent*,int);
  //! Slot called when a key is pressed.
  void keyPress(QKeyEvent*,int);
  //! Slot called when a key is released.
  void keyRelease(QKeyEvent*,int);
  //! Slot called when the mouse wheel is moved.
  void wheel(QWheelEvent*,int);
  //! Slot is called when the wheel timer expires.
  void wheelFinished();

protected:
  //! The pointer to the vtk camera object in the main 3D view.
  vtkCamera* m_camera;

  //! The handle to the vtk renderer associated with this camera.
  vtkRenderer* m_renderer;

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
  //! True if the scroll wheel is moving.
  bool m_scrollWheelMotion;
  //! True if the camera is in motion.
  bool m_cameraInMotion;

  //! Since the mouse scroll does not have an 'up' and a 'down' the scroll is considered to have finished a certain time after the wheel motion has been received.
  QTimer m_scrollTimer;

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
