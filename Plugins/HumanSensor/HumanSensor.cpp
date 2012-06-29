#include "HumanSensor.h"

void XN_CALLBACK_TYPE GestureIntermediateStageCompletedHandler(xn::GestureGenerator& generator, const XnChar* strGesture, const XnPoint3D* pPosition, void* pCookie)
{
    printf("Gesture %s: Intermediate stage complete (%f,%f,%f)\n", strGesture, pPosition->X, pPosition->Y, pPosition->Z);
}
void XN_CALLBACK_TYPE GestureReadyForNextIntermediateStageHandler(xn::GestureGenerator& generator, const XnChar* strGesture, const XnPoint3D* pPosition, void* pCookie)
{
    printf("Gesture %s: Ready for next intermediate stage (%f,%f,%f)\n", strGesture, pPosition->X, pPosition->Y, pPosition->Z);
}
void XN_CALLBACK_TYPE GestureProgressHandler(xn::GestureGenerator& generator, const XnChar* strGesture, const XnPoint3D* pPosition, XnFloat fProgress, void* pCookie)
{
    printf("Gesture %s progress: %f (%f,%f,%f)\n", strGesture, fProgress, pPosition->X, pPosition->Y, pPosition->Z);
}

#define CONFIG_PATH "/home/pwang/Dev/Kinect/Config.xml"

HumanSensor *HumanSensor::instance = NULL;
SessionState HumanSensor::m_SessionState = NOT_IN_SESSION;
float HumanSensor::m_elevation = 0;
float HumanSensor::m_azimuth = 0;

HumanSensor::HumanSensor()
{

}

HumanSensor::~HumanSensor()
{
    m_ScriptNode.Release();
    m_DepthGenerator.Release();
    m_HandsGenerator.Release();
    m_GestureGenerator.Release();
    m_Context.Release();
}

XnStatus HumanSensor::init()
{
    instance = this;
    xn::EnumerationErrors errors;

    XnStatus rc = m_Context.InitFromXmlFile(CONFIG_PATH, m_ScriptNode, &errors);
    check_errors(rc, errors, "InitFromXmlFile");
    check_rc(rc, "InitFromXmlFile");

    if (rc != XN_STATUS_OK) {
        return rc;
    }

    rc = m_Context.FindExistingNode(XN_NODE_TYPE_DEPTH, m_DepthGenerator);
    check_rc(rc, "Find depth generator");
    rc = m_Context.FindExistingNode(XN_NODE_TYPE_HANDS, m_HandsGenerator);
    check_rc(rc, "Find hands generator");
    rc = m_Context.FindExistingNode(XN_NODE_TYPE_GESTURE, m_GestureGenerator);
    check_rc(rc, "Find gesture generator");

    m_pSessionManager = new XnVSessionManager;
    rc = m_pSessionManager->Initialize(&m_Context, "Click,Wave", "RaiseHand");
    check_rc(rc, "SessionManager::Initialize");

    XnCallbackHandle hGestureIntermediateStageCompleted, hGestureProgress, hGestureReadyForNextIntermediateStage;
    m_GestureGenerator.RegisterToGestureIntermediateStageCompleted(GestureIntermediateStageCompletedHandler, NULL, hGestureIntermediateStageCompleted);
    m_GestureGenerator.RegisterToGestureReadyForNextIntermediateStage(GestureReadyForNextIntermediateStageHandler, NULL, hGestureReadyForNextIntermediateStage);
    m_GestureGenerator.RegisterGestureCallbacks(NULL, GestureProgressHandler, NULL, hGestureProgress);

    m_pSessionManager->RegisterSession(NULL, SessionStarting, SessionEnding, FocusProgress);

    m_pSlider = new XnVSelectableSlider2D(1, 1);
    m_pSlider->RegisterValueChange(NULL, SliderValueChanged2D);

    m_pSessionManager->AddListener(m_pSlider);

    rc = m_Context.StartGeneratingAll();
    check_rc(rc, "StartGenerating");

    return rc;
}

void HumanSensor::run()
{
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(update()));
    m_timer.start(5);
}

void HumanSensor::update()
{
    m_Context.WaitOneUpdateAll(m_DepthGenerator);
    m_pSessionManager->Update(&m_Context);

    if (instance)
        emit instance->viewAngleChanged(m_azimuth, m_elevation);
}

void XN_CALLBACK_TYPE HumanSensor::Swipe(XnVDirection eDir, XnFloat fVelocity, XnFloat fAngle, void *pUserCxt)
{
    switch (eDir) {
    case DIRECTION_LEFT :
        printf("Swipe left\n");
        break;
    case DIRECTION_RIGHT:
        printf("Swipe right\n");
        break;
    }
}

void XN_CALLBACK_TYPE HumanSensor::SliderValueChanged(XnFloat fValue, void *cxt)
{
    printf("Slider value: %f\n", fValue);

    m_azimuth = fValue * 360 - 180;
}

void XN_CALLBACK_TYPE HumanSensor::SliderValueChanged2D(XnFloat fXValue, XnFloat fYValue, void *cxt)
{
    printf("Slider value: %f -- %f\n", fXValue, fYValue);

    m_azimuth = fXValue * 360 - 180;
    m_elevation = fYValue * 180 - 90;
}

void XN_CALLBACK_TYPE HumanSensor::FocusProgress(const XnChar* strFocus, const XnPoint3D& ptPosition, XnFloat fProgress, void* UserCxt)
{
//	printf("Focus progress: %s @(%f,%f,%f): %f\n", strFocus, ptPosition.X, ptPosition.Y, ptPosition.Z, fProgress);
}

void XN_CALLBACK_TYPE HumanSensor::SessionStarting(const XnPoint3D& ptPosition, void* UserCxt)
{
    printf("Session start: (%f,%f,%f)\n", ptPosition.X, ptPosition.Y, ptPosition.Z);
    m_SessionState = IN_SESSION;
}

void XN_CALLBACK_TYPE HumanSensor::SessionEnding(void* UserCxt)
{
    printf("Session end\n");
    m_SessionState = NOT_IN_SESSION;
}

void XN_CALLBACK_TYPE HumanSensor::NoHands(void* UserCxt)
{
    if (m_SessionState != NOT_IN_SESSION)
    {
        printf("Quick refocus\n");
        m_SessionState = QUICK_REFOCUS;
    }
}