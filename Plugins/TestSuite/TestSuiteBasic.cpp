/*******************************************************************************
    Vurtigo: Visualization software for interventional applications in medicine
    Copyright (C) 2009 Sunnybrook Health Sciences Centre

    This file is part of Vurtigo.

    Vurtigo is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#include <vtkImageSinusoidSource.h>
#include <vtkColorTransferFunction.h>
#include <vtkPNGReader.h>
#include <vtkDICOMImageReader.h>
#include <vtkImageShiftScale.h>

#include <QList>
#include <QString>
#include <QFileDialog>

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
#include "rtBasic3DPointData.h"



TestSuiteBasic::TestSuiteBasic() {
  m_imgPeriod = 30;
  m_imgChange.setInterval(20);
  connect(&m_imgChange, SIGNAL(timeout()), this, SLOT(changeImage()));
  m_pngFileName = "";
  m_dicomFileName = "";
  m_imgMapToColors = vtkImageMapToColors::New();
  m_lookupTable = vtkWindowLevelLookupTable::New();
}

TestSuiteBasic::~TestSuiteBasic() {
  if (m_imgMapToColors) m_imgMapToColors->Delete();
  if (m_lookupTable) m_lookupTable->Delete();
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
      rtBasic3DPointData pp[10][10];
      rtBasic3DPointData p[4];

      p[0].setX(0.0);
      p[0].setY(0.0);
      p[0].setZ(0.0);
      p[0].setPointSize(0.25);
      p[0].getProperty()->SetColor(1, 0, 0); //Red
      p[0].getProperty()->SetOpacity(0.2);

      p[1].setX(1.0);
      p[1].setY(0.0);
      p[1].setZ(0.0);
      p[1].setPointSize(0.25);
      p[1].getProperty()->SetColor(1, 0, 0);

      p[2].setX(0.0);
      p[2].setY(1.0);
      p[2].setZ(0.0);
      p[2].setPointSize(0.25);
      p[2].getProperty()->SetColor(0, 1, 0);

      p[3].setX(0.0);
      p[3].setY(0.0);
      p[3].setZ(1.0);
      p[3].setPointSize(0.25);
      p[3].getProperty()->SetColor(0, 0, 1);

      ptObj->lock();
      for (int ix1=0; ix1<4; ix1++) {
        ptObj->addPoint(p[ix1]);
      }

      // Add 100 semi-transparent spheres.
      for (int ix1=2; ix1<12; ix1++) {
        for (int ix2=2; ix2<12; ix2++) {
          pp[ix1-2][ix2-2].setX(ix1);
          pp[ix1-2][ix2-2].setY(ix2);
          pp[ix1-2][ix2-2].setZ(2);
          pp[ix1-2][ix2-2].setPointSize(0.25);
          pp[ix1-2][ix2-2].getProperty()->SetColor(0,1,0);
          pp[ix1-2][ix2-2].getProperty()->SetOpacity(0.2);
          ptObj->addPoint(pp[ix1-2][ix2-2]);
        }
      }
      ptObj->Modified();
      ptObj->unlock();
    }
      
    m_lookupTable->SetHueRange(0.5, 1.0);
    m_lookupTable->SetSaturationRange(0.5, 1.0);
    m_lookupTable->SetValueRange(0.5, 1.0);

    m_imgMapToColors->SetOutputFormatToRGB();
    m_imgMapToColors->SetLookupTable(m_lookupTable);

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
      ptObj->setCoilCoords(c2, 13.2, 7.5, 2.5);
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
      ptObj->setCoilCoords(c3, 13.2, 7.5, 2.5);
      ptObj->setCoilSNR(c3, 40);
      ptObj->setCoilCoords(c4, 22.5, 9.0, 7.0);
      ptObj->setCoilSNR(c4, 40);
      ptObj->setCoilCoords(c5, 27.3, 11.2, 9.1);
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

      sinSrc->SetWholeExtent(0,127, 0, 127, 0, 127);
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

  if (m_2DPlane >= 0) {
    rt2DSliceDataObject* ptObj = static_cast<rt2DSliceDataObject*>(rtBaseHandle::instance().getObjectWithID(m_2DPlane));
    if (!ptObj) {
      emit sendOutput("Could Not Get 2D Plane Object! FAIL!");
    } else {
      emit sendOutput("Load 2D Plane Data...");
      vtkImageSinusoidSource* sinSrc = vtkImageSinusoidSource::New();
      vtkImageShiftScale *shift = vtkImageShiftScale::New();

      sinSrc->SetWholeExtent(0,255, 0, 255, 0, 0);
      sinSrc->SetDirection(1, 2, 3);
      sinSrc->SetPeriod(30);
      sinSrc->SetPhase(1);
      sinSrc->SetAmplitude(10);
      sinSrc->Update();
      
      shift->SetInput(sinSrc->GetOutput());
      shift->SetOutputScalarType(VTK_UNSIGNED_CHAR);
      shift->SetShift(20);
      shift->Update();

      ptObj->lock();
      ptObj->getTransform()->Translate(20, 200, 2);
      ptObj->getTransform()->RotateX(25);
      ptObj->getTransform()->RotateY(25);
      ptObj->getTransform()->RotateZ(25);
      ptObj->copyImageData2D(shift->GetOutput());
      ptObj->Modified();
      ptObj->unlock();

      sinSrc->Delete();
    }
  }
  
  if (m_2DPlaneColor >= 0) {
    rt2DSliceDataObject* ptObj = static_cast<rt2DSliceDataObject*>(rtBaseHandle::instance().getObjectWithID(m_2DPlaneColor));
    if (!ptObj) {
      emit sendOutput("Could Not Get 2D Color Plane Object! FAIL!");
    } else {
      emit sendOutput("Load 2D Color Plane Data...");

      vtkImageSinusoidSource* sinSrc = vtkImageSinusoidSource::New();

      vtkPNGReader *pngReader = vtkPNGReader::New();
      pngReader->SetFileName( m_pngFileName.toStdString().c_str() );
      pngReader->SetNumberOfScalarComponents(3);
      pngReader->SetDataScalarType(VTK_UNSIGNED_CHAR);
      pngReader->Update();

      ptObj->lock();
      ptObj->getTransform()->Translate(100, 300, 2);
      ptObj->getTransform()->RotateX(25);
      ptObj->getTransform()->RotateY(25);
      ptObj->getTransform()->RotateZ(25);
      ptObj->copyImageData2D(pngReader->GetOutput());
      ptObj->Modified();
      ptObj->unlock();

      sinSrc->Delete(); 
    }
  }

  if (m_2DPlane16 >= 0) {
    rt2DSliceDataObject* ptObj = static_cast<rt2DSliceDataObject*>(rtBaseHandle::instance().getObjectWithID(m_2DPlane16));
    if (!ptObj) {
      emit sendOutput("Could Not Get 2D 16-bit Plane Object! FAIL!");
    } else {
      emit sendOutput("Load 2D 16-bit Plane Data...");

      vtkDICOMImageReader *dcmReader = vtkDICOMImageReader::New();
      dcmReader->SetFileName( m_dicomFileName.toStdString().c_str() );
      dcmReader->SetNumberOfScalarComponents(1);
      dcmReader->SetDataScalarType(VTK_UNSIGNED_SHORT);
      dcmReader->Update();

      ptObj->lock();
      ptObj->getTransform()->Translate(100, 300, 2);
      ptObj->getTransform()->RotateX(25);
      ptObj->getTransform()->RotateY(25);
      ptObj->getTransform()->RotateZ(25);
      ptObj->copyImageData2D(dcmReader->GetOutput());
      ptObj->Modified();
      ptObj->unlock();
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
      rtBasic3DPointData tempPt;
      rtPolyDataObject::PolyPointLink tempLink;
      QList<rtBasic3DPointData> ptList;
      QList<rtPolyDataObject::PolyPointLink> linkList;

      tempPt.setX(0.0);
      tempPt.setY(0.0);
      tempPt.setZ(0.0);
      tempPt.setColor(1.0, 0.0, 0.0);
      ptList.push_back(tempPt);

      tempPt.setX(10.0);
      tempPt.setY(0.0);
      tempPt.setZ(0.0);
      tempPt.setColor(0.0, 1.0, 0.0);
      ptList.push_back(tempPt);

      tempPt.setX(0.0);
      tempPt.setY(10.0);
      tempPt.setZ(0.0);
      tempPt.setColor(0.0, 0.0, 1.0);
      ptList.push_back(tempPt);

      tempPt.setX(15.0);
      tempPt.setY(15.0);
      tempPt.setZ(-10.0);
      tempPt.setColor(1.0, 0.0, 0.0);
      ptList.push_back(tempPt);

      tempPt.setX(0.0);
      tempPt.setY(0.0);
      tempPt.setZ(-10.0);
      tempPt.setColor(0.5, 0.5, 0.5);
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

  // Test volume
  m_smallVol = rtBaseHandle::instance().requestNewObject(rtConstants::OT_3DObject, "Test Volume 128x128x128");
  testObject(m_smallVol, "Test Volume 128x128x128");

  // Test image
  m_2DPlane = rtBaseHandle::instance().requestNewObject(rtConstants::OT_2DObject, "Test Image 256x256");
  testObject(m_2DPlane, "Test Image 256x256");
  
  // Test image (color)
  m_2DPlaneColor = rtBaseHandle::instance().requestNewObject(rtConstants::OT_2DObject, "Color Test Image 256x256");
  testObject(m_2DPlaneColor, "Color Test Image 256x256");

  // Test image (16-bit grayscale)
  m_2DPlane16 = rtBaseHandle::instance().requestNewObject(rtConstants::OT_2DObject, "Test Image 16-bit 256x256");
  testObject(m_2DPlane16, "Test Image 16-bit 256x256");

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
      vtkImageShiftScale *shift = vtkImageShiftScale::New();

      sinSrc->SetWholeExtent(0,255, 0, 255, 0, 0);
      sinSrc->SetDirection(1, 2, 3);
      sinSrc->SetPeriod(m_imgPeriod);
      sinSrc->SetPhase(1);
      sinSrc->SetAmplitude(10);
      sinSrc->Update();

      shift->SetInput(sinSrc->GetOutput());
      shift->SetOutputScalarType(VTK_UNSIGNED_CHAR);
      shift->SetShift(+20);
      shift->Update();

      m_imgPeriod = m_imgPeriod + 1;
      if (m_imgPeriod > 100) m_imgPeriod = 10;

      vtkTransform *temp;
      vtkMatrix4x4 *matCopy;
      vtkTransform *tempCopy = vtkTransform::New();

      ptObj->lock();
      temp = ptObj->getTransform();
      matCopy = temp->GetMatrix();
      tempCopy->SetMatrix(matCopy);
      tempCopy->RotateX(2);
      ptObj->setVtkMatrix(tempCopy->GetMatrix());
      ptObj->copyImageData2D(shift->GetOutput());
      ptObj->Modified();
      ptObj->unlock();

      tempCopy->Delete();
      shift->Delete();
      sinSrc->Delete();
    }
  }

/*
  if (m_2DPlaneColor >= 0) {
    rt2DSliceDataObject* ptObj = static_cast<rt2DSliceDataObject*>(rtBaseHandle::instance().getObjectWithID(m_2DPlaneColor));
    if (!ptObj) {
      emit sendOutput("Could Not Get 2D Color Plane Object! FAIL!");
    } else {
      emit sendOutput("Change 2D Plane Color Info...");

      vtkTransform *temp;
      vtkMatrix4x4 *matCopy;
      vtkTransform *tempCopy = vtkTransform::New();

      ptObj->lock();
      temp = ptObj->getTransform();
      matCopy = temp->GetMatrix();
      tempCopy->SetMatrix(matCopy);
      tempCopy->RotateX(2);
      ptObj->setVtkMatrix(tempCopy->GetMatrix());
      ptObj->Modified();
      ptObj->unlock();

      tempCopy->Delete();
    }
  }

  if (m_2DPlane16 >= 0) {
    rt2DSliceDataObject* ptObj = static_cast<rt2DSliceDataObject*>(rtBaseHandle::instance().getObjectWithID(m_2DPlane16));
    if (!ptObj) {
      emit sendOutput("Could Not Get 2D 16-bit Plane Object! FAIL!");
    } else {
      emit sendOutput("Change 2D 16-bit Plane Info...");

      vtkTransform *temp;
      vtkMatrix4x4 *matCopy;
      vtkTransform *tempCopy = vtkTransform::New();

      ptObj->lock();
      temp = ptObj->getTransform();
      matCopy = temp->GetMatrix();
      tempCopy->SetMatrix(matCopy);
      tempCopy->RotateX(2);
      ptObj->setVtkMatrix(tempCopy->GetMatrix());
      ptObj->Modified();
      ptObj->unlock();

      tempCopy->Delete();
    }
  }
*/  

  static int count = 0;
  count++;
  if (((count % 50) == 0) && m_cath[0]) {
    rtCathDataObject* ptObj = static_cast<rtCathDataObject*>(rtBaseHandle::instance().getObjectWithID(m_cath[0]));
    if (!ptObj) {
      emit sendOutput("Could Not Get 1 Coil Catheter Object! FAIL!");
    } else {
      emit sendOutput("Moving Catheter...");
      ptObj->lock();
      double pos[3];
      ptObj->getPositionAtLocation(0, pos);
      pos[0] += 25*((rand()%32767)/32767.0 - 0.5);
      pos[1] += 25*((rand()%32767)/32767.0 - 0.5);
      pos[2] += 25*((rand()%32767)/32767.0 - 0.5);
      ptObj->setCoilCoords(0, pos[0], pos[1], pos[2]);
      ptObj->Modified();
      ptObj->unlock();
    }
  }



}
