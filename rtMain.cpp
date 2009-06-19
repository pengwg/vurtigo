#include <QApplication>
#include <iostream> 
#include "rtMainWindow.h"
#include "rtObjectManager.h"
#include "rtLabelRenderObject.h"
#include "rtTimeManager.h"
#include "rtPluginLoader.h"

#include "rt3dPointBufferDataObject.h"
#include "rtCathDataObject.h"
#include "rt3dVolumeDataObject.h"
#include "rt2dSliceDataObject.h"

#include "vtkImageSinusoidSource.h"

//! Just a function to load some test datasets. 
void loadTestData() {
  rtRenderObject* label;
  rtRenderObject* label2;
  rtRenderObject* pointList;
  rtRenderObject* noneObj;
  rtRenderObject* cath;
  rtRenderObject* cath2;
  rtRenderObject* cath4;
  rtRenderObject* vol3D;
  rtRenderObject* bigVol3D;

  rt3DPointBufferDataObject::SimplePoint p1, p2, p3, p4;

  qDebug("Loading Test Data...");

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

  int ix1=0, ix2=0;
  rt3DPointBufferDataObject::SimplePoint pp[10][10];

  label = rtObjectManager::instance().addObjectOfType(rtConstants::OT_TextLabel, "MyLabel");
  label2 = rtObjectManager::instance().addObjectOfType(rtConstants::OT_TextLabel, "MyLabel2");
  pointList = rtObjectManager::instance().addObjectOfType(rtConstants::OT_3DPointBuffer, "TestPointBuffer");

  static_cast<rt3DPointBufferDataObject*>(pointList->getDataObject())->addPoint(p1);
  static_cast<rt3DPointBufferDataObject*>(pointList->getDataObject())->addPoint(p2);
  static_cast<rt3DPointBufferDataObject*>(pointList->getDataObject())->addPoint(p3);
  static_cast<rt3DPointBufferDataObject*>(pointList->getDataObject())->addPoint(p4);

  // Add 100 semi-transparent spheres.
  for (ix1=2; ix1<12; ix1++) {
    for (ix2=2; ix2<12; ix2++) {
      pp[ix1-2][ix2-2].px = ix1;
      pp[ix1-2][ix2-2].py = ix2;
      pp[ix1-2][ix2-2].pz = 2;
      pp[ix1-2][ix2-2].pSize = 0.25;
      pp[ix1-2][ix2-2].pProp->SetColor(0,1,0);
      pp[ix1-2][ix2-2].pProp->SetOpacity(0.2);
      static_cast<rt3DPointBufferDataObject*>(pointList->getDataObject())->addPoint(pp[ix1-2][ix2-2]);
    }
  }

  pointList->update();

  cath = rtObjectManager::instance().addObjectOfType(rtConstants::OT_Cath, "Cath Object 1 Coil");
  cath2 = rtObjectManager::instance().addObjectOfType(rtConstants::OT_Cath, "Cath Object 2 Coil");
  cath4 = rtObjectManager::instance().addObjectOfType(rtConstants::OT_Cath, "Cath Object 4 Coil");

  int c11 = static_cast<rtCathDataObject*>(cath->getDataObject())->addCoil(0);
  static_cast<rtCathDataObject*>(cath->getDataObject())->setCoilCoords(c11, 1.5, 1.5, 1.5);
  static_cast<rtCathDataObject*>(cath->getDataObject())->setCoilSNR(c11, 50);
  static_cast<rtCathDataObject*>(cath->getDataObject())->Modified();

  int c21 = static_cast<rtCathDataObject*>(cath2->getDataObject())->addCoil(0);
  int c22 = static_cast<rtCathDataObject*>(cath2->getDataObject())->addCoil(1);
  static_cast<rtCathDataObject*>(cath2->getDataObject())->setCoilCoords(c21, 1.5, 1.5, 1.5);
  static_cast<rtCathDataObject*>(cath2->getDataObject())->setCoilSNR(c21, 50);
  static_cast<rtCathDataObject*>(cath2->getDataObject())->setCoilCoords(c22, 3.5, 2.5, 1.5);
  static_cast<rtCathDataObject*>(cath2->getDataObject())->setCoilSNR(c22, 25);
  static_cast<rtCathDataObject*>(cath2->getDataObject())->Modified();

  int c41 = static_cast<rtCathDataObject*>(cath4->getDataObject())->addCoil(0);
  int c42 = static_cast<rtCathDataObject*>(cath4->getDataObject())->addCoil(0);
  int c43 = static_cast<rtCathDataObject*>(cath4->getDataObject())->addCoil(1);
  int c44 = static_cast<rtCathDataObject*>(cath4->getDataObject())->addCoil(2);

  static_cast<rtCathDataObject*>(cath4->getDataObject())->setCoilCoords(c41, 1.5, 1.5, 1.5);
  static_cast<rtCathDataObject*>(cath4->getDataObject())->setCoilSNR(c41, 50);
  static_cast<rtCathDataObject*>(cath4->getDataObject())->setCoilCoords(c42, 1.6, 1.4, 1.3);
  static_cast<rtCathDataObject*>(cath4->getDataObject())->setCoilSNR(c42, 25);
  static_cast<rtCathDataObject*>(cath4->getDataObject())->setCoilCoords(c43, 3.5, 2.5, 1.5);
  static_cast<rtCathDataObject*>(cath4->getDataObject())->setCoilSNR(c43, 40);
  static_cast<rtCathDataObject*>(cath4->getDataObject())->setCoilCoords(c44, 4.5, 2.0, 2.0);
  static_cast<rtCathDataObject*>(cath4->getDataObject())->setCoilSNR(c44, 40);
  static_cast<rtCathDataObject*>(cath4->getDataObject())->Modified();

  vol3D = rtObjectManager::instance().addObjectOfType(rtConstants::OT_3DObject, "3D Volume 256 by 256 by 256");
  rt3DVolumeDataObject* vol3DData = static_cast<rt3DVolumeDataObject*>( vol3D->getDataObject() );
  vtkImageSinusoidSource* sinSrc = vtkImageSinusoidSource::New();

  sinSrc->SetWholeExtent(1,256, 1, 256, 1, 256);
  sinSrc->SetDirection(1, 2, 3);
  sinSrc->SetPeriod(30);
  sinSrc->SetPhase(1);
  sinSrc->SetAmplitude(10);
  sinSrc->Update();

  vol3DData->copyNewImageData(sinSrc->GetOutput());
  vol3DData->translateData(200, 20, 2);
  vol3DData->getTransform()->RotateX(25); // Rotate around X 25 degrees
  sinSrc->Delete();

  bigVol3D = rtObjectManager::instance().addObjectOfType(rtConstants::OT_3DObject, "3D Volume 400 by 400 by 400");
  rt3DVolumeDataObject* vol3DData2 = static_cast<rt3DVolumeDataObject*>( bigVol3D->getDataObject() );
  vtkImageSinusoidSource* sinSrc2 = vtkImageSinusoidSource::New();

  sinSrc2->SetWholeExtent(1,400, 1, 400, 1, 400);
  sinSrc2->SetDirection(1, 2, 3);
  sinSrc2->SetPeriod(50);
  sinSrc2->SetPhase(1);
  sinSrc2->SetAmplitude(10);
  sinSrc2->Update();

  vol3DData2->copyNewImageData(sinSrc2->GetOutput());

  sinSrc2->Delete();

  vtkImageSinusoidSource* sinSrc3 = vtkImageSinusoidSource::New();
  sinSrc3->SetWholeExtent(1,256, 1, 256, 1, 1);
  sinSrc3->SetDirection(1, 2, 3);
  sinSrc3->SetPeriod(30);
  sinSrc3->SetPhase(1);
  sinSrc3->SetAmplitude(10);
  sinSrc3->Update();

  rtRenderObject* test2DPlane = rtObjectManager::instance().addObjectOfType(rtConstants::OT_2DObject, "2D Image 256 by 256" );
  rt2DSliceDataObject* slice = static_cast<rt2DSliceDataObject*>( test2DPlane->getDataObject() );
  slice->getTransform()->Translate(20, 200, 2);
  slice->getTransform()->RotateX(25);
  slice->getTransform()->RotateY(25);
  slice->getTransform()->RotateZ(25);
  slice->copyImageData2D(sinSrc3->GetOutput());

  sinSrc3->Delete();

}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    rtMainWindow mainWin;
    
    rtTimeManager::instance().startRenderTimer(&mainWin, 40);
    rtObjectManager::instance().setMainWinHandle(&mainWin);

    loadTestData();

    int widID[2];
    widID[0] = mainWin.createNew2DWidget();
    widID[1] = mainWin.createNew2DWidget();

    mainWin.show();
    return app.exec();
}
