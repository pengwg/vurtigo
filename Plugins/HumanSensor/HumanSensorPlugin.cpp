#include "HumanSensorPlugin.h"
#include "rtApplication.h"
#include "rtMainWindow.h"
#include "rtCameraControl.h"
#include "rtBaseHandle.h"

#include "rt2dSliceDataObject.h"

#include <vtkImageGaussianSource.h>

void HumanSensorPlugin::objectModified(int objID) {
    // No objects are watched by this plugin.
}

bool HumanSensorPlugin::init() {

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
    m_viewAngleAmtLabel->setText("0");
    m_viewAngleSlider->setMinimum(0);
    m_viewAngleSlider->setMaximum(100);
    m_viewAngleSlider->setValue(50);
    m_viewAngleSlider->setOrientation(Qt::Horizontal);

    // Make the horizontal at least 180 pixels.
    m_viewAngleSlider->setMinimumSize(180, 20);

    // Set the layout containing the custom widgets.
    m_mainWidget->setLayout(m_mainPluginGuiLayout);

    // Connect the signal to the slot.
    // connect(m_viewAngleSlider, SIGNAL(valueChanged(int)), this, SLOT(viewAngleChanged(int)));
    connect(&m_humanSensor, SIGNAL(viewAngleChanged(float)), this, SLOT(setViewAngle(float)));
    // Create a simple Vurtigo test object.
    createTestObject();

    // Ask the base to update this plugin every 50 msec
    setUpdateTime(10);

    // Return true to show that the plugin has been created as required.
    return true;
}

void HumanSensorPlugin::cleanup()
{
    // Note that there is no cleanup of widgets here.
    // In Qt when a widget is added to a layout or a layout is added to a widget the parent takes ownership of the child and is will delete it.
}

void HumanSensorPlugin::update()
{
    m_humanSensor.run();
}

void HumanSensorPlugin::point3DSelected(double px, double py, double pz, int intensity) {
    // Plugin does not watch for point selection in the 3D view.
}

void HumanSensorPlugin::viewAngleChanged(int angle)
{

    // Change the text in the GUI to inform the user that a change has taken place.
    m_viewAngleAmtLabel->setText(QString::number(angle - 50));

    // Get a pointer to Vurtigo's camera control
    rtCameraControl *camera;
    camera = rtApplication::instance().getMainWinHandle()->getCameraControl();

    // Give the camera control the new view angle.
    camera->setViewAngle(0, static_cast<double>(angle - 50) / 50);

}

void HumanSensorPlugin::setViewAngle(float fvalue)
{
    m_viewAngleSlider->setValue(static_cast<int>(fvalue * 100));
    viewAngleChanged(static_cast<int>(fvalue * 100));
}

void HumanSensorPlugin::createTestObject() {
    // Ask the base handle (Vurtigo) to create a rtConstants::OT_3DObject type object.
    // This function will return the unique ID for the new object.
    m_smallVol = rtBaseHandle::instance().requestNewObject(rtConstants::OT_2DObject, "Small Test Volume");

    // Check that the object was actually created.
    // If an object creation fails then the id is negative.
    if (m_smallVol >= 0) {

        // Get the object with the unique ID
        rt2DSliceDataObject* ptObj = static_cast<rt2DSliceDataObject*>(rtBaseHandle::instance().getObjectWithID(m_smallVol));

        // Check that the object exists.
        if (ptObj) {

            // Create a source of input data.
            vtkImageGaussianSource* sinSrc = vtkImageGaussianSource::New();
            sinSrc->SetWholeExtent(1,32, 1, 32, 0, 0);
            sinSrc->Update();

            // Lock the object before doing modifications
            ptObj->lock();


            // Copy the image data over.
            ptObj->copyImageData2D(sinSrc->GetOutput());
            // Apply a translation to the data
            ptObj->translateTo(10, 20, 2);
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
Q_EXPORT_PLUGIN2(HumanSensor, HumanSensorPlugin)
