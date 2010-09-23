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
#include <vtkImageShiftScale.h>

#include <QList>
#include <QString>

#include "TestLargeVol.h"
#include "rtBaseHandle.h"
#include "objTypes.h"
#include "rt3dVolumeDataObject.h"

TestLargeVol::TestLargeVol() {
}

TestLargeVol::~TestLargeVol() {
}

void TestLargeVol::run() {
  emit sendOutput("--------- Create Large Volume ----------");
  createObjects();

  if (m_hugeVol >= 0) {
    rt3DVolumeDataObject* ptObj = static_cast<rt3DVolumeDataObject*>(rtBaseHandle::instance().getObjectWithID(m_hugeVol));
    if (!ptObj) {
      emit sendOutput("Could Not Get Huge Volume Object! FAIL!");
    } else {
      emit sendOutput("Load Very Large Volume Data...");
      vtkImageSinusoidSource* sinSrc = vtkImageSinusoidSource::New();

      sinSrc->SetWholeExtent(0, 399, 0, 399, 0, 399);
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

  emit sendOutput("-------- Create Large Volume Finished ---------");
}

//! Ask the base to create a number of objects of different types. The objects will be used later for testing.
void TestLargeVol::createObjects() {
  m_hugeVol = rtBaseHandle::instance().requestNewObject(rtConstants::OT_3DObject, "Test Volume 400x400x400");
  testObject(m_hugeVol, "Test Volume 400x400x400");
}

//! Check to see if an object has an ID that is valid.
void TestLargeVol::testObject(int id, QString name) {
  if (id >= 0) {
    emit sendOutput("Creating... " + name + " O.K.");
  } else {
    emit sendOutput("Creating... " + name + " FAIL!");
  }
}

