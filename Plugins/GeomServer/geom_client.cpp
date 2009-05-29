// geom_client.cpp
//
// Class for client connection to a geometry server for sharing of
// geometry information between applications.
// 
// Jeff Stainsby, 7Nov03
//

// add multiple plane 
// Nick May 05
//
// Change interface and add exception handling
// Jingyuan June 2006


#include "geom_client.h"

#include <QTcpSocket>
#include <QDataStream>
#include <QMutex>
#include <QString>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


using namespace std;

// Constructor
GeomClient::GeomClient () 
: m_geomXfer(0), m_image(0) {
    m_geomSocket = new QTcpSocket();
    m_commLock = new QMutex();
    m_eState = NoError;
}

// Destructor
GeomClient::~GeomClient () {
  // Remove the image data (if any)
  if (m_image) {
    delete [] m_image; 
    m_image = 0;
  }
  
  // Delete the data stream.
  if (m_geomXfer) {
    delete m_geomXfer; 
    m_geomXfer = 0; // in case disconnect is not called
  }

  // Delete the connection
  if (m_geomSocket) {
    delete m_geomSocket; 
    m_geomSocket = 0;
  }

  // Try to get the lock.
#if QT_VERSION >= 0x040300
  // Wait a max of 1 sec to get the lock. 
  if (m_commLock->tryLock(1000)) {
#else
  if (m_commLock->tryLock()) {
#endif
    // The lock has been obtained.
    // Release the lock.
    m_commLock->unlock();
    // Delete the lock
    delete m_commLock;
  }

}

// connect
//
// Connects to a geometry server on the specified _geomClient->setAllhostname and port
// and uses (1) or doesn't use (0) network byte swapping based on
// the value of the swap parameter
  int GeomClient::connect(const char *hostname, int port, bool swap) {
  qint32 message_size=0;

  // Lock.
  m_commLock->lock();

  // Create a new client socket connection (from RTHawk lib)
  //qRegisterMetaType<quint16>("quint16"); 
  //qRegisterMetaType<QIODevice::OpenMode>("OpenMode"); 
  m_geomSocket->connectToHost(QString(hostname), port);
  m_geomXfer = new QDataStream(m_geomSocket);

  if (!m_geomSocket->waitForConnected(2000)) {
    cout << "Socket Failed to Connect. " << endl;
    cout << m_geomSocket->error() << endl;
    m_commLock->unlock();
    return -1;
  }
  
  m_geomXfer->setByteOrder(QDataStream::LittleEndian);
  // Send known number to check swapness ...
  message_size = SWAP_TEST;
  (*m_geomXfer) << message_size;
  m_geomSocket->flush();
  (*m_geomXfer) << message_size;
  m_geomSocket->flush();
  m_geomSocket->waitForBytesWritten(2000);
  // If we were successful get the unique client ID that should
  // now be available from the server
  // waitForReadyRead is broken in earlier versions of Qt. 
  while (m_geomSocket->bytesAvailable() < sizeof(qint32)){
#if QT_VERSION >= 0x040300
    m_geomSocket->waitForReadyRead(2000);
#else
    QCoreApplication::processEvents();
#endif
  }

  (*m_geomXfer) >> message_size;

  // Release.
  m_commLock->unlock();

  return (int)message_size;
}

void GeomClient::disconnect() {

  m_commLock->lock();   
  try {
    if (m_geomXfer) {
      delete m_geomXfer; 
      m_geomXfer = 0;
    }
    
  } catch (...) {
    cerr << "Disconnecting from geometry server failed." << endl;
    throw;
  }
  m_commLock->unlock();

}

// ############################################# //
// ### Functions to set geometry information ### //
// ############################################# //

// setAll
//
// Function to set/update all currently supported geometry values
void GeomClient::setAll(int clientID, int trig, int resp, float* rot, float* trans, 
                       int fov, int size, unsigned char* img, int numChannels) {
  // NOTE: clientID is ignored for now but could be used later for
  // allow/deny update permissions
  qint32 message_size;

  m_commLock->lock();
  // First tell the server we are doing a set for all variables
  (*m_geomXfer) << (qint32)SET_ALL;
  
  // Send the client ID
  (*m_geomXfer) << (qint32)clientID;
  // Send the cardiac trigger delay
  (*m_geomXfer) << (qint32)trig;
  // Send the respiratory phase
  (*m_geomXfer) << (qint32)resp;
  // Send the rotation matrix
  message_size = ROT_MATRIX_SIZE*sizeof(long);
  m_geomXfer->writeRawData((char*)rot,message_size);
  // Send translation vector
  message_size = TRANS_MATRIX_SIZE*sizeof(long);
  m_geomXfer->writeRawData((char*)trans,message_size);
  // Send FOV
  (*m_geomXfer) << (qint32)fov;
  // Send image size
  (*m_geomXfer) << (qint32)(size*size);
  // Send num of channels in each pixel
  (*m_geomXfer) << (qint32)numChannels;
  // Send the actual image
  message_size = size * size * numChannels;
  m_geomXfer->writeRawData((char*)img,message_size);
  m_geomSocket->flush();
  m_geomSocket->waitForBytesWritten(2000);  

  m_commLock->unlock();

}

// setRotation
//
// Function to set/update rotation matrix values. Also updates clientID
// to keep track of the last client to update these values
void GeomClient::setRotation(int clientID, float* rot) {

  m_commLock->lock();
  // First tell the server we are doing a set for all variables
  (*m_geomXfer) << (qint32)SET_ROTATION;  
  // Send the client ID
  (*m_geomXfer) << (qint32)clientID;
  // Send the rotation matrix
  m_geomXfer->writeRawData((char*)rot, ROT_MATRIX_SIZE*sizeof(long));
  m_geomSocket->flush();
  m_geomSocket->waitForBytesWritten(2000);
  m_commLock->unlock();
}

// setTranslation
//
// Function to set/update translation matrix values. Also updates clientID
// to keep track of the last client to update these values
void GeomClient::setTranslation(int clientID, float* trans) {

  m_commLock->lock();
  // First tell the server we are doing a set for all variables
  (*m_geomXfer) << (qint32)SET_TRANSLATION;
  // Send the client ID
  (*m_geomXfer) << (qint32)clientID;
  // Send the translation matrix
  m_geomXfer->writeRawData((char*)trans, TRANS_MATRIX_SIZE*sizeof(float));
  m_geomSocket->flush();
  m_geomSocket->waitForBytesWritten(2000);
  m_commLock->unlock();
}

// setFOV
//
// Function to set/update field of view. Also updates clientID
// to keep track of the last client to update these values
void GeomClient::setFOV(int clientID, int fov) {

  m_commLock->lock();
  // First tell the server we are doing a set fov
  (*m_geomXfer) << (qint32)SET_FOV;
  // Send the client ID
  (*m_geomXfer) << (qint32)clientID;
  // Send the translation matrix
  (*m_geomXfer) << (qint32)fov;
  m_geomSocket->flush();
  m_geomSocket->waitForBytesWritten(2000);
  m_commLock->unlock();
}

//set Plane ID
void GeomClient::setPlaneID(int planeID) {

  m_commLock->lock();
  (*m_geomXfer) << (qint32)SET_PLANE_ID;

  // Send the client ID
  //m_geomXfer->send((long)clientID);

  // Send the plane ID 
  (*m_geomXfer) << (qint32)planeID;
  m_geomSocket->flush();
  m_geomSocket->waitForBytesWritten(2000);
  m_commLock->unlock();
}


// setTrigDelay
//
// Function to set/update cardiac trigger delay. Also updates clientID
// to keep track of the last client to update these values
void GeomClient::setTrigDelay(int clientID, int trig) {

  m_commLock->lock();
  // First tell the server we are doing a set cardiac trigger delay
  (*m_geomXfer) << (qint32)SET_TRIGGER_DELAY;
  // Send the client ID
  (*m_geomXfer) << (qint32)clientID;
  // Send the trigger delay
  (*m_geomXfer) << (qint32)trig;
  
  m_geomSocket->flush();
  m_geomSocket->waitForBytesWritten(2000);
  m_commLock->unlock();
}

// setRespPhase
//
// Function to set/update respiratory phase value. Also updates clientID
// to keep track of the last client to update these values
void GeomClient::setRespPhase(int clientID, int resp) {
  m_commLock->lock();
  // First tell the server we are doing a set cardiac trigger delay
  (*m_geomXfer) << (qint32)SET_RESP_PHASE;
  // Send the client ID
  (*m_geomXfer) << (qint32)clientID;
  // Send the respiratory phase
  (*m_geomXfer) << (qint32)resp;

  m_geomSocket->flush();
  m_geomSocket->waitForBytesWritten(2000);
  m_commLock->unlock();

}

// setImage
//
// Function to set/update image size and image data. Also updates clientID
// to keep track of the last client to update these values
void GeomClient::setImage(int clientID, int size, unsigned char* img, int numChannels) {
  qint32 message_size;

  m_commLock->lock();
  // First tell the server we are doing a set image
  (*m_geomXfer) << (qint32)SET_IMAGE;
  
  // Send the client ID
  (*m_geomXfer) << (qint32)clientID;
  // Send the image area size
  message_size = size * size;
  (*m_geomXfer) << message_size;    
  // Send number of channels
  (*m_geomXfer) << (qint32)numChannels;
  // Send image
  message_size *= numChannels; //actual size of image data to write
  m_geomXfer->writeRawData((char*)img, message_size); 
  
  m_geomSocket->flush();
  m_geomSocket->waitForBytesWritten(2000);
  m_commLock->unlock();
}



// Set all catheter information
//
// 
void GeomClient::setAllCath(int clientID, float* coords, int* angles, int locationID, int SNR, int coil_ID) {

  m_commLock->lock();
  // First tell the server we are doing a set cardiac trigger delay
  (*m_geomXfer) << (qint32)SET_ALL_CATH;
  
  // Send the client ID
  (*m_geomXfer) << (qint32)clientID;
  // Send the coil ID
  (*m_geomXfer) << (qint32)coil_ID;    
  // Send the coordinates
  m_geomXfer->writeRawData((char*)coords, COORDS_SIZE*sizeof(float));
  // Send the angles
  m_geomXfer->writeRawData((char*)angles, ANGLES_SIZE*sizeof(long));
  // Send the location ID
  (*m_geomXfer) << (qint32)locationID;
  // Send the SNR
  (*m_geomXfer) << (qint32)SNR;
  
  m_geomSocket->flush();
  m_geomSocket->waitForBytesWritten(2000);
  m_commLock->unlock();
    
}



void GeomClient::setCathCoordinates(int clientID, float* coords, int coil_ID) {

  m_commLock->lock();
  // First tell the server we are doing a set coordinates
  (*m_geomXfer) << (qint32)SET_COORDINATES;
  
  // Send the client ID
  (*m_geomXfer) << (qint32)clientID;
  // Send the coil ID
  (*m_geomXfer) << (qint32)coil_ID;    
  // Send the coordinates
  m_geomXfer->writeRawData((char*)coords, COORDS_SIZE*sizeof(float));
  
  m_geomSocket->flush();    
  m_geomSocket->waitForBytesWritten(2000); 
  m_commLock->unlock();
}




void GeomClient::setCathAngles(int clientID, int* angles, int coil_ID) {

  m_commLock->lock();
  // First tell the server we are doing a set angles
  (*m_geomXfer) << (qint32)SET_ANGLES;

  // Send the client ID
  (*m_geomXfer) << (qint32)clientID;
  // Send the coil ID
  (*m_geomXfer) << (qint32)coil_ID;    
  // Send the angles
  m_geomXfer->writeRawData((char*)angles, ANGLES_SIZE*sizeof(long));
  
  m_geomSocket->flush();  
  m_geomSocket->waitForBytesWritten(2000);
  m_commLock->unlock();
}




void GeomClient::setCathLocationID(int clientID, int locationID, int coil_ID) {
  
  m_commLock->lock();
  // First tell the server we are doing a set location ID
  (*m_geomXfer) << (qint32)SET_LOCATION_ID;
  // Send the client ID
  (*m_geomXfer) << (qint32)clientID;
  // Send the coil ID
  (*m_geomXfer) << (qint32)coil_ID;    
  // Send the location ID
  (*m_geomXfer) << (qint32)locationID;
  
  m_geomSocket->flush();  
  m_geomSocket->waitForBytesWritten(2000);
  m_commLock->unlock();
}





void GeomClient::setCathSNR(int clientID, int SNR, int coil_ID) {

  m_commLock->lock();
  // First tell the server we are doing a set SNR
  (*m_geomXfer) << (qint32)SET_SNR;  
  // Send the client ID
  (*m_geomXfer) << (qint32)clientID;
  // Send the coil ID
  (*m_geomXfer) << (qint32)coil_ID;    
  // Send the SNR
  (*m_geomXfer) << (qint32)SNR;
  
  m_geomSocket->flush(); 
  m_geomSocket->waitForBytesWritten(2000);
  m_commLock->unlock();
}




void GeomClient::setNumCathCoils(int clientID, int cathCoils) {

  m_commLock->lock();
  // First tell the server we are doing a set cath coil count
  (*m_geomXfer) << (qint32)SET_NUM_OF_CATH_COILS;
  
  // Send the client ID
  (*m_geomXfer) << (qint32)clientID;
  // Send the cath coil count
  (*m_geomXfer) << (qint32)cathCoils;
  
  m_geomSocket->flush();  
  m_geomSocket->waitForBytesWritten(2000);
  m_commLock->unlock();
}




// setCathMode
//
// Query the server for the number of catheter coils
void GeomClient::setCathMode(int clientID,int mode) {
  m_commLock->lock();
  // First tell the server we are doing a set cath coil count
  (*m_geomXfer) << (qint32)SET_CATH_MODE;
  
  // Send the client ID
  (*m_geomXfer) << (qint32)clientID;
  // Send the cath coil count
  (*m_geomXfer) << (qint32)mode;
  
  m_geomSocket->flush(); 
  m_geomSocket->waitForBytesWritten(2000);
  m_commLock->unlock();
}



// ############################################# //
// ### Functions to get geometry information ### //
// ############################################# //

// getLastClientWrite
//
// Query the server for the unique client ID of the client who last
// updated any geometry information
int GeomClient::getLastClientWrite() {
  qint32 lastClient;
  
  m_commLock->lock();
  // First ask the server for the client ID
  (*m_geomXfer) << (qint32)GET_LAST_CLIENT_WRITE;
  m_geomSocket->flush();
  m_geomSocket->waitForBytesWritten(2000);

  // waitForReadyRead is broken in earlier versions of Qt. 
  while (m_geomSocket->bytesAvailable() < sizeof(qint32)){
#if QT_VERSION >= 0x040300
    m_geomSocket->waitForReadyRead(2000);
#else
    QCoreApplication::processEvents();
#endif
  }

  // Get the client ID
  (*m_geomXfer) >> lastClient;
  m_geomSocket->flush();
  m_commLock->unlock();

  return (int)lastClient;
}




// getRotation
//
// Query the server for the current rotation matrix
float* GeomClient::getRotation() {

  m_commLock->lock();
  // First ask the server for the rotation matrix
  (*m_geomXfer) << (qint32)GET_ROTATION;
  m_geomSocket->flush();
  m_geomSocket->waitForBytesWritten(2000);

  // waitForReadyRead is broken in earlier versions of Qt. 
  while (m_geomSocket->bytesAvailable() < ROT_MATRIX_SIZE*sizeof(long)){
#if QT_VERSION >= 0x040300
    m_geomSocket->waitForReadyRead(2000);
#else
    QCoreApplication::processEvents();
#endif
  }

  // Get the rotation matrix
  m_geomXfer->readRawData((char*)m_rotation, ROT_MATRIX_SIZE*sizeof(long));
  m_geomSocket->flush();
  m_commLock->unlock();

  return (float*)m_rotation;
}



// getTranslation
//
// Query the server for the current translation matrix
float* GeomClient::getTranslation() {

  m_commLock->lock();
  // First ask the server for the rotation matrix
  (*m_geomXfer) << (qint32)GET_TRANSLATION;
  m_geomSocket->flush();
  m_geomSocket->waitForBytesWritten(2000);

  // waitForReadyRead is broken in earlier versions of Qt. 
  while (m_geomSocket->bytesAvailable() < TRANS_MATRIX_SIZE*sizeof(long)){
#if QT_VERSION >= 0x040300
    m_geomSocket->waitForReadyRead(2000);
#else
    QCoreApplication::processEvents();
#endif
  }  


  // Get the translation matrix
  m_geomXfer->readRawData((char*)m_translation, TRANS_MATRIX_SIZE*sizeof(long));
  m_geomSocket->flush();
  m_commLock->unlock();
  return (float*)m_translation;
}




// getFOV
//
// Query the server for the current field-of-view
int GeomClient::getFOV() {
  qint32 fov;

  m_commLock->lock();
  // First ask the server for the FOV
  (*m_geomXfer) << (qint32)GET_FOV;
  m_geomSocket->flush();
  m_geomSocket->waitForBytesWritten(2000);

  // waitForReadyRead is broken in earlier versions of Qt. 
  while (m_geomSocket->bytesAvailable() < sizeof(qint32)){
#if QT_VERSION >= 0x040300
    m_geomSocket->waitForReadyRead(2000);
#else
    QCoreApplication::processEvents();
#endif
  }


  // Get the FOV
  (*m_geomXfer) >> fov;
  m_geomSocket->flush();
  m_commLock->unlock();

  return (int)fov;
}



//Query the current plane ID
int GeomClient::getPlaneID() {
  qint32 planeID;

  m_commLock->lock();
  // First ask the server for the FOV
  (*m_geomXfer) << (qint32)GET_PLANE_ID;
  m_geomSocket->flush();
  m_geomSocket->waitForBytesWritten(2000);

  // waitForReadyRead is broken in earlier versions of Qt. 
  while (m_geomSocket->bytesAvailable() < sizeof(qint32)){
#if QT_VERSION >= 0x040300
    m_geomSocket->waitForReadyRead(2000);
#else
    QCoreApplication::processEvents();
#endif
  }
 
  // Get the FOV
  (*m_geomXfer) >> planeID;
  m_geomSocket->flush();
  m_commLock->unlock();
  return (int)planeID;
}




// getTrigDelay
//
// Query the server for the current cardiac trigger delay
// (i.e. time from last cardiac trigger)
int GeomClient::getTrigDelay() {
  qint32 delay;
  
  m_commLock->lock();
  // First ask the server for the trigger delay
  (*m_geomXfer) << (qint32)GET_TRIGGER_DELAY;
  m_geomSocket->flush();
  m_geomSocket->waitForBytesWritten(2000);

  // waitForReadyRead is broken in earlier versions of Qt. 
  while (m_geomSocket->bytesAvailable() < sizeof(qint32)){
#if QT_VERSION >= 0x040300
    m_geomSocket->waitForReadyRead(2000);
#else
    QCoreApplication::processEvents();
#endif
  }  


  // Get the trigger delay
  (*m_geomXfer) >> delay;
  m_geomSocket->flush();
  m_commLock->unlock();

  return (int)delay;
}



// getRespPhase
//
// Query the server for the current respiratory phase
// (i.e. the current value from the respiratory bellows)
int GeomClient::getRespPhase() {
  qint32 phase;

  m_commLock->lock();
  // First ask the server for the resp phase
  (*m_geomXfer) << (qint32)GET_RESP_PHASE;
  m_geomSocket->flush();
  m_geomSocket->waitForBytesWritten(2000);

  // waitForReadyRead is broken in earlier versions of Qt. 
  while (m_geomSocket->bytesAvailable() < sizeof(qint32)){
#if QT_VERSION >= 0x040300
    m_geomSocket->waitForReadyRead(2000);
#else
    QCoreApplication::processEvents();
#endif
  }   


  // Get the resp phase
  (*m_geomXfer) >> phase;
  m_geomSocket->flush();
  m_commLock->unlock();

  return (int)phase;
}



// getImageSize
//
// Query the server for the current image size
int GeomClient::getImageSize() {
  qint32 imgSize;
  
  m_commLock->lock();
  imgSize = GET_IMAGE_SIZE;
  // First ask the server for the image size
  (*m_geomXfer) << imgSize;
  m_geomSocket->flush();
  m_geomSocket->waitForBytesWritten(2000);

  // waitForReadyRead is broken in earlier versions of Qt. 
  while (m_geomSocket->bytesAvailable() < sizeof(qint32)){
#if QT_VERSION >= 0x040300
    m_geomSocket->waitForReadyRead(2000);
#else
    QCoreApplication::processEvents();
#endif
  } 

  // Get the image size
  (*m_geomXfer) >> imgSize;
  m_geomSocket->flush();
  m_commLock->unlock();

  return (int)imgSize;
}




// getNumImageChannels
//
// Query the server to get the number of channels
// for the pixels in the image
// Grey = 1, RGBA = 4
int GeomClient::getNumImgChannels() {
  qint32 numChannels;

  m_commLock->lock();
  // ask the server for the number of channels stored
  (*m_geomXfer) << (qint32)GET_NUM_IMAGE_CHANNELS;
  m_geomSocket->flush();
  m_geomSocket->waitForBytesWritten(2000);

  // waitForReadyRead is broken in earlier versions of Qt. 
  while (m_geomSocket->bytesAvailable() < sizeof(qint32)){
#if QT_VERSION >= 0x040300
    m_geomSocket->waitForReadyRead(2000);
#else
    QCoreApplication::processEvents();
#endif
  } 

  // retrieve
  (*m_geomXfer) >> numChannels;
  m_geomSocket->flush();
  m_commLock->unlock();

  return (int)numChannels;
}



// getImage
//
// Query the server for the current image data
unsigned char* GeomClient::getImage() {
  int imgSize;
  int numChannels;
  int message_size = 0;

  imgSize = getImageSize();
  numChannels = getNumImgChannels();

  m_commLock->lock();
  // First ask the server for the image
  (*m_geomXfer) << (qint32)GET_IMAGE;
  m_geomSocket->flush();

  // Get the image
  message_size = imgSize * imgSize * numChannels;
  m_geomSocket->waitForBytesWritten(2000);

  delete [] m_image; m_image = 0;
  m_image = new char[message_size];

  // waitForReadyRead is broken in earlier versions of Qt. 
  while (m_geomSocket->bytesAvailable() < message_size){
#if QT_VERSION >= 0x040300
    m_geomSocket->waitForReadyRead(2000);
#else
    QCoreApplication::processEvents();
#endif
  } 

  m_geomXfer->readRawData(m_image, message_size);
  m_commLock->unlock();

  return (unsigned char*)m_image;
}



// getCathCoordinates
//
// Query the server for the catheter coordinates
float* GeomClient::getCathCoordinates(int coil_ID) {

  m_commLock->lock();
  // First ask the server for the coordinates
  (*m_geomXfer) << (qint32)GET_COORDINATES;
  (*m_geomXfer) << (qint32)coil_ID;    
  m_geomSocket->flush();
  m_geomSocket->waitForBytesWritten(2000);

  // waitForReadyRead is broken in earlier versions of Qt. 

  while (m_geomSocket->bytesAvailable() < COORDS_SIZE*sizeof(float)){
#if QT_VERSION >= 0x040300
    m_geomSocket->waitForReadyRead(2000);
#else
    QCoreApplication::processEvents();
#endif
  }


  // Get the coordinates
  m_geomXfer->readRawData((char*)m_cathCoords, COORDS_SIZE*sizeof(float));
  m_geomSocket->flush();
  m_commLock->unlock();

  return m_cathCoords;
}




// getCathAngles
//
// Query the server for the catheter angles
int* GeomClient::getCathAngles(int coil_ID) {

  m_commLock->lock();
  // First ask the server for the coordinates
  (*m_geomXfer) << (qint32)GET_ANGLES;
  (*m_geomXfer) << (qint32)coil_ID;    
  m_geomSocket->flush();
  m_geomSocket->waitForBytesWritten(2000);

  // waitForReadyRead is broken in earlier versions of Qt. 
  while (m_geomSocket->bytesAvailable() < ANGLES_SIZE*sizeof(long)){
#if QT_VERSION >= 0x040300
    m_geomSocket->waitForReadyRead(2000);
#else
    QCoreApplication::processEvents();
#endif
  }


  // Get the coordinates
  m_geomXfer->readRawData((char*)m_cathAngles, ANGLES_SIZE*sizeof(long));
  m_geomSocket->flush();
  m_commLock->unlock();

  return (int*)m_cathAngles;
}



// getCathCoordinates
//
// Query the server for the catheter location ID
int GeomClient::getCathLocationID(int coil_ID) {
  qint32 locationID;

  m_commLock->lock();
  // First ask the server for the coordinates
  (*m_geomXfer) << (qint32)GET_LOCATION_ID;
  (*m_geomXfer) << (qint32)coil_ID;    
  m_geomSocket->flush();
  m_geomSocket->waitForBytesWritten(2000);

  // waitForReadyRead is broken in earlier versions of Qt. 
  while (m_geomSocket->bytesAvailable() < sizeof(qint32)){
#if QT_VERSION >= 0x040300
    m_geomSocket->waitForReadyRead(2000);
#else
    QCoreApplication::processEvents();
#endif
  }

  // Get the coordinates
  (*m_geomXfer) >> locationID;
  m_geomSocket->flush();
  m_commLock->unlock();
  return (int)locationID;
}



// getCathLocationID
//
// Query the server for the catheter SNR
int GeomClient::getCathSNR(int coil_ID) {
  qint32 SNR;

  m_commLock->lock();
  // First ask the server for the coordinates
  (*m_geomXfer) << (qint32)GET_SNR;
  (*m_geomXfer) << (qint32)coil_ID;    
  m_geomSocket->flush();
  m_geomSocket->waitForBytesWritten(2000);
  // waitForReadyRead is broken in earlier versions of Qt. 
  while (m_geomSocket->bytesAvailable() < sizeof(qint32)){
#if QT_VERSION >= 0x040300
    m_geomSocket->waitForReadyRead(2000);
#else
    QCoreApplication::processEvents();
#endif
  }  

  // Get the coordinates
  (*m_geomXfer) >> SNR;
  m_geomSocket->flush();
  m_commLock->unlock();

  return (int)SNR;
}




// getNumCathCoils
//
// Query the server for the number of catheter coils
int GeomClient::getNumCathCoils() {
  qint32 numCathCoils;

  m_commLock->lock();
  // First ask the server for the coordinates
  (*m_geomXfer) << (qint32)GET_NUM_OF_CATH_COILS;
  m_geomSocket->flush();
  m_geomSocket->waitForBytesWritten(2000);
  // waitForReadyRead is broken in earlier versions of Qt. 
  while (m_geomSocket->bytesAvailable() < sizeof(qint32)){
#if QT_VERSION >= 0x040300
    m_geomSocket->waitForReadyRead(2000);
#else
    QCoreApplication::processEvents();
#endif
  }

  // Get the coordinates
  (*m_geomXfer) >> numCathCoils;
  m_geomSocket->flush();
  m_commLock->unlock();
  return (int)numCathCoils;
}



// getCathMode
//
// Query the server for the catheter tracking mode (phase/projection)
int GeomClient::getCathMode() {
  qint32 cath_mode;

  m_commLock->lock();
  // First ask the server for the coordinates
  (*m_geomXfer) << (qint32)GET_CATH_MODE;
  m_geomSocket->flush();
  m_geomSocket->waitForBytesWritten(2000);
  // waitForReadyRead is broken in earlier versions of Qt. 
  while (m_geomSocket->bytesAvailable() < sizeof(qint32)){
#if QT_VERSION >= 0x040300
    m_geomSocket->waitForReadyRead(2000);
#else
    QCoreApplication::processEvents();
#endif
  }  

  // Get the coordinates
  (*m_geomXfer) >> cath_mode;
  m_geomSocket->flush();
  m_commLock->unlock();
  return (int)cath_mode;
}

// setVolumeTranslation
//
// Function to set/update translation for the volume. Also updates clientID
// to keep track of the last client to update these values
void GeomClient::setVolumeTranslation(int clientID, float* trans) {
  int message_size;

  m_commLock->lock();
  // First tell the server we are doing a set for all variables
  (*m_geomXfer) << (qint32)SET_VOL_TRANSLATION;
  // Send the client ID
  (*m_geomXfer) << (qint32)clientID;
  // Send the translation matrix
  message_size = TRANS_MATRIX_SIZE;
  m_geomXfer->writeRawData((char*)trans,message_size*sizeof(long));
  
  m_geomSocket->waitForBytesWritten(2000);
  m_commLock->unlock();
}

// getVolumeTranslation
//
// Query the server for the current volume translation
float* GeomClient::getVolumeTranslation() {
  int message_size;

  message_size = TRANS_MATRIX_SIZE;

  m_commLock->lock();
  // First ask the server for the rotation matrix
  (*m_geomXfer) << (qint32)GET_VOL_TRANSLATION;
  m_geomSocket->waitForBytesWritten(2000);
  // waitForReadyRead is broken in earlier versions of Qt. 
  while (m_geomSocket->bytesAvailable() < message_size*sizeof(float)){
#if QT_VERSION >= 0x040300
    m_geomSocket->waitForReadyRead(2000);
#else
    QCoreApplication::processEvents();
#endif
  } 

  // Get the translation matrix
  m_geomXfer->readRawData((char*)m_volTranslation,message_size*sizeof(float));
  m_commLock->unlock();

  return (float*)m_volTranslation;
}
