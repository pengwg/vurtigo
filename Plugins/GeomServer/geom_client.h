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

  enum SocketError{
    NoError = 0, 
    ConnectionRefusedError, 
    RemoteHostClosedError,
    HostNotFoundError, 
    OtherError
  };


    GeomClient();
    ~GeomClient();

    // Functions to connect to server
    int connect(const char* hostname, int port);
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

    void setErrorState(SocketError es) { m_eState = es; }
    void clearErrorState() { m_eState = NoError; }

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

    SocketError getErrorState() { return m_eState; }

  private:
    QDataStream* m_geomXfer;    // Recon server for images
    QTcpSocket* m_geomSocket;   // Connection to the image recon server
    QMutex* m_commLock;         // A lock to prevent a client from calling one function before another has finished.

    //! The socket error state. Value of zero if there is no error. 
    SocketError m_eState;

    int m_coilID;

    long m_rotation[ROT_MATRIX_SIZE];
    long m_translation[TRANS_MATRIX_SIZE];
    char* m_image;
    float m_cathCoords[COORDS_SIZE];
    long m_cathAngles[ANGLES_SIZE];
    long m_volTranslation[TRANS_MATRIX_SIZE];
};

#endif
