#include <iostream>
#include <fstream>

#include "readOnlyMode.h"

using namespace std;

ReadOnlyMode::ReadOnlyMode() {
}

ReadOnlyMode::~ReadOnlyMode() {
}

bool ReadOnlyMode::init(GeometrySender* geom, struct arguments* args) {
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

void ReadOnlyMode::runMode() {
    // Read all the relevant planes
    for (int ix1=0; ix1<m_numPlanes; ix1++) {
      m_planeID = ix1;
      receivePlane(m_planeID);
    }
    receiveCatheter();
}

