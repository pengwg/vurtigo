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
  int ix1;

  m_sender = geom;
 
  m_planeID = 0;
  m_numPlanes = 2;
  m_imgDataArray.clear();
  for (ix1=0; ix1<m_numPlanes; ix1++) {
    m_imgDataArray.push_back(imgData);
  }

  // A single catheter may be read in this mode.  
  m_cathDataArray.clear();
  m_cathDataArray.push_back(cathData);

  return true;
}

void ReadOnlyMode::runMode() {
  int ix1, ix2;
  char fname[256];
  int fileSize;

    // Read all the relevant planes
    for (ix2=0; ix2<m_numPlanes; ix2++) {
      m_planeID = ix2;
      getAllGeom();
    }
}

