#include <QApplication>
#include <iostream> 
#include "rtMainWindow.h"
#include "rtObjectManager.h"
#include "rtLabelRenderObject.h"
#include "rtTimeManager.h"
#include "rtPluginLoader.h"

#include "rt3dPointBufferDataObject.h"


//! Just a function to load some test datasets. 
void loadTestData() {
  rtRenderObject* label;
  rtRenderObject* label2;
  rtRenderObject* pointList;
  rtRenderObject* noneObj;

  rt3DPointBufferDataObject::SimplePoint p1, p2, p3, p4;

  p1.px = 0.0;
  p1.py = 0.0;
  p1.pz = 0.0;
  p1.pSize = 0.25;
  p1.pProp->SetColor(1, 0, 0); //Red
  p1.pProp->SetOpacity(0.2);

  p2.px = 1.0;
  p2.py = 0.0;
  p2.pz = 0.0;
  p2.pSize = 0.25;
  p2.pProp->SetColor(1, 0, 0);

  p3.px = 0.0;
  p3.py = 1.0;
  p3.pz = 0.0;
  p3.pSize = 0.25;
  p3.pProp->SetColor(0, 1, 0);

  p4.px = 0.0;
  p4.py = 0.0;
  p4.pz = 1.0;
  p4.pSize = 0.25;
  p4.pProp->SetColor(0, 0, 1);

  label = rtObjectManager::instance().addObjectOfType(rtConstants::OT_TextLabel, "MyLabel");
  label2 = rtObjectManager::instance().addObjectOfType(rtConstants::OT_TextLabel, "MyLabel2");
  pointList = rtObjectManager::instance().addObjectOfType(rtConstants::OT_3DPointBuffer, "TestPointBuffer");

  static_cast<rt3DPointBufferDataObject*>(pointList->getDataObject())->addPoint(p1);
  static_cast<rt3DPointBufferDataObject*>(pointList->getDataObject())->addPoint(p2);
  static_cast<rt3DPointBufferDataObject*>(pointList->getDataObject())->addPoint(p3);
  static_cast<rt3DPointBufferDataObject*>(pointList->getDataObject())->addPoint(p4);
  pointList->update();

  // This call creates several warnings. It is only for a test. None objects should not be created. 
  noneObj = rtObjectManager::instance().addObjectOfType(rtConstants::OT_None, "None Object");
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    rtMainWindow mainWin;
    
    rtTimeManager::instance().startRenderTimer(&mainWin, 40);
    rtObjectManager::instance().setMainWinHandle(&mainWin);

    loadTestData();

    mainWin.show();
    return app.exec();
}
