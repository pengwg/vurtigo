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
#include "CameraMotionPlugin.h"
#include "rtApplication.h"
#include "rtMainWindow.h"
#include "rtCameraControl.h"
#include "rtBaseHandle.h"

#include "rt3dVolumeDataObject.h"

#include <vtkImageSinusoidSource.h>

void CameraMotionPlugin::objectModified(int objID) {
  // No objects are watched by this plugin.
}

bool CameraMotionPlugin::init() {

  // First create the GUI
  // Create the main widget (From DataInterface).
  // This parameter is NULL if the plugin does not use a widget.
  m_mainWidget = new QWidget();

  // Create the layout.
  m_mainPluginGuiLayout = new QHBoxLayout();

  // Create the widgets to go into the layout.
  m_viewAngleLabel = new QLabel();
  m_viewAngleAmtLabel = new QLabel();
  m_viewAngleSlider = new QSlider();

  // Add the widgets to the layout
  m_mainPluginGuiLayout->addWidget(m_viewAngleLabel);
  m_mainPluginGuiLayout->addWidget(m_viewAngleSlider);
  m_mainPluginGuiLayout->addWidget(m_viewAngleAmtLabel);

  // Set the propeties for those widgets.
  m_viewAngleLabel->setText("View Angle: ");
  m_viewAngleAmtLabel->setText("30");
  m_viewAngleSlider->setMinimum(1);
  m_viewAngleSlider->setMaximum(179);
  m_viewAngleSlider->setValue(30);
  m_viewAngleSlider->setOrientation(Qt::Horizontal);

  // Make the horizontal at least 180 pixels.
  m_viewAngleSlider->setMinimumSize(180, 20);

  // Set the layout containing the custom widgets.
  m_mainWidget->setLayout(m_mainPluginGuiLayout);

  // Connect the signal to the slot.
  connect(m_viewAngleSlider, SIGNAL(valueChanged(int)), this, SLOT(viewAngleChanged(int)));

  // Create a simple Vurtigo test object.
  createTestObject();

  // Ask the base to update this plugin every 500 msec
  setUpdateTime(500);

  // Return true to show that the plugin has been created as required.
  return true;
}

void CameraMotionPlugin::cleanup() {
  // Note that there is no cleanup of widgets here.
  // In Qt when a widget is added to a layout or a layout is added to a widget the parent takes ownership of the child and is will delete it.
}

void CameraMotionPlugin::update(){
  // This function will be called as specified by setUpdateTime().

  // Check that the object was actually created.
  // If an object creation fails then the id is negative.
  if (m_smallVol >= 0) {

    // Get the object with the unique ID
    rt3DVolumeDataObject* ptObj = static_cast<rt3DVolumeDataObject*>(rtBaseHandle::instance().getObjectWithID(m_smallVol));

    // Check that the object exists.
    if (ptObj) {

      // Lock the object before doing modifications
      ptObj->lock();

      // Apply a rotation to the data
      ptObj->getTransform()->RotateX(5);

      // Reset the modified time.
      ptObj->Modified();

      // Unlock the object after modifications are finished.
      ptObj->unlock();

    }
  }
}

void CameraMotionPlugin::point3DSelected(double px, double py, double pz, int intensity) {
  // Plugin does not watch for point selection in the 3D view.
}

void CameraMotionPlugin::viewAngleChanged(int angle) {

  // Change the text in the GUI to inform the user that a change has taken place.
  m_viewAngleAmtLabel->setText(QString::number(angle));

  // Get a pointer to Vurtigo's camera control
  rtCameraControl *camera;
  camera = rtApplication::instance().getMainWinHandle()->getCameraControl();

  // Give the camera control the new view angle.
  camera->setViewAngle(static_cast<double>(angle));
}


void CameraMotionPlugin::createTestObject() {
  // Ask the base handle (Vurtigo) to create a rtConstants::OT_3DObject type object.
  // This function will return the unique ID for the new object.
  m_smallVol = rtBaseHandle::instance().requestNewObject(rtConstants::OT_3DObject, "Small Test Volume");

  // Check that the object was actually created.
  // If an object creation fails then the id is negative.
  if (m_smallVol >= 0) {

    // Get the object with the unique ID
    rt3DVolumeDataObject* ptObj = static_cast<rt3DVolumeDataObject*>(rtBaseHandle::instance().getObjectWithID(m_smallVol));

    // Check that the object exists.
    if (ptObj) {

      // Create a source of input data.
      vtkImageSinusoidSource* sinSrc = vtkImageSinusoidSource::New();
      sinSrc->SetWholeExtent(1,32, 1, 32, 1, 32);
      sinSrc->SetDirection(1, 2, 3);
      sinSrc->SetPeriod(5);
      sinSrc->SetPhase(1);
      sinSrc->SetAmplitude(15);
      sinSrc->Update();

      // Lock the object before doing modifications
      ptObj->lock();


      // Copy the image data over.
      ptObj->copyNewImageData(sinSrc->GetOutput());
      // Apply a translation to the data
      ptObj->translateData(10, 20, 2);
      // Reset the modified time.
      ptObj->Modified();

      // Unlock the object after modifications are finished.
      ptObj->unlock();

      // Delete the source as it has already been copied.
      sinSrc->Delete();
    }
  }

}

// This last line will export the plugin. 
Q_EXPORT_PLUGIN2(CameraMotion, CameraMotionPlugin)
