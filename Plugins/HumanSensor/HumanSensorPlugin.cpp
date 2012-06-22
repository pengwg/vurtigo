#include "HumanSensorPlugin.h"
#include "rtApplication.h"
#include "rtMainWindow.h"
#include "rtCameraControl.h"
#include "rtBaseHandle.h"

#include "rt2dSliceDataObject.h"

#include <vtkImageGaussianSource.h>

void HumanSensorPlugin::objectModified(int objID)
{
    // No objects are watched by this plugin.
}

bool HumanSensorPlugin::init()
{

    m_mainWidget = new QWidget();

    m_mainPluginGuiLayout = new QHBoxLayout();

    m_viewAngleLabel = new QLabel();
    m_viewAngleAmtLabel = new QLabel();
    m_viewAngleSlider = new QSlider();

    m_mainPluginGuiLayout->addWidget(m_viewAngleLabel);
    m_mainPluginGuiLayout->addWidget(m_viewAngleSlider);
    m_mainPluginGuiLayout->addWidget(m_viewAngleAmtLabel);

    m_viewAngleLabel->setText("View Angle: ");
    m_viewAngleAmtLabel->setText("0");
    m_viewAngleSlider->setMinimum(0);
    m_viewAngleSlider->setMaximum(100);
    m_viewAngleSlider->setValue(50);
    m_viewAngleSlider->setOrientation(Qt::Horizontal);

    m_viewAngleSlider->setMinimumSize(180, 20);

    m_mainWidget->setLayout(m_mainPluginGuiLayout);

    // connect(m_viewAngleSlider, SIGNAL(valueChanged(int)), this, SLOT(viewAngleChanged(int)));
    connect(&m_humanSensor, SIGNAL(viewAngleChanged(float)), this, SLOT(setViewAngle(float)));

    createTestObject();


    setUpdateTime(10);

    XnStatus rc = m_humanSensor.init();
    if (rc != XN_STATUS_OK)
        return false;
    else
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
    camera->changeViewAngle(0, static_cast<double>(angle - 50) / 50);

}

void HumanSensorPlugin::setViewAngle(float fvalue)
{
    m_viewAngleSlider->setValue(static_cast<int>(fvalue * 100));
    viewAngleChanged(static_cast<int>(fvalue * 100));
}

void HumanSensorPlugin::createTestObject()
{
    m_smallVol = rtBaseHandle::instance().requestNewObject(rtConstants::OT_2DObject, "Small Test Volume");

    if (m_smallVol >= 0) {
        rt2DSliceDataObject* ptObj = static_cast<rt2DSliceDataObject*>(rtBaseHandle::instance().getObjectWithID(m_smallVol));

        if (ptObj) {

            vtkImageGaussianSource* sinSrc = vtkImageGaussianSource::New();
            sinSrc->SetWholeExtent(1,32, 1, 32, 0, 0);
            sinSrc->Update();

            ptObj->lock();

            ptObj->copyImageData2D(sinSrc->GetOutput());
            ptObj->translateTo(10, 20, 2);
            ptObj->Modified();

            ptObj->unlock();
            sinSrc->Delete();
        }
    }

}

// This last line will export the plugin. 
Q_EXPORT_PLUGIN2(HumanSensor, HumanSensorPlugin)
