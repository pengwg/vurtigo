#ifndef WRITE_ONLY_MODE_H
#define WRITE_ONLY_MODE_H

#include "genericMode.h"

// DCMTK
#include "cfunix.h"
#include "ofstring.h"

// VTK
#include "vtkTransform.h"


const int NUM_ENTRIES = 4;
const std::string ENTRY_STRINGS[NUM_ENTRIES] = {"HFS", "FFS","HFP","FFP"};


//! ENTRY_FLIPS  flip factors to convert from patient coordinate system to RCS
/*! These flip arrays can handle flipping of the axes to convert to rufous coord system,
// but won't be able to handle Left Decubitus & Right Decubitus where the patient
// axes are rotated relative to the Rufous Coordinate System axes: 
// This structure should be modified to store rotation matrices instead.
*/
const int ENTRY_FLIPS[NUM_ENTRIES][3] = {
	{-1,-1,1}, /* Head First Supine to RCS*/
	{1,-1,-1}, /* Feet First Supine to RCS */
	{1,1,1}, /* Head First Prone to RCS */
	{-1,1,-1} /* Feet First Prone to RCS */
};


// DICOM structure. NOTE: Not all values will be filled in. 
struct DICOMImageData {
  OFString patientsName;
  OFString studyDate;
  OFString studyTime;
  unsigned short numRows;
  unsigned short numCols;
  unsigned long pixelGroupLen;
  const unsigned short* shortData;
  unsigned char* charData;
  unsigned long numElements;
  unsigned short minPixVal;
  unsigned short maxPixVal;
  double pixSpace[2];
  double imgPosition[3]; // Position of img.
  double imgOrient[6]; // Direction cosines
  OFString patientPosition;
  int fov;
  vtkMatrix4x4* transform;
  float rot[9];
  float translate[3];
};




class WriteOnlyMode : public GenericMode {
 public:
  WriteOnlyMode();
  ~WriteOnlyMode();
  
  // Init the TextInterface.
  bool init(GeometrySender* geom, struct arguments* args);
  
  // Start the mode running. 
  void runMode();
 private:
  bool m_prompt;
  bool m_outputImg;
  int m_numIter;
  int m_sleep;
  int m_numPlanes;

  vector<Point3D> m_volTransPoints;
  vector<Point3D> m_exTransPoints;

  //! The number time points that we have catheter data. 
  int m_numCathPoints;
  //! The number of time points that we have vol translations.
  int m_numVolTransPoints;
  int m_numExTransPoints;
  //! The number of SETALL datasets. 
  int m_numSetAllPoints;
  //! The number of DICOM  datasets.
  int m_numDCMPoints;

  //! Use and send cath info to the geom server. 
  bool m_useCathInfo;
  bool m_useVolTransFile;
  bool m_useDICOMFile;
  bool m_useSetAllFile;
  bool m_useExTransFile;

  //! The DICOM IMAGE header.
  DICOMImageData m_ddata;

  bool setCatheterFile(std::string cathFile);
  bool setVolTransFile(std::string volFile);
  bool setExtraTransFile(std::string exFile);
  bool setSetAllFile(std::string setAllFile);
  bool setDICOMFile(std::string setDICOMFile);
};

#endif
