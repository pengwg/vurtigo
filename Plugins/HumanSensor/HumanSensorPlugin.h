#ifndef HUMAN_SENSOR_PLUGIN_H
#define HUMAN_SENSOR_PLUGIN_H

#include "rtPluginInterface.h"

// Qt includes
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>

#include "HumanSensor.h"

class HumanSensorPlugin : public QObject, public DataInterface
{
    Q_OBJECT
    Q_INTERFACES(DataInterface)
public:
    void objectModified(int objID);
    bool init();
    void cleanup();
    void update();
    void point3DSelected(double px, double py, double pz, int intensity);

public slots:
    void viewAngleChanged(int);
    void setViewAngle(float azimuth, float elevation);

protected:
    void createTestObject();
    int m_smallVol;

    QVBoxLayout *m_mainPluginGuiLayout;

    QHBoxLayout *m_azimuthSliderLayout;
    QLabel *m_viewAngleLabelAzimuth;
    QLabel *m_viewAngleAmtLabelAzimuth;
    QSlider *m_viewAngleSliderAzimuth;

    QHBoxLayout *m_elevationSliderLayout;
    QLabel *m_viewAngleLabelElevation;
    QLabel *m_viewAngleAmtLabelElevation;
    QSlider *m_viewAngleSliderElevation;
private:
    HumanSensor m_humanSensor;

    float m_currElevation;
    float m_currAzimuth;
};

#endif
