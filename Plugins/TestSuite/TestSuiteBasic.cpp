#include "TestSuiteBasic.h"
#include "rtBaseHandle.h"
#include "objTypes.h"
#include "rtLabelRenderObject.h"
#include "rt3dPointBufferDataObject.h"
#include "rtCathDataObject.h"
#include "rt3dVolumeDataObject.h"
#include "rt2dSliceDataObject.h"
#include "rtMatrixDataObject.h"
#include "rtPolyDataObject.h"
#include "rtColorFuncDataObject.h"
#include "rtPieceFuncDataObject.h"

#include "vtkImageSinusoidSource.h"
#include "vtkColorTransferFunction.h"

#include <QList>

TestSuiteBasic::TestSuiteBasic() {
  m_imgPeriod = 30;
  m_imgChange.setInterval(20);
  connect(&m_imgChange, SIGNAL(timeout()), this, SLOT(changeImage()));
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

      ptObj->lock();
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
      ptObj->unlock();

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
      ptObj->lock();
      int c1 = ptObj->addCoil(0);
      ptObj->setCoilCoords(c1, 1.5, 1.5, 1.5);
      ptObj->setCoilSNR(c1, 50);
      ptObj->Modified();
      ptObj->unlock();
    }
  }

  if (m_cath[1]) {
    rtCathDataObject* ptObj = static_cast<rtCathDataObject*>(rtBaseHandle::instance().getObjectWithID(m_cath[1]));
    if (!ptObj) {
      emit sendOutput("Could Not Get 2 Coil Catheter Object! FAIL!");
    } else {
      emit sendOutput("Setting Cath Data...");
      ptObj->lock();
      int c1 = ptObj->addCoil(0);
      int c2 = ptObj->addCoil(1);
      ptObj->setCoilCoords(c1, 1.5, 1.5, 1.5);
      ptObj->setCoilSNR(c1, 50);
      ptObj->setCoilCoords(c2, 3.5, 2.5, 1.5);
      ptObj->setCoilSNR(c2, 25);
      ptObj->Modified();
      ptObj->unlock();
    }
  }

  if (m_cath[2]) {
    rtCathDataObject* ptObj = static_cast<rtCathDataObject*>(rtBaseHandle::instance().getObjectWithID(m_cath[2]));
    if (!ptObj) {
      emit sendOutput("Could Not Get 5 Coil Catheter Object! FAIL!");
    } else {
      emit sendOutput("Setting Cath Data...");
      ptObj->lock();
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
      ptObj->unlock();
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

      ptObj->lock();
      ptObj->copyNewImageData(sinSrc->GetOutput());
      ptObj->translateData(200, 20, 2);
      ptObj->getTransform()->RotateX(25); // Rotate around X 25 degrees
      ptObj->Modified();
      ptObj->unlock();

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

      sinSrc->SetWholeExtent(1, 425, 1, 425, 1, 425);
      sinSrc->SetDirection(1, 2, 3);
      sinSrc->SetPeriod(50);
      sinSrc->SetPhase(1);
      sinSrc->SetAmplitude(10);
      sinSrc->Update();

      ptObj->lock();
      ptObj->copyNewImageData(sinSrc->GetOutput());
      ptObj->Modified();
      ptObj->unlock();

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

      ptObj->lock();
      ptObj->getTransform()->Translate(20, 200, 2);
      ptObj->getTransform()->RotateX(25);
      ptObj->getTransform()->RotateY(25);
      ptObj->getTransform()->RotateZ(25);
      ptObj->copyImageData2D(sinSrc->GetOutput());
      ptObj->Modified();
      ptObj->unlock();

      sinSrc->Delete();
    }
  }

  if (m_matrix >= 0) {
    rtMatrixDataObject* ptObj = static_cast<rtMatrixDataObject*>(rtBaseHandle::instance().getObjectWithID(m_matrix));
    if (!ptObj) {
      emit sendOutput("Could Not Get Matrix Object! FAIL!");
    } else {
      emit sendOutput("Load Matrix Orientation...");

      ptObj->lock();
      ptObj->getTransform()->RotateX(25);
      ptObj->getTransform()->RotateY(25);
      ptObj->getTransform()->RotateZ(25);
      ptObj->Modified();
      ptObj->unlock();

    }
  }

  if (m_polyObj>=0) {
    rtPolyDataObject* ptObj = static_cast<rtPolyDataObject*>(rtBaseHandle::instance().getObjectWithID(m_polyObj));
    if (!ptObj) {
      emit sendOutput("Could Not Get Poly Object! FAIL!");
    } else {
      emit sendOutput("Load Poly Object Surfaces...");
      rtPolyDataObject::PolyPoint tempPt;
      rtPolyDataObject::PolyPointLink tempLink;
      QList<rtPolyDataObject::PolyPoint> ptList;
      QList<rtPolyDataObject::PolyPointLink> linkList;

      tempPt.ptList[0] = 0.0;
      tempPt.ptList[1] = 0.0;
      tempPt.ptList[2] = 0.0;
      tempPt.color[0] = 255;
      tempPt.color[1] = 0;
      tempPt.color[2] = 0;
      ptList.push_back(tempPt);

      tempPt.ptList[0] = 10.0;
      tempPt.ptList[1] = 0.0;
      tempPt.ptList[2] = 0.0;
      tempPt.color[0] = 0;
      tempPt.color[1] = 255;
      tempPt.color[2] = 0;
      ptList.push_back(tempPt);

      tempPt.ptList[0] = 0.0;
      tempPt.ptList[1] = 10.0;
      tempPt.ptList[2] = 0.0;
      tempPt.color[0] = 0;
      tempPt.color[1] = 0;
      tempPt.color[2] = 255;
      ptList.push_back(tempPt);

      tempPt.ptList[0] = 15.0;
      tempPt.ptList[1] = 15.0;
      tempPt.ptList[2] = -10.0;
      tempPt.color[0] = 255;
      tempPt.color[1] = 0;
      tempPt.color[2] = 0;
      ptList.push_back(tempPt);

      tempPt.ptList[0] = 0.0;
      tempPt.ptList[1] = 0.0;
      tempPt.ptList[2] = -10.0;
      tempPt.color[0] = 128;
      tempPt.color[1] = 128;
      tempPt.color[2] = 128;
      ptList.push_back(tempPt);

      tempLink.threeVertex[0] = 0;
      tempLink.threeVertex[1] = 1;
      tempLink.threeVertex[2] = 2;
      linkList.push_back(tempLink);

      tempLink.threeVertex[0] = 3;
      tempLink.threeVertex[1] = 1;
      tempLink.threeVertex[2] = 2;
      linkList.push_back(tempLink);

      tempLink.threeVertex[0] = 1;
      tempLink.threeVertex[1] = 0;
      tempLink.threeVertex[2] = 4;
      linkList.push_back(tempLink);

      tempLink.threeVertex[0] = 2;
      tempLink.threeVertex[1] = 0;
      tempLink.threeVertex[2] = 4;
      linkList.push_back(tempLink);

      ptObj->lock();
      emit sendOutput("Load Poly Object Geometry...");
      ptObj->setNewGeometry(&ptList, &linkList);
      emit sendOutput("Done Loading Poly Object Geometry...");
      ptObj->Modified();
      ptObj->unlock();
    }
  }


  if (m_ctf>=0) {
    rtColorFuncDataObject* ptObj = static_cast<rtColorFuncDataObject*>(rtBaseHandle::instance().getObjectWithID(m_ctf));
    if (!ptObj) {
      emit sendOutput("Could Not Get Color Transfer Function! FAIL!");
    } else {
      emit sendOutput("Load Color Function Points...");
      vtkColorTransferFunction* temp = vtkColorTransferFunction::New();
      temp->SetColorSpaceToRGB();
      temp->RemoveAllPoints();
      temp->AddRGBPoint(0.0, 0.0, 0.0, 0.0);
      temp->AddRGBPoint(255.0, 1.0, 1.0, 1.0);

      // Send the color function to the object.
      ptObj->lock();
      ptObj->setColorFunction(temp);
      ptObj->Modified();
      ptObj->unlock();

      temp->Delete();
    }
  }

  if (m_ctfGreen>=0) {
    rtColorFuncDataObject* ptObj = static_cast<rtColorFuncDataObject*>(rtBaseHandle::instance().getObjectWithID(m_ctfGreen));
    if (!ptObj) {
      emit sendOutput("Could Not Get Color Transfer Function! FAIL!");
    } else {
      emit sendOutput("Load Color Function Points...");
      vtkColorTransferFunction* temp = vtkColorTransferFunction::New();
      temp->SetColorSpaceToRGB();
      temp->RemoveAllPoints();
      temp->AddRGBPoint(0.0, 0.0, 0.0, 0.0);
      temp->AddRGBPoint(255.0, 0.1, 1.0, 0.1);

      // Send the color function to the object.
      ptObj->lock();
      ptObj->setColorFunction(temp);
      ptObj->Modified();
      ptObj->unlock();

      temp->Delete();
    }
  }

  if (m_piece>=0) {
    rtPieceFuncDataObject* ptObj = static_cast<rtPieceFuncDataObject*>(rtBaseHandle::instance().getObjectWithID(m_piece));
    if (!ptObj) {
      emit sendOutput("Could Not Get Color Transfer Function! FAIL!");
    } else {
      emit sendOutput("Load Color Function Points...");
      vtkPiecewiseFunction* temp = vtkPiecewiseFunction::New();
      temp->RemoveAllPoints();
      temp->AddPoint(0.0, 0.0);
      temp->AddPoint(255.0, 1.0);

      // Send the color function to the object.
      ptObj->lock();
      ptObj->setPiecewiseFunction(temp);
      ptObj->Modified();
      ptObj->unlock();

      temp->Delete();
    }
  }

  m_imgChange.start();
  // Start the event loop.
  exec();
  m_imgChange.stop();

  emit sendOutput("-------- End Basic Test ---------");
}

//! Ask the base to create a number of objects of different types. The objects will be used later for testing.
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

  // Two test volumes
  m_smallVol = rtBaseHandle::instance().requestNewObject(rtConstants::OT_3DObject, "Test Volume 128x128x128");
  m_hugeVol = rtBaseHandle::instance().requestNewObject(rtConstants::OT_3DObject, "Test Volume 425x425x425");
  testObject(m_smallVol, "Test Volume 128x128x128");
  testObject(m_hugeVol, "Test Volume 425x425x425");

  // Test image
  m_2DPlane = rtBaseHandle::instance().requestNewObject(rtConstants::OT_2DObject, "Test Image 256x256");
  testObject(m_2DPlane, "Test Image 256x256");

  // Test matrix
  m_matrix = rtBaseHandle::instance().requestNewObject(rtConstants::OT_vtkMatrix4x4, "Test Matrix");
  testObject(m_matrix, "Test Matrix");

  // Poly Data Object
  m_polyObj = rtBaseHandle::instance().requestNewObject(rtConstants::OT_vtkPolyData, "Poly Surface Object");
  testObject(m_polyObj, "Poly Surface Object");

  // Color transfer function
  m_ctf = rtBaseHandle::instance().requestNewObject(rtConstants::OT_vtkColorTransferFunction, "Basic Grey");
  m_ctfGreen = rtBaseHandle::instance().requestNewObject(rtConstants::OT_vtkColorTransferFunction, "Greenish");
  testObject(m_ctf, "Basic Grey");
  testObject(m_ctfGreen, "Greenish");

  // Piecewise function
  m_piece = rtBaseHandle::instance().requestNewObject(rtConstants::OT_vtkPiecewiseFunction, "Standard Scale");
  testObject(m_piece, "Standard Scale");
}

//! Check to see if an object has an ID that is valid.
void TestSuiteBasic::testObject(int id, QString name) {
  if (id >= 0) {
    emit sendOutput("Creating... " + name + " O.K.");
  } else {
    emit sendOutput("Creating... " + name + " FAIL!");
  }
}

void TestSuiteBasic::changeImage() {
  if (m_2DPlane >= 0) {
    rt2DSliceDataObject* ptObj = static_cast<rt2DSliceDataObject*>(rtBaseHandle::instance().getObjectWithID(m_2DPlane));
    if (!ptObj) {
      emit sendOutput("Could Not Get 2D Plane Object! FAIL!");
    } else {
      emit sendOutput("Change 2D Plane Info...");
      vtkImageSinusoidSource* sinSrc = vtkImageSinusoidSource::New();

      sinSrc->SetWholeExtent(1,256, 1, 256, 1, 1);
      sinSrc->SetDirection(1, 2, 3);
      sinSrc->SetPeriod(m_imgPeriod);
      sinSrc->SetPhase(1);
      sinSrc->SetAmplitude(10);
      sinSrc->Update();

      m_imgPeriod = m_imgPeriod + 1;
      if (m_imgPeriod > 100) m_imgPeriod = 10;

      ptObj->lock();
      ptObj->getTransform()->RotateX(2);
      ptObj->copyImageData2D(sinSrc->GetOutput());
      ptObj->Modified();
      ptObj->unlock();

      sinSrc->Delete();
    }
  }
}
