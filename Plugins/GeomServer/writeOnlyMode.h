#ifndef WRITE_ONLY_MODE_H
#define WRITE_ONLY_MODE_H

#include "genericMode.h"

// DCMTK
//#include "cfunix.h"
//#include "ofstring.h"

// VTK
#include "vtkTransform.h"

//// DICOM structure. NOTE: Not all values will be filled in.
//struct DICOMImageData {
//  OFString patientsName;
//  OFString studyDate;
//  OFString studyTime;
//  unsigned short numRows;
//  unsigned short numCols;
//  unsigned long pixelGroupLen;
//  const unsigned short* shortData;
//  unsigned char* charData;
//  unsigned long numElements;
//  unsigned short minPixVal;
//  unsigned short maxPixVal;
//  double pixSpace[2];
//  double imgPosition[3]; // Position of img.
//  double imgOrient[6]; // Direction cosines
//  OFString patientPosition;
//  int fov;
//  vtkMatrix4x4* transform;
//  float rot[9];
//  float translate[3];
//};
class WriteOnlyMode : public GenericMode {
 public:
  WriteOnlyMode();
  ~WriteOnlyMode();
  
  // Init the TextInterface.
  bool init(GeometrySender* geom, struct arguments* args);
  
  // Start the mode running. 
  void runMode();
 private:
  int m_numPlanes;

//  vector<Point3D> m_volTransPoints;
//  vector<Point3D> m_exTransPoints;

  //! The number time points that we have catheter data. 
  int m_numCathPoints;
  //! The number of DICOM  datasets.
  int m_numDCMPoints;

  //! Use and send cath info to the geom server. 
  bool m_useCathInfo;
  bool m_useDICOMFile;

//  //! The DICOM IMAGE header.
//  DICOMImageData m_ddata;

  bool setCatheterFile(std::string cathFile);
//  bool setDICOMFile(std::string setDICOMFile);
};

#endif
