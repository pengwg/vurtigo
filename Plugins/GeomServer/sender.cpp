// C++
#include <iostream>        
#include <fstream>
#include <strstream>

// Internal
#include "sender.h"
#include "geom_client.h"

using namespace std;

//! Default constructor
GeometrySender::GeometrySender(): m_geomClient(0), m_clientID(0)
{
  m_geomClient = new GeomClient();
}

//! Destructor
GeometrySender::~GeometrySender()
{
  if (m_geomClient) delete m_geomClient;
}

bool GeometrySender::connect(char* hostname, int port, bool swap){
	
  if (m_clientID > 0) {
    // Already connected. 
    cout << "Client: Already connected with ID: " << m_clientID << endl;
    return true; 
  }

  m_clientID=m_geomClient->connect(hostname,port,swap);
  if (m_clientID > 0) {
    cout << "Client: Connected with ID: " << m_clientID << endl;
  }
  return m_clientID > 0;
}

void GeometrySender::disconnect() {
  m_clientID = 0;
  if (m_geomClient) m_geomClient->disconnect();
}


//! Initialize catheter mode on geometry server
/*! 
// \pre geometry client information must have been set already
// \pre must have successfully called setCatheterFile
// \return true on success, false on failure
*/
bool GeometrySender::initializeCath(int cathMode, int numCoils)
{
    if (!m_geomClient || m_clientID < 0)
    {
        cerr << "initializeCath(): geometry client has not been set" << endl;
        return false;
    }

    m_geomClient->setCathMode(m_clientID, cathMode);
    m_geomClient->setNumCathCoils(m_clientID, numCoils);
    return true;

}

//! Send next catheter point
/*! Sends all catheter coordinate & snr info for next time point
// \pre initializeCath must have been successfully called
// \return true on success, false on failure
*/
bool GeometrySender::sendNextCath(CATHDATA cDat)
{   
  int coil;
  COILDATA * coilDat;

  for (coil = 0; coil < cDat.numCoils; coil++) {
    coilDat = &(cDat.coils[coil]);
    m_geomClient->setAllCath(m_clientID, coilDat->coords, coilDat->angles, coilDat->locID, coilDat->SNR, coil);
  }
  return true;

}

bool GeometrySender::sendImgData(IMAGEDATA *imgDat, int planeID) {
  if (!imgDat) {
    cerr << "sendImgData failed! Data pointer is NULL." << endl;
    return false;
  } 

  if (!imgDat->img) {
    cerr << "sendImgData failed! Image pointer is NULL." << endl;
  }

  m_geomClient->setPlaneID(planeID);
  m_geomClient->setAll(m_clientID, imgDat->trig, imgDat->resp, imgDat->rotMatrix, imgDat->transMatrix, imgDat->FOV, imgDat->imgSize, imgDat->img, imgDat->numChannels);

  return true;
}


bool GeometrySender::setPlaneID(int planeID)
{  
  m_geomClient->setPlaneID(planeID);
  return true;
}

bool GeometrySender::setFov(int fov)
{
  m_geomClient->setFOV(m_clientID, fov);
  return true;
}
  
bool GeometrySender::setTranslation(float* trans)
{
  m_geomClient->setTranslation(m_clientID, trans);
  return true;
}

bool GeometrySender::setVolTranslation(float* trans)
{
  m_geomClient->setVolumeTranslation(m_clientID, trans);
  return true;
}

bool GeometrySender::getVolTranslation(float*& trans)
{
  trans = m_geomClient->getVolumeTranslation(); 
  return true;
}


bool GeometrySender::xRotation(float angle)
{
  (void)angle;
  cout << "Not yet implemented" << endl;
  return true;
}

bool GeometrySender::yRotation(float angle)
{
  (void)angle;
  cout << "Not yet implemented" << endl;
  return true;
}

bool GeometrySender::zRotation(float angle)
{
  (void)angle;
  cout << "Not yet implemented" << endl;
  return true;
}

bool GeometrySender::setRotation(float* rotation)
{
  m_geomClient->setRotation(m_clientID, rotation);
  return true;
}

bool GeometrySender::setAxial()
{
  cout << "Not yet implemented" << endl;
  return true;
}

bool GeometrySender::setSagittal() {
  cout << "Not yet implemented" << endl;
  return true;
}

bool GeometrySender::setCoronal() {
  cout << "Not yet implemented" << endl;
  return true;
}

void GeometrySender::setTrigDelay(int trig) {
  m_geomClient->setTrigDelay(m_clientID, trig);
}
void GeometrySender::setRespPhase(int phase) {
  m_geomClient->setRespPhase(m_clientID, phase);
}

unsigned char* GeometrySender::getImage() {
  return m_geomClient->getImage();
}


