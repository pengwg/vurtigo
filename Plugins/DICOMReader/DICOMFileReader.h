#ifndef DICOMFILEREADER_H
#define DICOMFILEREADER_H

#include <QList>
#include <QString>

#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkImageData.h"
#include "vtkImageChangeInformation.h"
#include "vtkSmartPointer.h"
#include "vtkMatrixToLinearTransform.h"

// DCMTK
#include "cfunix.h"
#include "ofstring.h"

class DICOMFileReader
{
public:
  DICOMFileReader();
  ~DICOMFileReader();

  struct DICOMImageData {
    OFString patientsName;
    OFString studyDate;
    OFString studyTime;
    OFString patientPosition;
    unsigned short numRows;
    unsigned short numCols;
    unsigned long pixelGroupLen;
    unsigned short* shortData;
    unsigned long numElements;
    double pixSpace[2];
    double imgPosition[3]; // Position of img.
    double imgOrient[6]; // Direction cosines
    int fov; // FOV is only valid for square datasets.
    double sliceThickness;
  };


  bool setDirectory(QString dirPath);
  bool readFile(QString fName);
  bool createVolume(QList<DICOMImageData>* imgData);

  vtkImageData* getImageData() { return m_infoFix->GetOutput(); }
  vtkTransform* getTransform() { return m_transform; }

protected:
  QList<QString> m_fileList;
  QList<DICOMImageData> m_imgData;

  //! The vtk pipeline ready version of the image data.
  vtkSmartPointer<vtkImageData> m_vtkImgData;

  //! Matrix for the transform.
  vtkSmartPointer<vtkMatrix4x4> m_matrix;

  //! The object the converts between the vtkMatrix4x4 and the vtkTransform.
  vtkSmartPointer<vtkMatrixToLinearTransform> m_matrixToTransform;

  //! The transform for the data.
  vtkSmartPointer<vtkTransform> m_transform;

  vtkSmartPointer<vtkImageChangeInformation> m_infoFix;

  //! The DICOM IMAGE header.
  DICOMImageData m_ddata;
};

#endif // DICOMFILEREADER_H
