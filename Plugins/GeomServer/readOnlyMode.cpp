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

  m_numIter = args->iterations;
  m_outputImg = args->outputImages;
  m_sleep = args->sleep;
  m_prompt = args->userPrompt;
 
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

  for (ix1=0; ix1<m_numIter; ix1++) {

    // Read all the relevant planes
    for (ix2=0; ix2<m_numPlanes; ix2++) {
      m_planeID = ix2;
      getAllGeom();
    }
    // Print out the data. 
    print();

    // Read image then output to file. 
    if (m_outputImg && 
	m_imgDataArray[m_planeID].imgSize>0 && 
	m_imgDataArray[m_planeID].numChannels>0 ) {

      fileSize = m_imgDataArray[m_planeID].imgSize * m_imgDataArray[m_planeID].imgSize * m_imgDataArray[m_planeID].numChannels;
      m_imgDataArray[m_planeID].img = m_sender->getImage();
      sprintf(fname,"geom-%d.im",ix1);
      fstream of(fname,ios::out);
      of.write((char*)m_imgDataArray[m_planeID].img, fileSize);
      of.close();      
    }

    // Use either the prompt or the timer.
    if (m_prompt) {
      string input;
      cout << "Hit [Enter] for next iteration ";
      getline(cin, input);
    } else {
      usleep(m_sleep * 1000);
    }

  }

}

