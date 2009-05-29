#ifndef GEOM_TEST_SENDER_H
#define GEOM_TEST_SENDER_H

#include <string>
#include <vector>
#include "arguments.h"

// Client Lib
#include "geom_client.h"


//! Sends a "repeating loop" of geometry information from a file to the server 
class GeometrySender {

    public: 
        GeometrySender();
        ~GeometrySender();

	bool connect(char*, int, bool);
	void disconnect();        
        bool isConnected();

        bool initializeCath(int cathMode, int numCoils);
        bool sendNextCath(CATHDATA cDat);
        
	bool sendImgData(IMAGEDATA *imgDat, int planeID);
	

	//! Get the Trig Delay
	int getTrig() { return m_geomClient->getTrigDelay(); }
	//! Get the resp phase. 
	int getResp() { return m_geomClient->getRespPhase(); }
	//! Get the FOV from the server. 
	int getFOV() { return m_geomClient->getFOV(); }
	//! Get the plane ID from the server. 
	int getPlaneID() { return m_geomClient->getPlaneID(); }
	
	float* getImgRotation() { return m_geomClient->getRotation(); }
	float* getImgTranslation() { return m_geomClient->getTranslation(); }

	int getImgSize() { return m_geomClient->getImageSize(); }
	int getNumChan() { return m_geomClient->getNumImgChannels(); }
	int getLastClientWrite() { return m_geomClient->getLastClientWrite(); }

	//! Get the volume translation from the server. 
	bool getVolTranslation(float*& trans);
	float* getVolTranslation() { return m_geomClient->getVolumeTranslation(); }

	//! Get the image from the server.
	unsigned char* getImage();

	// Catheter Access Functions.
	int getCathModeAsInt() { return m_geomClient->getCathMode(); }
	int getNumCathCoils() { return m_geomClient->getNumCathCoils(); }

	float* getCathCoords(int coil) { return m_geomClient->getCathCoordinates(coil); }
	int* getCathAngles(int coil) { return m_geomClient->getCathAngles(coil); }
	int getCathLocID(int coil) { return m_geomClient->getCathLocationID(coil); }
	int getCathSNR(int coil) { return m_geomClient->getCathSNR(coil); }


	// Functions to send info to the server. 
	bool setPlaneID(int planeID);
	bool setFov(int fov);
	bool setTranslation(float* trans);	
	bool setVolTranslation(float* trans);
	bool xRotation(float angle);
	bool yRotation(float angle);
	bool zRotation(float angle);
	bool setRotation(float* rotation);
	bool setAxial();
	bool setSagittal();
	bool setCoronal();
	void setTrigDelay(int);
	void setRespPhase(int);

    protected:
        GeomClient* m_geomClient;   //!< the geometry communications helper
        int m_clientID;             //!< geometry client id	
};



#endif
