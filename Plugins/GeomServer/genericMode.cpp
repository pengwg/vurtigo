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
#include "genericMode.h"
#include <iostream>
#include <cstring>
#include <sstream>

#include <QTime>

#include "Converter.h"
#include "rt2dSliceDataObject.h"
#include "rtMessage.h"
#include "rtApplication.h"
#include "rtTimeManager.h"

using namespace std;

GenericMode::GenericMode():m_sender(0), m_zeroCathOnly(false) {
  converter = new Converter();

  m_imgDataArray.clear();
  m_cathDataArray.clear();
}

GenericMode::~GenericMode() {

  delete converter;
}


bool GenericMode::init(GeometrySender* geom, struct arguments* args) {
  m_sender = geom;

  m_planeID = 0;
  m_planeList.clear();
  m_cathList.clear();
  m_imgDataArray.clear();
  m_cathDataArray.clear();

  // Set one resource for each.
  m_planeListLock.release(1);
  m_cathListLock.release(1);

  return true;
}

void GenericMode::runMode() {
  IMAGEDATA imgData;
  CATHDATA cathData;
  rt2DSliceDataObject * localImage;
  rtCathDataObject * localCath;
  QTime tt;

  // Nothing to read.
  if (m_planeList.count() == 0 && m_cathList.count() == 0) return;

  imgData.img=NULL;

  m_planeListLock.acquire();

  tt.start();
  // Re-size as is needed.
  while( m_imgDataArray.size() < ((unsigned int)m_planeList.size()) ) {
    imgData.arraySize = 0;
    imgData.img = NULL;
    m_imgDataArray.push_back(imgData);
  }

  QString resizeTime;
  resizeTime.append("Re-size image data array: ");
  resizeTime.append(QString::number(tt.elapsed()));
  rtApplication::instance().getMessageHandle()->bench(resizeTime);

  /*
  std::stringstream msg;
  msg << "Re-size image data array: " << tt.elapsed();
  rtApplication::instance().getMessageHandle()->bench(msg.str());
*/

  // Get the trigger time.
  int triggerT = m_sender->getTrig();
  int respR = m_sender->getResp();

  // Set the trigger delay for the application.
  rtApplication::instance().getTimeManager()->setGlobalTriggerDelay(triggerT);
  rtApplication::instance().getTimeManager()->setGlobalResp(respR);

  // Read all the relevant planes
  for (int ix1=0; ix1<m_planeList.count(); ix1++) {
    QString planeTime;
    planeTime.clear();
    planeTime.append("Image Plane with ID: ");
    planeTime.append(QString::number(m_planeID));
    planeTime.append(" takes ");

    /*
    std::stringstream msg;
    msg << "Image Plane with ID: " << m_planeID << " takes ";
    */
    tt.restart();
    m_planeID = ix1;
    if (m_planeList[ix1].act == SenderThread::OBJ_READ) {
      receivePlaneAndPosition(m_planeID);

      //msg << " plane and position received: " << tt.elapsed();
      planeTime.append(" plane and position received: ");
      planeTime.append(QString::number(tt.elapsed()));

      localImage = converter->getLocalImage(ix1, m_imgDataArray[ix1].imgSize * m_imgDataArray[ix1].imgSize * m_imgDataArray[ix1].numBytesPerPixel);
      if (!localImage) continue;
      converter->setLocalImage(m_imgDataArray[ix1], localImage);
    } else if(m_planeList[ix1].act == SenderThread::OBJ_WRITE) {
      // Set to remote if needed.
      converter->setRemoteImageTransform(this->getImages());
      receivePlane(m_planeID);
      sendPlanePosition(m_planeID);

      //msg << " plane received and position sent: " << tt.elapsed();
      planeTime.append(" plane received and position sent: ");
      planeTime.append(QString::number(tt.elapsed()));

      localImage = converter->getLocalImage(ix1, m_imgDataArray[ix1].imgSize * m_imgDataArray[ix1].imgSize * m_imgDataArray[ix1].numBytesPerPixel);
      if (!localImage) continue;
      converter->setLocalImageOnly(m_imgDataArray[ix1], localImage);
    } else if (m_planeList[ix1].act == SenderThread::OBJ_IGNORE) {
      // Do nothing
    } else if (m_planeList[ix1].act == SenderThread::OBJ_ERROR) {
      // Do nothing
    }

    /*
    msg << " final: " << tt.elapsed();
    rtApplication::instance().getMessageHandle()->bench(msg.str());
    */
    planeTime.append(" final: ");
    planeTime.append(QString::number(tt.elapsed()));
    rtApplication::instance().getMessageHandle()->bench(planeTime);
  }

  m_planeListLock.release();

  m_cathListLock.acquire();

  while(m_cathDataArray.size() < m_cathList.count()) {
    m_cathDataArray.push_back(cathData);
  }

  for (int ix1=0; ix1<m_cathList.count(); ix1++) {
    tt.restart();
    if (m_cathList[ix1].act == SenderThread::OBJ_READ) {

      // Get the catheter is required.
      if ( (m_zeroCathOnly && ix1 == 0) || !m_zeroCathOnly) {
        receiveCatheter(ix1);
      }

      if (m_zeroCathOnly) {
        // Only catheter zero is used for ALL instances of the catheter in Vurtigo.
        if (m_cathDataArray.at(0).cathMode != NO_CATHETERS) {
          localCath = converter->getLocalCath(ix1);
          converter->setLocalCath(m_cathDataArray[0], localCath);
        }
      } else {
        // Each catheter is different.
        if (m_cathDataArray.at(ix1).cathMode != NO_CATHETERS) {
          localCath = converter->getLocalCath(ix1);
          converter->setLocalCath(m_cathDataArray[ix1], localCath);
        }
      }
    } else if(m_cathList[ix1].act == SenderThread::OBJ_WRITE) {
      // Do nothing
    } else if (m_cathList[ix1].act == SenderThread::OBJ_IGNORE) {
      // Do nothing
    } else if (m_cathList[ix1].act == SenderThread::OBJ_ERROR) {
      // Do nothing
    }

    QString cathTime;
    cathTime.clear();
    cathTime.append("Catheter with ID: ");
    cathTime.append(QString::number(ix1));
    cathTime.append(" takes ");
    cathTime.append(QString::number(tt.elapsed()));
    rtApplication::instance().getMessageHandle()->bench(cathTime);
    /*
    std::stringstream msg;
    msg << "Catheter with ID: " << ix1 << " takes " << tt.elapsed();
    rtApplication::instance().getMessageHandle()->bench(msg.str());
    */
  }
  m_cathListLock.release();

}

bool GenericMode::sendPlanePosition(int id) {
  if (id < 0) return false;

  IMAGEDATA* currImg;
  float *rotation, *translation;

  rotation = new float[ROT_MATRIX_SIZE];
  translation = new float[TRANS_MATRIX_SIZE];

  currImg = &m_imgDataArray[m_planeID];
  for (int ix1=0; ix1<ROT_MATRIX_SIZE && rotation; ix1++) {
    rotation[ix1] = currImg->rotMatrix[ix1];
  }

  for (int ix1=0; ix1<TRANS_MATRIX_SIZE && translation; ix1++) {
    translation[ix1] = currImg->transMatrix[ix1];
  }

  m_sender->setPlaneID(m_planeID);
  m_sender->setRotation(rotation);
  m_sender->setTranslation(translation);

  delete[] rotation;
  delete[] translation;

  return true;
}

bool GenericMode::receivePlanePosition(int id) {
  if (id < 0) return false;

  IMAGEDATA* currImg;
  float *rotation, *translation;

  m_planeID = id;
  currImg = &m_imgDataArray[m_planeID];

  m_sender->setPlaneID(m_planeID);
  rotation = m_sender->getImgRotation();
  translation = m_sender->getImgTranslation();

  for (int ix1=0; ix1<ROT_MATRIX_SIZE && rotation; ix1++) {
    currImg->rotMatrix[ix1] = rotation[ix1];
  }

  for (int ix1=0; ix1<TRANS_MATRIX_SIZE && translation; ix1++) {
    currImg->transMatrix[ix1] = translation[ix1];
  }

  return true;
}

bool GenericMode::receivePlane(int id) {
  IMAGEDATA* currImg;
  unsigned char * image;
  bool res = false;

  // The gometry plane.
  m_planeID = id;
  m_sender->setPlaneID(m_planeID);
  currImg = &m_imgDataArray[m_planeID];
  currImg->imgSize = m_sender->getImgSize();
  currImg->numChannels = m_sender->getNumChan();
  currImg->numBytesPerPixel = m_sender->getNumBytesPerPixel();

  // Check the image size and the number of channels to see if this makes sense.
  if (currImg->imgSize > 0 && currImg->imgSize <= 2048 && currImg->numChannels > 0 && currImg->numChannels <= 16) {

    currImg->trig = m_sender->getTrig();
    currImg->resp = m_sender->getResp();
    currImg->FOV = m_sender->getFOV();
    image =  m_sender->getImage();

    if (image) {
      int imageSize = currImg->imgSize * currImg->imgSize * currImg->numBytesPerPixel;

      // Check if a resize is needed.
      if (currImg->arraySize == 0 || currImg->arraySize != imageSize) {
        if (currImg->img != NULL)
          delete currImg->img;
        currImg->img = new unsigned char[imageSize];
        currImg->arraySize = imageSize;
      }

      memcpy(currImg->img, image, imageSize);
      res=true;
    }
  }
  return res;
}


bool GenericMode::receivePlaneAndPosition(int id) {
  if (id < 0) return false;

  IMAGEDATA* currImg;
  float *rotation, *translation;
  unsigned char * image;
  bool res = false;

  m_planeID = id;
  currImg = &m_imgDataArray[m_planeID];
  m_sender->setPlaneID(m_planeID);

  // Get the plane.
  currImg->imgSize = m_sender->getImgSize();
  currImg->numChannels = m_sender->getNumChan();
  currImg->numBytesPerPixel = m_sender->getNumBytesPerPixel();

  // Check the image size and the number of channels to see if this makes sense.
  if (currImg->imgSize > 0 && currImg->imgSize <= 2048 && currImg->numChannels > 0 && currImg->numChannels <= 16) {

    currImg->trig = m_sender->getTrig();
    currImg->resp = m_sender->getResp();
    currImg->FOV = m_sender->getFOV();
    image =  m_sender->getImage();

    if (image) {
      int imageSize = currImg->imgSize * currImg->imgSize * currImg->numBytesPerPixel;

      // Check if a resize is needed.
      if (currImg->arraySize == 0 || currImg->arraySize != imageSize) {
        if (currImg->img != NULL)
          delete currImg->img;
        currImg->img = new unsigned char[imageSize];
        currImg->arraySize = imageSize;
      }

      memcpy(currImg->img, image, imageSize);
      res=true;
    }
  }

  // Get the plane position.
  rotation = m_sender->getImgRotation();
  translation = m_sender->getImgTranslation();

  if (!rotation || !translation) {
    res = false;
  } else {
    for (int ix1=0; ix1<ROT_MATRIX_SIZE; ix1++) {
      currImg->rotMatrix[ix1] = rotation[ix1];
    }

    for (int ix1=0; ix1<TRANS_MATRIX_SIZE; ix1++) {
      currImg->transMatrix[ix1] = translation[ix1];
    }
  }
  return res;
}


bool GenericMode::receiveCatheter(int id) {
  CATHDATA* currCath;
  COILDATA coilDat;
  float *coords;
  int *angles;
  bool res = false;

    // Always use the first catheter.
  currCath = &m_cathDataArray[id];

  //QTime temp;
  //temp = QTime::currentTime();
  //std::cout << "Sec: " << temp.second() << "." << temp.msec() << std::endl;

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

void GenericMode::setPlaneList(QList<SenderThread::ListObject>  plane) {
  m_planeListLock.acquire();
  m_planeList.clear();
  for (int ix1=0; ix1<plane.count(); ix1++) {
    m_planeList.append(plane[ix1]);
  }
  m_planeListLock.release();
}

void GenericMode::setCathList(QList<SenderThread::ListObject> cath) {
  m_cathListLock.acquire();
  m_cathList.clear();
  for (int ix1=0; ix1<cath.count(); ix1++) {
    m_cathList.append(cath[ix1]);
  }
  m_cathListLock.release();
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
    cout << "Number of bytes per pixel = " << m_imgDataArray[ix1].numBytesPerPixel << endl;
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
