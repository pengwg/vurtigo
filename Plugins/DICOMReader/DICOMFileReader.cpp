#include "DICOMFileReader.h"

#include <QDir>
#include <QStringList>

#include <iostream>
#include <string>
#include <math.h>

//DCMTK
#include "cfunix.h"
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
}


DICOMFileReader::~DICOMFileReader() {
  cleanupImageData();
  m_matrixToTransform->Delete();
  m_matrix->Delete();
  m_vtkImgData->Delete();
  m_transform->Delete();
  m_infoFix->Delete();
}

void DICOMFileReader::cleanupImageData() {
  while (!m_imgData.isEmpty()) {
    // Cleanup the old image data.
    short* tmp = m_imgData.takeFirst().shortData;
    if (tmp)
      delete [] tmp;
  }
  m_comments.clear();
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

  for (int ix1=0; ix1<files.count(); ix1++) {
    m_comments.append("Image Number: ===== " + QString::number(ix1) + " ===== \n");
    if ( !readFile(fileDir.filePath(files.at(ix1))) ) {
      // Failed to read the file.
      std::cout << "Failed to Read DICOM File: " << fileDir.filePath(files.at(ix1)).toStdString() << std::endl;
      return false;
    } else {
      // File was read correctly.
      m_imgData.append(m_ddata);
    }
  }

  if (!createVolume(&m_imgData)) {
    std::cout << "Could not create final volume." << std::endl;
    return false;
  }
  return true;
}

//! Read a single DICOM file
bool DICOMFileReader::readFile(QString fName) {
  DcmFileFormat dcmFile;
  OFCondition status;
  DcmDataset* datSet;

  const unsigned short* temp;

  // Load the DICOM file.
  status = dcmFile.loadFile(fName.toStdString().c_str());

  // Check that the file was loaded
  if (status.bad()) {
    std::cout << "Error: cannot read DICOM file (" << status.text() << ")" << std::endl;
    return false;
  }

  datSet = dcmFile.getDataset();

  if (datSet->findAndGetOFString(DCM_PatientsName, m_ddata.patientsName).good()) {
    m_comments.append("Patient's Name: " + QString(m_ddata.patientsName.c_str()) + "\n");
  }
  if (datSet->findAndGetOFString(DCM_StudyDate, m_ddata.studyDate).good()) {
    m_comments.append("Study Date: " + QString(m_ddata.studyDate.c_str()) + "\n");
  }
  if (datSet->findAndGetOFString(DCM_StudyTime, m_ddata.studyTime).good()) {
    //std::cout << "Study Time: " << m_ddata.studyTime << std::endl;
  }
  if (datSet->findAndGetOFString(DCM_PatientPosition, m_ddata.patientPosition).good()) {
    //std::cout << "Patient Position: " << m_ddata.patientPosition << std::endl;
  }
  if (datSet->findAndGetUint16(DCM_Rows, m_ddata.numRows).good()) {
    m_comments.append("Number of Rows: " + QString::number(m_ddata.numRows) + "\n");
  }
  if (datSet->findAndGetUint16(DCM_Columns, m_ddata.numCols).good()) {
    m_comments.append("Number of Columns: " + QString::number(m_ddata.numCols) + "\n");
  }
  if (datSet->findAndGetUint16(DCM_BitsStored, m_ddata.bitsPerPixel).good()) {
    //std::cout << "Bits Per Pixel: " << m_ddata.bitsPerPixel << std::endl;
  }
  if (datSet->findAndGetSint32(DCM_NumberOfFrames, m_ddata.numFrames).good()) {
    // std::cout << "Number of Frames: " << m_ddata.numFrames << std::endl;
  } else {
    m_ddata.numFrames = 1;
  }

  OFString numImgs;
  OFString currImg;
  // Check for cine.
  if (datSet->findAndGetOFString(DCM_CardiacNumberOfImages, numImgs).good()) {
    //std::cout << "Images For Each Cardiac Cycle: " << m_ddata.imagesPerCycle.c_str() << std::endl;
    m_ddata.imagesPerCycle = QString(numImgs.c_str()).toUShort();
  }
  if (m_ddata.imagesPerCycle < 1) {
     m_ddata.imagesPerCycle = 1;
  }
  m_comments.append("Images Per Cycle: " + QString::number(m_ddata.imagesPerCycle) + "\n");

  // Special GE tag. The current cardiac cycle.
  if (datSet->findAndGetOFString(DcmTagKey(0x0019, 0x10d7), currImg).good()) {
    //std::cout << "Cardiac Phase: " << m_ddata.cardiacPhase.c_str() << std::endl;
    m_ddata.cardiacPhase = QString(currImg.c_str()).toUShort();
  }
  if (m_ddata.cardiacPhase < 1) {
     m_ddata.cardiacPhase = 1;
  }



  if (datSet->findAndGetUint16(DCM_PixelRepresentation, m_ddata.sine).good()) {
    // std::cout << "Pixels are signed: " << m_ddata.sine << std::endl;
  }
  if (datSet->findAndGetUint32(DCM_PixelDataGroupLength, m_ddata.pixelGroupLen).good()) {
    //std::cout << "Pixel Group Length: " << m_ddata.pixelGroupLen << std::endl;
  }
  if (datSet->findAndGetUint16Array(DCM_PixelData, temp, &m_ddata.numElements, false).good()) {
    //std::cout << "Read data points: "  <<  m_ddata.numElements << std::endl;
  }

  if (m_ddata.numElements != m_ddata.numRows* m_ddata.numCols && m_ddata.numElements > 0) {
    std::cout << "Error: Problem reading all of the dicom pixel data." << std::endl;
    return false;
  }

  m_ddata.shortData = new short[m_ddata.numElements];
  memcpy(m_ddata.shortData, temp, sizeof(short)*m_ddata.numElements);

  int ix1;
  for (ix1=0; ix1<2; ix1++) datSet->findAndGetFloat64(DCM_PixelSpacing, m_ddata.pixSpace[ix1], ix1);
  for (ix1=0; ix1<3; ix1++) datSet->findAndGetFloat64(DCM_ImagePositionPatient, m_ddata.imgPosition[ix1], ix1);
  for (ix1=0; ix1<6; ix1++) datSet->findAndGetFloat64(DCM_ImageOrientationPatient, m_ddata.imgOrient[ix1], ix1);

  // Find the FOV.
  m_ddata.fov = (m_ddata.pixSpace[0]*m_ddata.numRows)/10.0f;

  if (datSet->findAndGetFloat64(DCM_SliceThickness, m_ddata.sliceThickness).good()) {
    //std::cout << "Slice Thickness: " << m_ddata.sliceThickness << std::endl;
  }

  return true;

}


bool DICOMFileReader::createVolume(QList<DICOMImageData>* imgData) {
  if (!imgData) return false;

  int locIdx = 0; // Default is HFS.
  // Set up the patient position.
  for (int ix1=0; ix1<NUM_ENTRIES; ix1++) {
    if ( m_ddata.patientPosition == ENTRY_STRINGS[ix1].c_str() ) {
      locIdx = ix1;
    }
  }

  if (imgData->count() <= 0) {
    return false;
  } else if (imgData->count() == 1) {
    // Just One Slice
    m_vtkImgData->SetScalarTypeToShort();
    m_vtkImgData->SetDimensions(imgData->at(0).numRows, imgData->at(0).numCols, 1);
    m_vtkImgData->AllocateScalars();

    // TODO this single slice option.
  } else if (imgData->count() > 1) {

    // Multiple Slices
    m_vtkImgData->SetScalarTypeToShort();
    m_vtkImgData->SetDimensions(imgData->at(0).numRows, imgData->at(0).numCols, imgData->count());
    m_vtkImgData->AllocateScalars();

    // Calculate the Z direction spacing.
    double xd, yd, zd;
    double zspacing = 0.0;

    xd = ENTRY_FLIPS[locIdx][0]*(imgData->at(0).imgPosition[0]-imgData->at(1).imgPosition[0]);
    yd = ENTRY_FLIPS[locIdx][1]*(imgData->at(0).imgPosition[1]-imgData->at(1).imgPosition[1]);
    zd = ENTRY_FLIPS[locIdx][2]*(imgData->at(0).imgPosition[2]-imgData->at(1).imgPosition[2]);

    double pos[3];
    double rowOrient[3];
    double colOrient[3];

    pos[0] = ENTRY_FLIPS[locIdx][0]*imgData->at(0).imgPosition[0];
    pos[1] = ENTRY_FLIPS[locIdx][1]*imgData->at(0).imgPosition[1];
    pos[2] = ENTRY_FLIPS[locIdx][2]*imgData->at(0).imgPosition[2];

    rowOrient[0] = ENTRY_FLIPS[locIdx][0]*imgData->at(0).imgOrient[0];
    rowOrient[1] = ENTRY_FLIPS[locIdx][1]*imgData->at(0).imgOrient[1];
    rowOrient[2] = ENTRY_FLIPS[locIdx][2]*imgData->at(0).imgOrient[2];

    colOrient[0] = ENTRY_FLIPS[locIdx][0]*imgData->at(0).imgOrient[3];
    colOrient[1] = ENTRY_FLIPS[locIdx][1]*imgData->at(0).imgOrient[4];
    colOrient[2] = ENTRY_FLIPS[locIdx][2]*imgData->at(0).imgOrient[5];

    // Calculate the z vector
    double zVec[3];
    zVec[0] = (rowOrient[1]*colOrient[2]-rowOrient[2]*colOrient[1]);
    zVec[1] = (rowOrient[2]*colOrient[0]-rowOrient[0]*colOrient[2]);
    zVec[2] = (rowOrient[0]*colOrient[1]-rowOrient[1]*colOrient[0]);

    // Check which direction to put the slices in.
    double flipFlag = zVec[0]*xd+zVec[1]*yd+zVec[2]*zd;

    zspacing = sqrt(xd*xd + yd*yd + zd*zd);

    double negZed = 1;
    if (flipFlag < 0) {
      negZed = -1;
    } else {
      negZed = 1;
    }
    m_vtkImgData->SetSpacing(m_ddata.pixSpace[0], -m_ddata.pixSpace[1], negZed*zspacing);

    short* temp;
    // Copy and the data
    for (int ix1=0; ix1<imgData->count(); ix1++) {
      for (int row=0; row<imgData->at(0).numRows; row++) {
        for (int col=0; col<imgData->at(0).numCols; col++) {
          temp = (short*)m_vtkImgData->GetScalarPointer(col, row, ix1);
          *temp =  imgData->at(ix1).shortData[row*imgData->at(0).numCols+col];
        }
      }
    }

    // Reset the spacing.
    m_infoFix->SetInput(m_vtkImgData);
    m_infoFix->CenterImageOff();
    m_infoFix->SetOutputSpacing(m_ddata.pixSpace[0], m_ddata.pixSpace[1], -1*negZed*zspacing);
    m_infoFix->Update();

    m_matrix->SetElement(0, 0, rowOrient[0]);
    m_matrix->SetElement(1, 0, rowOrient[1]);
    m_matrix->SetElement(2, 0, rowOrient[2]);
    m_matrix->SetElement(0, 1, colOrient[0]);
    m_matrix->SetElement(1, 1, colOrient[1]);
    m_matrix->SetElement(2, 1, colOrient[2]);
    m_matrix->SetElement(0, 2, zVec[0]);
    m_matrix->SetElement(1, 2, zVec[1]);
    m_matrix->SetElement(2, 2, zVec[2]);

    m_matrix->SetElement(0, 3, pos[0]);
    m_matrix->SetElement(1, 3, pos[1]);
    m_matrix->SetElement(2, 3, pos[2]);
  }

  return true;
}
