#include "TestSuiteBasic.h"
#include "rtBaseHandle.h"
#include "objTypes.h"
#include "rtLabelRenderObject.h"
#include "rt3dPointBufferDataObject.h"
#include "rtCathDataObject.h"
#include "rt3dVolumeDataObject.h"
#include "rt2dSliceDataObject.h"
#include "rtMatrixDataObject.h"

#include "vtkImageSinusoidSource.h"

TestSuiteBasic::TestSuiteBasic() {
}

void TestSuiteBasic::run() {
  emit sendOutput("--------- Basic Test ----------");
  basicTestCreateObjects();

  if (m_3DPoints >= 0) {
    rt3DPointBufferDataObject* ptObj = static_cast<rt3DPointBufferDataObject*>(rtBaseHandle::instance().getObjectWithID(m_3DPoints));

    if (!ptObj) {
      emit sendOutput("Could Not Get 3D Sphere Object! FAIL!");
    } else {

      emit sendOutput("Setting Data for 3D Spheres...");
      rt3DPointBufferDataObject::SimplePoint pp[10][10];
      rt3DPointBufferDataObject::SimplePoint p[4];

      p[0].px = 0.0;
      p[0].py = 0.0;
      p[0].pz = 0.0;
      p[0].pSize = 0.25;
      p[0].pProp->SetColor(1, 0, 0); //Red
      p[0].pProp->SetOpacity(0.2);

      p[1].px = 1.0;
      p[1].py = 0.0;
      p[1].pz = 0.0;
      p[1].pSize = 0.25;
      p[1].pProp->SetColor(1, 0, 0);

      p[2].px = 0.0;
      p[2].py = 1.0;
      p[2].pz = 0.0;
      p[2].pSize = 0.25;
      p[2].pProp->SetColor(0, 1, 0);

      p[3].px = 0.0;
      p[3].py = 0.0;
      p[3].pz = 1.0;
      p[3].pSize = 0.25;
      p[3].pProp->SetColor(0, 0, 1);

      for (int ix1=0; ix1<4; ix1++) {
        ptObj->addPoint(p[ix1]);
      }

      // Add 100 semi-transparent spheres.
      for (int ix1=2; ix1<12; ix1++) {
        for (int ix2=2; ix2<12; ix2++) {
          pp[ix1-2][ix2-2].px = ix1;
          pp[ix1-2][ix2-2].py = ix2;
          pp[ix1-2][ix2-2].pz = 2;
          pp[ix1-2][ix2-2].pSize = 0.25;
          pp[ix1-2][ix2-2].pProp->SetColor(0,1,0);
          pp[ix1-2][ix2-2].pProp->SetOpacity(0.2);
          ptObj->addPoint(pp[ix1-2][ix2-2]);
        }
      }
      ptObj->Modified();
      // Have to force an update for the 3D checkbox to appear.
      rtBaseHandle::instance().forceRenderUpdate(m_3DPoints);
    }
  }


  if (m_cath[0]) {
    rtCathDataObject* ptObj = static_cast<rtCathDataObject*>(rtBaseHandle::instance().getObjectWithID(m_cath[0]));
    if (!ptObj) {
      emit sendOutput("Could Not Get 1 Coil Catheter Object! FAIL!");
    } else {
      emit sendOutput("Setting Cath Data...");
      int c1 = ptObj->addCoil(0);
      ptObj->setCoilCoords(c1, 1.5, 1.5, 1.5);
      ptObj->setCoilSNR(c1, 50);
      ptObj->Modified();
    }
  }

  if (m_cath[1]) {
    rtCathDataObject* ptObj = static_cast<rtCathDataObject*>(rtBaseHandle::instance().getObjectWithID(m_cath[1]));
    if (!ptObj) {
      emit sendOutput("Could Not Get 2 Coil Catheter Object! FAIL!");
    } else {
      emit sendOutput("Setting Cath Data...");
      int c1 = ptObj->addCoil(0);
      int c2 = ptObj->addCoil(1);
      ptObj->setCoilCoords(c1, 1.5, 1.5, 1.5);
      ptObj->setCoilSNR(c1, 50);
      ptObj->setCoilCoords(c2, 3.5, 2.5, 1.5);
      ptObj->setCoilSNR(c2, 25);
      ptObj->Modified();
    }
  }

  if (m_cath[2]) {
    rtCathDataObject* ptObj = static_cast<rtCathDataObject*>(rtBaseHandle::instance().getObjectWithID(m_cath[2]));
    if (!ptObj) {
      emit sendOutput("Could Not Get 5 Coil Catheter Object! FAIL!");
    } else {
      emit sendOutput("Setting Cath Data...");
      int c1 = ptObj->addCoil(0);
      int c2 = ptObj->addCoil(0);
      int c3 = ptObj->addCoil(1);
      int c4 = ptObj->addCoil(2);
      int c5 = ptObj->addCoil(3);
      ptObj->setCoilCoords(c1, 1.5, 1.5, 1.5);
      ptObj->setCoilSNR(c1, 50);
      ptObj->setCoilCoords(c2, 1.6, 1.4, 1.3);
      ptObj->setCoilSNR(c2, 25);
      ptObj->setCoilCoords(c3, 3.5, 2.5, 1.5);
      ptObj->setCoilSNR(c3, 40);
      ptObj->setCoilCoords(c4, 4.5, 2.0, 2.0);
      ptObj->setCoilSNR(c4, 40);
      ptObj->setCoilCoords(c5, 6.5, 1.5, 2.5);
      ptObj->setCoilSNR(c5, 40);
      ptObj->Modified();
    }
  }

  if (m_smallVol >= 0) {
    rt3DVolumeDataObject* ptObj = static_cast<rt3DVolumeDataObject*>(rtBaseHandle::instance().getObjectWithID(m_smallVol));
    if (!ptObj) {
      emit sendOutput("Could Not Get Small Volume Object! FAIL!");
    } else {
      emit sendOutput("Load Small Volume Data...");
      vtkImageSinusoidSource* sinSrc = vtkImageSinusoidSource::New();

      sinSrc->SetWholeExtent(1,128, 1, 128, 1, 128);
      sinSrc->SetDirection(1, 2, 3);
      sinSrc->SetPeriod(25);
      sinSrc->SetPhase(1);
      sinSrc->SetAmplitude(10);
      sinSrc->Update();

      ptObj->copyNewImageData(sinSrc->GetOutput());
      ptObj->translateData(200, 20, 2);
      ptObj->getTransform()->RotateX(25); // Rotate around X 25 degrees
      sinSrc->Delete();
    }
  }

  if (m_hugeVol >= 0) {
    rt3DVolumeDataObject* ptObj = static_cast<rt3DVolumeDataObject*>(rtBaseHandle::instance().getObjectWithID(m_hugeVol));
    if (!ptObj) {
      emit sendOutput("Could Not Get Huge Volume Object! FAIL!");
    } else {
      emit sendOutput("Load Very Large Volume Data...");
      vtkImageSinusoidSource* sinSrc = vtkImageSinusoidSource::New();

      sinSrc->SetWholeExtent(1, 450, 1, 450, 1, 450);
      sinSrc->SetDirection(1, 2, 3);
      sinSrc->SetPeriod(50);
      sinSrc->SetPhase(1);
      sinSrc->SetAmplitude(10);
      sinSrc->Update();

      ptObj->copyNewImageData(sinSrc->GetOutput());
      sinSrc->Delete();
    }
  }

  if (m_2DPlane >= 0) {
    rt2DSliceDataObject* ptObj = static_cast<rt2DSliceDataObject*>(rtBaseHandle::instance().getObjectWithID(m_2DPlane));
    if (!ptObj) {
      emit sendOutput("Could Not Get 2D Plane Object! FAIL!");
    } else {
      emit sendOutput("Load 2D Plane Data...");
      vtkImageSinusoidSource* sinSrc = vtkImageSinusoidSource::New();

      sinSrc->SetWholeExtent(1,256, 1, 256, 1, 1);
      sinSrc->SetDirection(1, 2, 3);
      sinSrc->SetPeriod(30);
      sinSrc->SetPhase(1);
      sinSrc->SetAmplitude(10);
      sinSrc->Update();

      ptObj->getTransform()->Translate(20, 200, 2);
      ptObj->getTransform()->RotateX(25);
      ptObj->getTransform()->RotateY(25);
      ptObj->getTransform()->RotateZ(25);
      ptObj->copyImageData2D(sinSrc->GetOutput());
      sinSrc->Delete();
    }
  }

  if (m_matrix >= 0) {
    rtMatrixDataObject* ptObj = static_cast<rtMatrixDataObject*>(rtBaseHandle::instance().getObjectWithID(m_matrix));
    if (!ptObj) {
      emit sendOutput("Could Not Get Matrix Object! FAIL!");
    } else {
      emit sendOutput("Load Matrix Orientation...");
      ptObj->getTransform()->RotateX(25);
      ptObj->getTransform()->RotateY(25);
      ptObj->getTransform()->RotateZ(25);
      ptObj->Modified();
    }
  }

  emit sendOutput("-------- End Basic Test ---------");
}


void TestSuiteBasic::basicTestCreateObjects() {
  m_label = rtBaseHandle::instance().requestNewObject(rtConstants::OT_TextLabel, "Test Label");
  testObject(m_label, "Test Label");
  m_3DPoints = rtBaseHandle::instance().requestNewObject(rtConstants::OT_3DPointBuffer, "Test 3D Points");
  testObject(m_label, "Test 3D Points");
  m_cath[0] = rtBaseHandle::instance().requestNewObject(rtConstants::OT_Cath, "Test One Coil One Loc");
  m_cath[1] = rtBaseHandle::instance().requestNewObject(rtConstants::OT_Cath, "Test Two Coil Two Loc");
  m_cath[2] = rtBaseHandle::instance().requestNewObject(rtConstants::OT_Cath, "Test Five Coil Four Loc");
  testObject(m_cath[0], "Test One Coil One Loc");
  testObject(m_cath[1], "Test Two Coil Two Loc");
  testObject(m_cath[2], "Test Five Coil Four Loc");
  m_smallVol = rtBaseHandle::instance().requestNewObject(rtConstants::OT_3DObject, "Test Volume 128x128x128");
  m_hugeVol = rtBaseHandle::instance().requestNewObject(rtConstants::OT_3DObject, "Test Volume 450x450x450");
  m_2DPlane = rtBaseHandle::instance().requestNewObject(rtConstants::OT_2DObject, "Test Image 256x256");
  m_matrix = rtBaseHandle::instance().requestNewObject(rtConstants::OT_vtkMatrix4x4, "Test Matrix");
  testObject(m_smallVol, "Test Volume 128x128x128");
  testObject(m_hugeVol, "Test Volume 512x512x512");
  testObject(m_2DPlane, "Test Image 256x256");
  testObject(m_matrix, "Test Matrix");
}

//! Check to see if an object has an ID that is valid.
void TestSuiteBasic::testObject(int id, QString name) {
  if (id >= 0) {
    emit sendOutput("Creating... " + name + " O.K.");
  } else {
    emit sendOutput("Creating... " + name + " FAIL!");
  }
}

