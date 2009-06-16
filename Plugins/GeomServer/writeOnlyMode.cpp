#include "writeOnlyMode.h"

#include <string>
using namespace std;

//DCMTK
//#include "cfunix.h"
//#include "dcdeftag.h"
//#include "dcfilefo.h"
//#include "ofcond.h"
//#include "dcdatset.h"
//#include "dctagkey.h"




WriteOnlyMode::WriteOnlyMode() {
//  m_ddata.transform = vtkMatrix4x4::New();
}

WriteOnlyMode::~WriteOnlyMode() {
//  if ( m_ddata.transform) m_ddata.transform->Delete();
}

bool WriteOnlyMode::init(GeometrySender* geom, struct arguments* args) {
  m_sender = geom;

  // Setup the parameters. 
  m_numIter = args->iterations;
  m_outputImg = args->outputImages;
  m_sleep = args->sleep;
  m_prompt = args->userPrompt;
 
  // Arrays can be set up when the files are read. 
  m_planeID = 0;
  m_numPlanes = 0;
  m_imgDataArray.clear();
  m_cathDataArray.clear();


  // Set the default to send nothing. 
  m_useCathInfo = false;
  m_useVolTransFile = false;
  m_useDICOMFile = false;
  m_useSetAllFile = false;
  m_useExTransFile = false;


  m_numCathPoints = 0;
  m_numVolTransPoints = 0;
  m_numDCMPoints = 0;
  m_numSetAllPoints = 0;
  m_numExTransPoints = 0;

  args->cathFile = "dataFiles/cathMoving.txt";

  if (args->cathFile != NULL) {
    m_useCathInfo = setCatheterFile(args->cathFile) && (m_numCathPoints>0) && m_sender->initializeCath(m_cathDataArray[0].cathMode, m_cathDataArray[0].numCoils);
  }
//  if (args->volFile != NULL) {
//    m_useVolTransFile = setVolTransFile(args->volFile) && (m_numVolTransPoints>0);
//  }
//  if(args->dicomFile != NULL) {
//    m_useDICOMFile = setDICOMFile(args->dicomFile) && (m_numDCMPoints>0);
//  }
//  if(args->setAllFile !=NULL && !m_useDICOMFile) {
//    m_useSetAllFile = setSetAllFile(args->setAllFile) && (m_numSetAllPoints>0);
//  }
//  if (args->transFile != NULL && !m_useDICOMFile && !m_useSetAllFile) {
//    m_useExTransFile = setExtraTransFile(args->transFile) && (m_numExTransPoints>0);
//  }


  if (m_useCathInfo) cout << "Use Catheter with: " << m_numCathPoints << " time points." << endl;
  if (m_useVolTransFile) cout << "Use Volume Translation with: " << m_numVolTransPoints << " time points." << endl;
  if (m_useDICOMFile) cout << "Use DICOM with: " << m_numDCMPoints << " time points." << endl;
  if (m_useSetAllFile) cout << "Use SetAll with: " << m_numSetAllPoints << " time points." << endl;
  if (m_useExTransFile) cout << "Use Extra Translation with: " << m_numExTransPoints << " time points." << endl;


  return true;
}

void WriteOnlyMode::runMode() {
  int ix1;
  float temp[3];

  cout << "STARTED SENDING: " << endl;
  for (ix1=0; ix1<m_numIter; ix1++) {
    if (m_useCathInfo) {
      // There exists cath info to be sent. 
      m_sender->sendNextCath(m_cathDataArray[ix1%m_numCathPoints]);   
    }

//    if (m_useVolTransFile) {
//      temp[0] = m_volTransPoints[ix1%m_numVolTransPoints].x;
//      temp[1] = m_volTransPoints[ix1%m_numVolTransPoints].y;
//      temp[2] = m_volTransPoints[ix1%m_numVolTransPoints].z;
//      m_sender->setVolTranslation(temp);
//    }
//
//    // Only one of these options can be used at a time.
//    if (m_useDICOMFile) {
//      m_sender->sendImgData(&m_imgDataArray[ix1%m_numDCMPoints], m_planeID);
//    } else if (m_useSetAllFile){
//      m_sender->sendImgData(&m_imgDataArray[ix1%m_numSetAllPoints], m_planeID);
//    } else if (m_useExTransFile) {
//      temp[0] = m_exTransPoints[ix1%m_numExTransPoints].x;
//      temp[1] = m_exTransPoints[ix1%m_numExTransPoints].y;
//      temp[2] = m_exTransPoints[ix1%m_numExTransPoints].z;
//      m_sender->setTranslation(temp);
//    }

    // Use either the prompt or the timer.
    if (m_prompt) {
      string input;
      cout << "Hit [Enter] for next iteration ";
      getline(cin, input);
    } else {
      usleep(m_sleep * 10000);
      cout << "." << flush;
    }
  }
  cout << endl;
}




//! Set file containing catheter information
/*! This is a text file containing the number of catheter points, the number of
 catheter coils, and a list of catheter points - x, y, z, and SNR - with no other 
 separators aside from spaces/newlines.
// \param cathFile    the catheter information file
// \return true on success, false on failure
*/
bool WriteOnlyMode::setCatheterFile(string cathFile)
{
    ifstream file;
    int numCoils;
    CATHDATA elem;
    COILDATA coilElem;
    int ind, coil, ix1;

    file.open(cathFile.c_str(), ios::in);

    if (!file)
    {
        cerr << "Could not open catheter file" << cathFile << ". Continuing without catheters." << endl;
	m_numCathPoints = 0; // We have no catheter time points. 
        return false;
    }
    
    file >> m_numCathPoints; // time points
    file >> numCoils; // Number of coils.
    if (m_numCathPoints <= 0 || numCoils <= 0) { 
      cerr << "Invalid catheter info read from file. Number of time points: " << m_numCathPoints << " , number of cath coils: " << numCoils  << endl;
      m_numCathPoints = 0;
      return false; 
    }

    elem.cathMode = static_cast<int>(CATH_PROJECTION); // This is the only mode supported so far.
    elem.numCoils = numCoils;

    // Set angles to zero since we are not using them. 
    coilElem.angles[0] = 0;
    coilElem.angles[1] = 0;

    // For each time point:
    for (ind = 0; ind < m_numCathPoints; ind++) {
      // Read each coil
      elem.coils.clear();
      for (coil = 0; coil < numCoils; coil++) {
	for (ix1=0; ix1<COORDS_SIZE; ix1++) {
	  file >> coilElem.coords[ix1];
	}
	file >> coilElem.locID;
	file >> coilElem.SNR;
	elem.coils.push_back(coilElem);
      }
      // Add the time point. 
      m_cathDataArray.push_back(elem);
    }
    file.close();

    return true;

}

////! Read the file that sets a number of volume translations.
//bool WriteOnlyMode::setVolTransFile(std::string volFile) {
//  ifstream file;
//  Point3D temp;
//  int ix1;
//
//  file.open(volFile.c_str(), ios::in);
//
//  if (!file) {
//    cerr << "Volume translation file: " << volFile << " could not be opened." << endl;
//    m_numVolTransPoints = 0;
//    return false;
//  }
//
//  file >> m_numVolTransPoints;
//  m_volTransPoints.clear();
//  for (ix1=0; ix1< m_numVolTransPoints; ix1++) {
//    file >> temp.x >> temp.y >> temp.z;
//    m_volTransPoints.push_back(temp);
//  }
//
//  file.close();
//
//  return true;
//}
//
////! Read the file that sets the extra translation to give to the matrix.
//bool WriteOnlyMode::setExtraTransFile(std::string exFile) {
//  ifstream file;
//  Point3D temp;
//  int ix1;
//
//
//  file.open(exFile.c_str(), ios::in);
//
//  if (!file) {
//    cerr << "Volume translation file: " << exFile << " could not be opened." << endl;
//    m_numExTransPoints = 0;
//    return false;
//  }
//
//  file >> m_numExTransPoints;
//  m_exTransPoints.clear();
//  for (ix1=0; ix1< m_numExTransPoints; ix1++) {
//    file >> temp.x >> temp.y >> temp.z;
//    m_exTransPoints.push_back(temp);
//  }
//
//  file.close();
//
//  return true;
//}
//
//
////! Init the data for the SET_ALL command.
//bool WriteOnlyMode::setSetAllFile(std::string setAllFile) {
//  int ix1;
//  ifstream file;
//  int totalSize;
//  int temp;
//
//
//  IMAGEDATA imgDat;
//
//  // Init with no image.
//  imgDat.img = 0;
//
//  file.open(setAllFile.c_str(), ios::in);
//  if (!file) {
//    cerr << "Could not open file to use on setAll command " << setAllFile << ". Continuing without SetAll." << endl;
//    return false;
//  }
//
//  file >> imgDat.trig;
//  file >> imgDat.resp;
//
//  // Get the 3 by 3 rotation matrix.
//  for (ix1=0; ix1<9; ix1++) {
//    file >> imgDat.rotMatrix[ix1];
//  }
//
//  // Get the 3 by 1 translation vector.
//  for (ix1=0; ix1<3; ix1++) {
//    file >> imgDat.transMatrix[ix1];
//  }
//
//  file >> imgDat.FOV;
//  file >> imgDat.imgSize;
//  file >> imgDat.numChannels;
//
//  totalSize = imgDat.imgSize*imgDat.imgSize*imgDat.numChannels;
//  if (totalSize <= 0 || totalSize > 2048*2048*16) {
//    cerr << "Image Size from input file is corrupt " << totalSize << ". Continuing without SetAll." << endl;
//    return false;
//  }
//
//
//  if (!imgDat.img) imgDat.img = new unsigned char[totalSize];
//  for (ix1=0; ix1<totalSize; ix1++){
//    file >> temp;
//    imgDat.img[ix1] = (unsigned char)temp;
//  }
//
//  // So far we only have one such point.
//  m_numSetAllPoints = 1;
//  m_imgDataArray.clear();
//  m_imgDataArray.push_back(imgDat);
//
//  return true;
//}
//
//
//
////! Read a DICOM file for image data.
//bool WriteOnlyMode::setDICOMFile(std::string setDICOMFile) {
//  DcmFileFormat dcmFile;
//  OFCondition status;
//  DcmDataset* datSet;
//  unsigned int ix1;
//  unsigned int locIdx;
//
//  IMAGEDATA imgDat;
//
//  // TODO: set the trig and resp to reasonable values.
//  imgDat.trig = 0;
//  imgDat.resp = 0;
//
//  // TODO: Set this value from the dicom file.
//  imgDat.numChannels = 1;
//
//  // Load the DICOM file.
//  status = dcmFile.loadFile(setDICOMFile.c_str());
//
//  // Check that the file was loaded
//  if (status.bad()) {
//    cerr << "Error: cannot read DICOM file (" << status.text() << ")" << endl;
//    return false;
//  }
//
//  datSet = dcmFile.getDataset();
//
//  if (datSet->findAndGetOFString(DCM_PatientsName, m_ddata.patientsName).good()) {
//    cout << "Patient's Name: " << m_ddata.patientsName << endl;
//  }
//  if (datSet->findAndGetOFString(DCM_StudyDate, m_ddata.studyDate).good()) {
//    cout << "Study Date: " << m_ddata.studyDate << endl;
//  }
//  if (datSet->findAndGetOFString(DCM_StudyTime, m_ddata.studyTime).good()) {
//    cout << "Study Time: " << m_ddata.studyTime << endl;
//  }
//  if (datSet->findAndGetOFString(DCM_PatientPosition, m_ddata.patientPosition).good()) {
//    cout << "Patient Position: " << m_ddata.patientPosition << endl;
//  }
//  if (datSet->findAndGetUint16(DCM_Rows, m_ddata.numRows).good()) {
//    cout << "Number of Rows: " << m_ddata.numRows << endl;
//    imgDat.imgSize =  m_ddata.numRows; // NumRows == NumCol
//  }
//  if (datSet->findAndGetUint16(DCM_Columns, m_ddata.numCols).good()) {
//    cout << "Number of Columns: " << m_ddata.numCols << endl;
//  }
//  if (datSet->findAndGetUint32(DCM_PixelDataGroupLength, m_ddata.pixelGroupLen).good()) {
//    cout << "Pixel Group Length: " << m_ddata.pixelGroupLen << endl;
//  }
//  if (datSet->findAndGetUint16Array(DCM_PixelData, m_ddata.shortData, &m_ddata.numElements, false).good()) {
//    cout << "Read data points: "  <<  m_ddata.numElements << endl;
//  }
//
//  if (m_ddata.numElements != m_ddata.numRows* m_ddata.numCols && m_ddata.numElements > 0) {
//    cout << "Error: Problem reading all of the dicom pixel data." << endl;
//    return false;
//  }
//
//  if (datSet->findAndGetUint16(DCM_SmallestImagePixelValue, m_ddata.minPixVal).good()) {
//    cout << "Min Pixel Value: " << m_ddata.minPixVal << endl;
//  }
//  if (datSet->findAndGetUint16(DCM_LargestImagePixelValue, m_ddata.maxPixVal).good()) {
//    cout << "Max Pixel Value: " << m_ddata.maxPixVal << endl;
//  }
//
//  for (ix1=0; ix1<2; ix1++) datSet->findAndGetFloat64(DCM_PixelSpacing, m_ddata.pixSpace[ix1], ix1);
//  for (ix1=0; ix1<3; ix1++) datSet->findAndGetFloat64(DCM_ImagePositionPatient, m_ddata.imgPosition[ix1], ix1);
//  for (ix1=0; ix1<6; ix1++) datSet->findAndGetFloat64(DCM_ImageOrientationPatient, m_ddata.imgOrient[ix1], ix1);
//
//  m_ddata.charData = new unsigned char[m_ddata.numElements];
//  imgDat.img = new unsigned char[m_ddata.numElements];
//  // Transform the short array into chars.
//  for (ix1=0; ix1<m_ddata.numElements; ix1++) {
//    m_ddata.charData[ix1] = (float)m_ddata.shortData[ix1]/((float)m_ddata.maxPixVal/255.0f);
//    imgDat.img[ix1] = m_ddata.charData[ix1];
//  }
//
//  // Find the FOV.
//  m_ddata.fov = (m_ddata.pixSpace[0]*m_ddata.numRows)/10.0f;
//  imgDat.FOV = m_ddata.fov;
//
//  // Find the flips according to patient position.
//  for (ix1=0; ix1<NUM_ENTRIES ; ix1++) {
//    if (m_ddata.patientPosition == ENTRY_STRINGS[ix1].c_str()) {
//      locIdx = ix1;
//    }
//  }
//
//  // Find the rotation matrix.
//  for (ix1=0; ix1<6; ix1++) {
//    // Set the direction cosines. (AND the flips)
//    m_ddata.transform->SetElement(ix1%3, ix1/3, -1*m_ddata.imgOrient[ix1]*ENTRY_FLIPS[locIdx][ix1%3]);
//  }
//
//  // Find the third vector by doing the cross product of the first two. (USE the flipped data!!)
//  m_ddata.transform->SetElement(0, 2, m_ddata.transform->GetElement(1,0)*m_ddata.transform->GetElement(2,1)
//				- m_ddata.transform->GetElement(2,0)*m_ddata.transform->GetElement(1,1));
//  m_ddata.transform->SetElement(1, 2, m_ddata.transform->GetElement(2,0)*m_ddata.transform->GetElement(0,1)
//				- m_ddata.transform->GetElement(0,0)*m_ddata.transform->GetElement(2,1));
//  m_ddata.transform->SetElement(2, 2, m_ddata.transform->GetElement(0,0)*m_ddata.transform->GetElement(1,1)
//				- m_ddata.transform->GetElement(1,0)*m_ddata.transform->GetElement(0,1));
//
//  // Now transform the RCS to the World CS.
//  for (ix1=0; ix1<9; ix1++) {
//    if (ix1< 3 || ix1>5) {
//      m_ddata.transform->SetElement(ix1/3, ix1%3, -1*m_ddata.transform->GetElement(ix1/3,ix1%3));
//    }
//  }
//
//  for (ix1=0; ix1<9; ix1++) {
//    m_ddata.rot[ix1] = m_ddata.transform->GetElement(ix1/3, ix1%3);
//    imgDat.rotMatrix[ix1] = m_ddata.rot[ix1];
//  }
//
//  // Do the translation.
//
//  // Find the rotation matrix.
//  for (ix1=0; ix1<6; ix1++) {
//    // Set the direction cosines. (AND "some" of the flips)
//    m_ddata.transform->SetElement(ix1%3, ix1/3, m_ddata.imgOrient[ix1]*ENTRY_FLIPS[locIdx][ix1%3]);
//  }
//
//  // [upperLeft] + rowSp*(rows-1)/2 * [rowDir] + colSp*(cols-1)/2 * [colDir]
//  float rowFac = (m_ddata.pixSpace[0]*(m_ddata.numRows-1)/2.0f);
//  float colFac = (m_ddata.pixSpace[1]*(m_ddata.numCols-1)/2.0f);
//  for (ix1=0; ix1<3; ix1++) {
//    m_ddata.translate[ix1] =  m_ddata.imgPosition[ix1]*ENTRY_FLIPS[locIdx][ix1%3]
//      +rowFac*m_ddata.transform->GetElement(ix1%3, 0)
//      +colFac*m_ddata.transform->GetElement(ix1%3, 1);
//  }
//
//  // Change to World.
//  m_ddata.translate[0] = -1*m_ddata.translate[0];
//  m_ddata.translate[2] = -1*m_ddata.translate[2];
//
//  // Set the translation
//  for (ix1=0; ix1<3; ix1++) {
//    imgDat.transMatrix[ix1] = m_ddata.translate[ix1];
//  }
//
//  // So far we only have one such point.
//  m_numDCMPoints = 1;
//  m_imgDataArray.clear();
//  m_imgDataArray.push_back(imgDat);
//
//  return true;
//}
