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

    m_viewAngleLabelElevation = new QLabel();
    m_viewAngleAmtLabelElevation = new QLabel();
    m_viewAngleSliderElevation = new QSlider();

    m_elevationSliderLayout = new QHBoxLayout;
    m_elevationSliderLayout->addWidget(m_viewAngleLabelElevation);
    m_elevationSliderLayout->addWidget(m_viewAngleSliderElevation);
    m_elevationSliderLayout->addWidget(m_viewAngleAmtLabelElevation);

    m_viewAngleLabelElevation->setText("View Angle Elevation: ");
    m_viewAngleAmtLabelElevation->setText("0");
    m_viewAngleSliderElevation->setMinimum(-90);
    m_viewAngleSliderElevation->setMaximum(90);
    m_viewAngleSliderElevation->setValue(0);
    m_viewAngleSliderElevation->setOrientation(Qt::Vertical);
    m_viewAngleSliderElevation->setMinimumSize(180, 20);

    m_viewAngleLabelAzimuth = new QLabel();
    m_viewAngleAmtLabelAzimuth = new QLabel();
    m_viewAngleSliderAzimuth = new QSlider();

    m_azimuthSliderLayout = new QHBoxLayout;
    m_azimuthSliderLayout->addWidget(m_viewAngleLabelAzimuth);
    m_azimuthSliderLayout->addWidget(m_viewAngleSliderAzimuth);
    m_azimuthSliderLayout->addWidget(m_viewAngleAmtLabelAzimuth);

    m_viewAngleLabelAzimuth->setText("View Angle Azimuth: ");
    m_viewAngleAmtLabelAzimuth->setText("0");
    m_viewAngleSliderAzimuth->setMinimum(-180);
    m_viewAngleSliderAzimuth->setMaximum(180);
    m_viewAngleSliderAzimuth->setValue(0);
    m_viewAngleSliderAzimuth->setOrientation(Qt::Horizontal);
    m_viewAngleSliderAzimuth->setMinimumSize(180, 20);

    m_mainPluginGuiLayout = new QVBoxLayout();
    m_mainPluginGuiLayout->addLayout(m_elevationSliderLayout);
    m_mainPluginGuiLayout->addLayout(m_azimuthSliderLayout);

    m_mainWidget->setLayout(m_mainPluginGuiLayout);
    // connect(m_viewAngleSlider, SIGNAL(valueChanged(int)), this, SLOT(viewAngleChanged(int)));

    createTestObject();
    setUpdateTime(-1);

    XnStatus rc = m_humanSensor.init();
    if (rc != XN_STATUS_OK)
        return false;

    m_currElevation = 0;
    m_currAzimuth = 0;
    connect(&m_humanSensor, SIGNAL(viewAngleChanged(float, float)), this, SLOT(setViewAngle(float, float)));
    m_humanSensor.run();
    return true;
}

void HumanSensorPlugin::cleanup()
{
    // Note that there is no cleanup of widgets here.
    // In Qt when a widget is added to a layout or a layout is added to a widget the parent takes ownership of the child and is will delete it.
}

void HumanSensorPlugin::update()
{

}

void HumanSensorPlugin::point3DSelected(double px, double py, double pz, int intensity) {
    // Plugin does not watch for point selection in the 3D view.
}

void HumanSensorPlugin::viewAngleChanged(int angle)
{

}

void HumanSensorPlugin::setViewAngle(float azimuth, float elevation)
{
    m_viewAngleSliderElevation->setValue(static_cast<int>(elevation));
    m_viewAngleAmtLabelElevation->setText(QString::number(static_cast<int>(elevation)));

    m_viewAngleSliderAzimuth->setValue(static_cast<int>(azimuth));
    m_viewAngleAmtLabelAzimuth->setText(QString::number(static_cast<int>(azimuth)));

    rtCameraControl *camera;
    camera = rtApplication::instance().getMainWinHandle()->getCameraControl();

    /*double dElevation = static_cast<double>(elevation - m_currElevation);
    double dAzimuth = static_cast<double>(azimuth - m_currAzimuth);
    camera->changeViewAngle(-dElevation, -dAzimuth);

    m_currElevation = elevation;
    m_currAzimuth = azimuth;*/

    double x, y, z;
    camera->getDefaultPosition(x, y, z);

    double R = sqrt(x * x + y * y + z * z);

    double newAzimuth = atan2(z, x) - azimuth / 180 * M_PI;
    double newElevation = asin(y / R) + elevation / 180 * M_PI;
    double r = R * cos(newElevation);
    camera->setPosition(r * cos(newAzimuth), R * sin(newElevation), r * sin(newAzimuth));
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
