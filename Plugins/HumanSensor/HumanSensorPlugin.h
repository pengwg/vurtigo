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
    void setViewAngle(float angle);

protected:
    void createTestObject();
    int m_smallVol;

    HumanSensor m_humanSensor;
    ////////////
    // Gui Elements
    ////////////
    QHBoxLayout *m_mainPluginGuiLayout;
    QLabel *m_viewAngleLabel;
    QLabel *m_viewAngleAmtLabel;
    QSlider *m_viewAngleSlider;
private:

};

#endif
