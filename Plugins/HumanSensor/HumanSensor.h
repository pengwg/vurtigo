#ifndef HUMANSENSOR_H
#define HUMANSENSOR_H

#include <QObject>
#include <QTimer>

#include <XnOpenNI.h>
#include <XnCppWrapper.h>
#include <XnHash.h>
#include <XnLog.h>

#include <XnVNite.h>

typedef enum
{
    IN_SESSION,
    NOT_IN_SESSION,
    QUICK_REFOCUS
} SessionState;

class HumanSensor : public QObject
{
    Q_OBJECT
public:
    HumanSensor();
    ~HumanSensor();

    XnStatus init();

    XnStatus check_rc(XnStatus &rc, const char *what);
    XnStatus check_errors(XnStatus &rc, xn::EnumerationErrors &errors, const char *what);

    void run();

signals:
    void viewAngleChanged(float, float);

private:
    static HumanSensor *instance;
    static void XN_CALLBACK_TYPE FocusProgress(const XnChar* strFocus, const XnPoint3D& ptPosition, XnFloat fProgress, void* UserCxt);
    static void XN_CALLBACK_TYPE SessionStarting(const XnPoint3D& ptPosition, void* UserCxt);
    static void XN_CALLBACK_TYPE SessionEnding(void* UserCxt);
    static void XN_CALLBACK_TYPE NoHands(void* UserCxt);
    static void XN_CALLBACK_TYPE Swipe(XnVDirection eDir, XnFloat fVelocity, XnFloat fAngle, void *pUserCxt);
    static void XN_CALLBACK_TYPE SliderValueChanged(XnFloat fValue, void *cxt);
    static void XN_CALLBACK_TYPE SliderValueChanged2D(XnFloat fXValue, XnFloat fYValue, void *cxt);

    static SessionState m_SessionState;

    xn::Context m_Context;
    xn::ScriptNode m_ScriptNode;
    xn::DepthGenerator m_DepthGenerator;
    xn::HandsGenerator m_HandsGenerator;
    xn::GestureGenerator m_GestureGenerator;

    XnVSessionManager* m_pSessionManager;

    XnVSelectableSlider2D* m_pSlider;

    static float m_elevation;
    static float m_azimuth;

    QTimer m_timer;

private slots:
    void update();

};

inline XnStatus HumanSensor::check_rc(XnStatus &rc, const char *what)
{
    if (rc != XN_STATUS_OK) {
        printf("%s failed: %s\n", what, xnGetStatusString(rc));
        return rc;
    }
}

inline XnStatus HumanSensor::check_errors(XnStatus &rc, xn::EnumerationErrors &errors, const char *what)
{
    if (rc == XN_STATUS_NO_NODE_PRESENT) {
        XnChar strError[1024];
        errors.ToString(strError, 1024);
        printf("%s\n", strError);
        return (rc);
    }
}

#endif // HUMANSENSOR_H
