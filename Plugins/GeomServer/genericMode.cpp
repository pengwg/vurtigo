#include "genericMode.h"
#include <iostream>
#include <cstring>

using namespace std;

GenericMode::GenericMode():m_sender(0) {
  int ix1=0; 

  m_imgDataArray.clear();
  m_cathDataArray.clear();
}

GenericMode::~GenericMode() {
}

void GenericMode::print() {
  int ix1;

  cout << "-------------------------------- START PRINT" << endl;

  cout << "PlaneID: " << m_planeID << endl;

  for (ix1=0; ix1<m_imgDataArray.size(); ix1++) {
    cout << "-- Plane with ID: "  << ix1 << endl;
    cout << "Respiratory phase: " <<  m_imgDataArray[ix1].resp << endl;
    cout << "Trigger delay: " << m_imgDataArray[ix1].trig << endl;
    cout << "FOV: " << m_imgDataArray[ix1].FOV << " cm"  << endl;
    cout << "Rotation: [" << m_imgDataArray[ix1].rotMatrix[0] << " " << m_imgDataArray[ix1].rotMatrix[1] << " " << m_imgDataArray[ix1].rotMatrix[2] << " ]" << endl;
    cout << "          [" << m_imgDataArray[ix1].rotMatrix[3] << " " << m_imgDataArray[ix1].rotMatrix[4] << " " << m_imgDataArray[ix1].rotMatrix[5] << " ]" << endl;
    cout << "          [" << m_imgDataArray[ix1].rotMatrix[6] << " " << m_imgDataArray[ix1].rotMatrix[7] << " " << m_imgDataArray[ix1].rotMatrix[8] << " ]" << endl;
    cout << "Translation: [" << m_imgDataArray[ix1].transMatrix[0] << " " << m_imgDataArray[ix1].transMatrix[1] << " " << m_imgDataArray[ix1].transMatrix[2] << " ]" << endl;
    cout << "Image size = " << m_imgDataArray[ix1].imgSize << endl;
    cout << "Number of image color channels = " << m_imgDataArray[ix1].numChannels << endl;
  }

  for (ix1=0; ix1<m_cathDataArray.size(); ix1++) {
    cout << "Number of catheter coils: " << m_cathDataArray[ix1].numCoils << endl;
    cout << "Catheter mode: " << m_cathDataArray[ix1].cathMode << endl;
    if (NO_CATHETERS != static_cast<CatheterMode>(m_cathDataArray[ix1].cathMode)) {
      for (int coil = 0; coil < m_cathDataArray[ix1].numCoils; coil++) {
	cout << "Cath coil = " << coil <<" locID = " << m_cathDataArray[ix1].coils[coil].locID << " SNR = " << m_cathDataArray[ix1].coils[coil].SNR <<  endl;
	cout << "Coords: [" <<  m_cathDataArray[ix1].coils[coil].coords[0] << ", " 
	     <<  m_cathDataArray[ix1].coils[coil].coords[1] << ", " 
	     <<  m_cathDataArray[ix1].coils[coil].coords[2] << "]" << endl;
	cout << "Angles: [" << m_cathDataArray[ix1].coils[coil].angles[0] << ", " 
	     <<  m_cathDataArray[ix1].coils[coil].angles[1] << "]" << endl;
      } 
    }
  }
  cout << "-------------------------------- END PRINT" << endl;
}


bool GenericMode::init(GeometrySender* geom, struct arguments* args) {
  IMAGEDATA imgData;
  CATHDATA cathData;

  m_sender = geom;

  m_planeID = 0;
  m_numPlanes = 2;
  m_imgDataArray.clear();
  imgData.img = NULL;
  for (int ix1=0; ix1<m_numPlanes; ix1++) {
    m_imgDataArray.push_back(imgData);
  }

  // A single catheter may be read in this mode.
  m_cathDataArray.clear();
  m_cathDataArray.push_back(cathData);

  return true;
}

void GenericMode::runMode() {
    // Read all the relevant planes
    for (int ix1=0; ix1<m_numPlanes; ix1++) {
      m_planeID = ix1;
      receivePlane(m_planeID);
    }
    receiveCatheter();
}



bool GenericMode::receivePlane(int id) {
  IMAGEDATA* currImg;
  float *rotation, *translation;
  unsigned char * image;
  int ix1;
  bool res = false;

  // The gometry plane.
  m_planeID = id;
  m_sender->setPlaneID(m_planeID=id);
  currImg = &m_imgDataArray[m_planeID];
  currImg->imgSize = m_sender->getImgSize();
  currImg->numChannels = m_sender->getNumChan();

  // Check the image size and the number of channels to see if this makes sense.
  if (currImg->imgSize > 0 && currImg->imgSize <= 2048 && currImg->numChannels > 0 && currImg->numChannels <= 16) {

    currImg->trig = m_sender->getTrig();
    currImg->resp = m_sender->getResp();
    currImg->FOV = m_sender->getFOV();
    rotation = m_sender->getImgRotation();
    translation = m_sender->getImgTranslation();
    image =  m_sender->getImage();

    if (currImg->img != NULL)
      delete currImg->img;

    int imageSize = currImg->imgSize * currImg->imgSize * currImg->numChannels;
    currImg->img = new unsigned char[imageSize];
    for (ix1=0; ix1<imageSize && image; ix1++) {
      currImg->img[ix1] = image[ix1];
    }

    for (ix1=0; ix1<ROT_MATRIX_SIZE && rotation; ix1++) {
      currImg->rotMatrix[ix1] = rotation[ix1];
    }

    for (ix1=0; ix1<TRANS_MATRIX_SIZE && translation; ix1++) {
      currImg->transMatrix[ix1] = translation[ix1];
    }
    res=true;
  }
  return res;
}

bool GenericMode::receiveCatheter() {
  CATHDATA* currCath;
  COILDATA coilDat;
  float *coords;
  int *angles;
  bool res = false;

    // Always use the first catheter.
  currCath = &m_cathDataArray[0];

  // Get catheter information.
  currCath->cathMode = m_sender->getCathModeAsInt();
  if (NO_CATHETERS != static_cast<CatheterMode>(currCath->cathMode)) {
    currCath->numCoils = m_sender->getNumCathCoils();
    currCath->coils.clear();
    for (int ix1=0; ix1<currCath->numCoils; ix1++) {
      coilDat.locID = m_sender->getCathLocID(ix1);
      coilDat.SNR = m_sender->getCathSNR(ix1);
      coords = m_sender->getCathCoords(ix1);
      angles = m_sender->getCathAngles(ix1);

      memcpy(coilDat.coords, coords, COORDS_SIZE*sizeof(float));
      memcpy(coilDat.angles, angles, ANGLES_SIZE*sizeof(float));

      currCath->coils.push_back(coilDat);
    }
    // We have a catheter if we have at least one coil.
    if (currCath->numCoils > 0)  res = true;
  }

  return res;
}

