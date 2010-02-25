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
#include "DICOMFileReader.h"

#include <QDir>
#include <QStringList>

#include <iostream>
#include <string>
#include <math.h>

#include <vtkMath.h>

//DCMTK
#include "osconfig.h"
#include "dcdeftag.h"
#include "dcfilefo.h"
#include "ofcond.h"
#include "dcdatset.h"
#include "dctagkey.h"

DICOMFileReader::DICOMFileReader() {
  m_vtkImgData = vtkImageData::New();
  m_transform = vtkTransform::New();
  m_matrixToTransform = vtkMatrixToLinearTransform::New();
  m_matrix = vtkMatrix4x4::New();
  m_infoFix = vtkImageChangeInformation::New();

  // Set up the mini pipeline.
  m_matrix->Identity();
  m_matrixToTransform->SetInput(m_matrix);
  m_transform->SetInput(m_matrixToTransform);
  m_transform->Inverse();

  m_ddata = NULL;

  m_volName = "";
}


DICOMFileReader::~DICOMFileReader() {
  cleanupImageData();
  m_matrixToTransform->Delete();
  m_matrix->Delete();
  m_vtkImgData->Delete();
  m_transform->Delete();
  m_infoFix->Delete();

  if (m_ddata) delete m_ddata;
}

void DICOMFileReader::cleanupImageData() {

  //Clear the list
  while (!m_imgData.empty()) {
    DICOMImageData* temp;
    temp = m_imgData.takeFirst();
    if (temp == m_ddata) {
      m_ddata=NULL;
    }
    delete temp;
  }
  m_comments.clear();
  // Reset the trigger list.
  m_triggerList.clear();
}

//! Set the name of the directory to get the DICOM files from.
bool DICOMFileReader::setDirectory(QString dirPath) {

  QDir fileDir(dirPath);
  QStringList files;
  QStringList filters;
  filters << "*.dcm" << "*.DCM" << "*.MR.dcm";

  if (!fileDir.exists()) {
    std::cout << "No such directory: " << dirPath.toStdString() << std::endl;
    return false;
  }

  fileDir.setFilter(QDir::NoSymLinks | QDir::NoDotAndDotDot | QDir::Readable | QDir::Files);
  fileDir.setNameFilters(filters);
  files = fileDir.entryList();

  if (files.count() <= 0) {
    std::cout << "Directory found, but no DICOM files could be found inside:  " << dirPath.toStdString() << std::endl;
    return false;
  }

  cleanupImageData();

  double maxTrig=-1.0;

  // Create the trigger list for the first set of images.
  for (int ix1=0; ix1<files.count(); ix1++) {
    double trig = DICOMImageData::getTriggerFromFile(fileDir.filePath(files.at(ix1)));
    if (!m_triggerList.contains(trig) && trig > maxTrig) {
      m_triggerList.append(trig);
      maxTrig = trig;
    } else {
      break;
    }
  }

  for (int ix1=0; ix1<files.count(); ix1++) {
    m_comments.append("Image Number: ===== " + QString::number(ix1+1) + " ===== \n");
    m_ddata = new DICOMImageData();
    // Some image types need a trigger list. 
    m_ddata->setTrigList(&m_triggerList);
    if ( !m_ddata->readFile(fileDir.filePath(files.at(ix1))) ) {
      // Failed to read the file.
      std::cout << "Failed to Read DICOM File: " << fileDir.filePath(files.at(ix1)).toStdString() << std::endl;
      return false;
    } else {
      // File was read correctly.
      m_imgData.append(m_ddata);
    }
  }

  // Give that volume a default name.
  m_volName = fileDir.dirName();
  return true;
}



bool DICOMFileReader::createVolume(QList<DICOMImageData*>* imgData) {
  // Check for the image data pointer
  if (!imgData) {
    std::cout << "Error: No Image Pointer!" << std::endl;
    return false;
  }
  // Check that there are actually elements in the pointer.
  if (imgData->count() <= 0) {
    std::cout << "Error: Number of images read is: " << imgData->count() << std::endl;
    return false;
  }

  int numZSlices, numFrames, numRow, numCol, volZSlices;
  int locIdx = 0; // Default is HFS.

  // Set up the patient position for the first file. Should be the same for all the others too!
  for (int ix1=0; ix1<NUM_ENTRIES; ix1++) {
    if ( imgData->at(0)->getPatientPosition() == ENTRY_STRINGS[ix1].c_str() ) {
      locIdx = ix1;
    }
  }

  // Get the number of rows and columns
  numRow = imgData->at(0)->getNumRows();
  numCol = imgData->at(0)->getNumCols();

  // Chack for cine. If there are multiple images per cycle then this is a movie.
  if ( imgData->at(0)->isCineData() ) {
    // Check if the number of images is devisible by the number of images in one cycle.
    if (imgData->count() % imgData->at(0)->getImagesPerCycle() != 0) {
      std::cout << "Error: Number of images: " << imgData->count() << " Not divisible by: " << imgData->at(0)->getImagesPerCycle() << std::endl;
      return false;
    }

    numFrames = imgData->at(0)->getImagesPerCycle();
    numZSlices = (int) (imgData->count() / imgData->at(0)->getImagesPerCycle());
  } else {
    numFrames = 1;
    numZSlices = imgData->count();
  }

  // Number of Z slices in the volume is the same as in the DICOM files EXCEPT when there is only one slice.
  // To be a valid volume the thrid dimension must be at least 2.
  if (numZSlices == 1) {
    volZSlices = 2;
  } else {
    volZSlices = numZSlices;
  }

  // Create the image data.
  m_vtkImgData->SetScalarTypeToShort();
  m_vtkImgData->SetDimensions(numRow, numCol, volZSlices);
  m_vtkImgData->SetNumberOfScalarComponents(numFrames);
  m_vtkImgData->AllocateScalars();

  // Get the position of the first slice.
  double pos[3];
  pos[0] = ENTRY_FLIPS[locIdx][0]*imgData->at(0)->getImagePosition(0);
  pos[1] = ENTRY_FLIPS[locIdx][1]*imgData->at(0)->getImagePosition(1);
  pos[2] = ENTRY_FLIPS[locIdx][2]*imgData->at(0)->getImagePosition(2);

  // Get the row and column orientations.
  double rowOrient[3];
  double colOrient[3];

  rowOrient[0] = ENTRY_FLIPS[locIdx][0]*imgData->at(0)->getImageOrientation(0, 0);
  rowOrient[1] = ENTRY_FLIPS[locIdx][1]*imgData->at(0)->getImageOrientation(0, 1);
  rowOrient[2] = ENTRY_FLIPS[locIdx][2]*imgData->at(0)->getImageOrientation(0, 2);

  colOrient[0] = ENTRY_FLIPS[locIdx][0]*imgData->at(0)->getImageOrientation(1, 0);
  colOrient[1] = ENTRY_FLIPS[locIdx][1]*imgData->at(0)->getImageOrientation(1, 1);
  colOrient[2] = ENTRY_FLIPS[locIdx][2]*imgData->at(0)->getImageOrientation(1, 2);

  // Calclulate the z vector as the cross product of the row and column vectors.
  double zVec[3];
  vtkMath::Cross(rowOrient, colOrient, zVec);

  double zspacing = 0.0;
  double negZed = 1;

  if (numZSlices > 1) {
    double xd, yd, zd;

    // At least two Z Slices
    xd = ENTRY_FLIPS[locIdx][0]*(imgData->at(0)->getImagePosition(0)-imgData->at(numFrames)->getImagePosition(0));
    yd = ENTRY_FLIPS[locIdx][1]*(imgData->at(0)->getImagePosition(1)-imgData->at(numFrames)->getImagePosition(1));
    zd = ENTRY_FLIPS[locIdx][2]*(imgData->at(0)->getImagePosition(2)-imgData->at(numFrames)->getImagePosition(2));
    zspacing = sqrt(xd*xd + yd*yd + zd*zd);

    // Check for a sane spacing...
    if (zspacing < 0.001) zspacing = 0.5;

    // Check which direction to put the slices in.
    double flipFlag = zVec[0]*xd+zVec[1]*yd+zVec[2]*zd;

    if (flipFlag < 0) {
      negZed = -1;
    } else {
      negZed = 1;
    }
  } else {
    // Only one slice so there really is no z spacing or z direction. Default to 1.
    zspacing = 1.0f;
    negZed = 1.0f;
  }

  // Set the spacing for the image data.
  m_vtkImgData->SetSpacing(imgData->at(0)->getPixelSpace(0), -imgData->at(0)->getPixelSpace(1), negZed*zspacing);

  short* temp;
  short* imgPtr;
  // Copy and the data
  for (int ix1=0; ix1<volZSlices; ix1++) {
    for (int row=0; row<imgData->at(0)->getNumRows(); row++) {
      for (int col=0; col<imgData->at(0)->getNumCols(); col++) {
        temp = (short*)m_vtkImgData->GetScalarPointer(col, row, ix1);
        for (int frame = 0; frame<numFrames; frame++) {
          imgPtr = imgData->at( (ix1%numZSlices)*numFrames+frame )->getDataPtr();
          *temp =  imgPtr[row*imgData->at(0)->getNumCols()+col];
          temp++;
        }
      }
    }
  }

  // Reset the spacing.
  m_infoFix->SetInput(m_vtkImgData);
  m_infoFix->CenterImageOff();
  m_infoFix->SetOutputSpacing(imgData->at(0)->getPixelSpace(0), imgData->at(0)->getPixelSpace(1), -1*negZed*zspacing);
  m_infoFix->Update();

  // Set the orientation
  m_matrix->SetElement(0, 0, rowOrient[0]);
  m_matrix->SetElement(1, 0, rowOrient[1]);
  m_matrix->SetElement(2, 0, rowOrient[2]);
  m_matrix->SetElement(0, 1, colOrient[0]);
  m_matrix->SetElement(1, 1, colOrient[1]);
  m_matrix->SetElement(2, 1, colOrient[2]);
  m_matrix->SetElement(0, 2, zVec[0]);
  m_matrix->SetElement(1, 2, zVec[1]);
  m_matrix->SetElement(2, 2, zVec[2]);

  // Set the position
  m_matrix->SetElement(0, 3, pos[0]);
  m_matrix->SetElement(1, 3, pos[1]);
  m_matrix->SetElement(2, 3, pos[2]);

  return true;
}
