// geom_client.h
//
// Header file for geometry client class.
// Jeff Stainsby, 7Nov03
//
// Modified by Stefan Pintilie, Sept 2008
//

#ifndef GEOM_CLIENT_H
#define GEOM_CLIENT_H

class QDataStream;
class QTcpSocket;
class QMutex;

// Some local constants for matrix sizes etc
#include "GeomServerConsts.h"



// Define the geometry client class
class GeomClient {

  public:
    GeomClient();
    ~GeomClient();

    // Functions to connect to server
    int connect(const char* hostname, int port, bool swap=false);
    void disconnect();

    // Functions for setting/updating geometry information
    void setAll(int clientID, int trig, int resp, float* rot, float* trans, int fov, int size, unsigned char* img, int numChannels );
    void setRotation(int clientID, float* rot);
    void setTranslation(int clientID, float* trans);
    void setFOV(int clientID, int fov);
    void setTrigDelay(int clientID, int trig);
    void setRespPhase(int clientID, int resp);
    void setImage(int clientID, int size, unsigned char* img, int numChannels );
    void setPlaneID(int planeID);
    
    void setAllCath(int clientID, float* coords, int* angles, int locationID, int SNR, int coil_ID);
    void setCathCoordinates(int clientID, float* coords, int coil_ID);
    void setCathAngles(int clientID, int* angles, int coil_ID);
    void setCathLocationID(int clientID, int locationID, int coil_ID);
    void setCathSNR(int clientID, int SNR, int coil_ID);
    void setNumCathCoils(int clientID, int cathCoils);
    void setCathMode(int clientID, int mode);
    void setVolumeTranslation(int clientID, float* trans);
    
    // Functions for querying geometry information
    int getLastClientWrite();
    float* getRotation();
    float* getTranslation();
    int getFOV();
    int getTrigDelay();
    int getRespPhase();
    int getImageSize();
    int getNumImgChannels();
    unsigned char* getImage();
    int getPlaneID();
    float* getCathCoordinates(int coil_ID);
    int* getCathAngles(int coil_ID);
    int getCathLocationID(int coil_ID);
    int getCathSNR(int coil_ID);
    int getNumCathCoils();
    int getCathMode();
    float* getVolumeTranslation();
    
  private:
    QDataStream* m_geomXfer;    // Recon server for images
    QTcpSocket* m_geomSocket;   // Connection to the image recon server
    QMutex* m_commLock;         // A lock to prevent a client from calling one function before another has finished.

    int m_coilID;

    long m_rotation[ROT_MATRIX_SIZE];
    long m_translation[TRANS_MATRIX_SIZE];
    char* m_image;
    float m_cathCoords[COORDS_SIZE];
    long m_cathAngles[ANGLES_SIZE];
    long m_volTranslation[TRANS_MATRIX_SIZE];
};

#endif
