#ifndef CAMERA_MOTION_PLUGIN_H
#define CAMERA_MOTION_PLUGIN_H

#include "rtPluginInterface.h"

// Qt includes
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>

//! Plugin to control certain movements of the camera.
class CameraMotionPlugin : public QObject, public DataInterface {

  // Need to use Q_OBJECT so that this class can use signals and slots.
  Q_OBJECT
  // This class implements the 'DataInterface'.
  Q_INTERFACES(DataInterface)

 public:
  //! This function is called when an object in the base is modified.
  /*!
    The plugin can register to have certain objects as part of a waatch list and be alerted when they change.
    \sa DataInterface::addObjectToWatchList()
   */
  void objectModified(int objID);

  //! Function called when the plugin is first created.
  /*!
    All of the plugin setup commands should go in here.
    */
  bool init();

  //! Function called just before the plugin is deleted.
  /*!
    All of the plugin cleanup work is done here.
    */
  void cleanup();

  //! Plugin can register to receive update calls every so often
  /*!
    Called by the base at regular time intervals.
    \sa DataInterface::setUpdateTime()
    */
  void update();

  //! Plugins can watch of clicks in the 3D window.
  /*!
    Plugin must register with the rtBaseHandle first.
    \sa rtBaseHandle::watchClick()
    */
  void point3DSelected(double px, double py, double pz, int intensity);

 public slots:

  //! Slot that is called when the view angle is changed with the slider.
  void viewAngleChanged(int);

 protected:

  //! Create a Simple Vurtigo test object.
  void createTestObject();

  //! Object ID for the small test object.
  int m_smallVol;

  ////////////
  // Gui Elements
  ////////////
  //! Layout for the plugin GUI
  QHBoxLayout *m_mainPluginGuiLayout;

  //! Generic Qt Label to inform the user about the purpose of the slider
  QLabel *m_viewAngleLabel;

  //! Qt Labal to show the acurrent view angle value
  QLabel *m_viewAngleAmtLabel;

  //! Slider so the user can adjust the view angle.
  QSlider *m_viewAngleSlider;
 private:

};

#endif
