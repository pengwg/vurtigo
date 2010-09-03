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
#ifndef GEOM_TEST_SENDER_H
#define GEOM_TEST_SENDER_H

#include "arguments.h"

// Client Lib
#include "geom_client.h"


//! Sends a "repeating loop" of geometry information from a file to the server 
class GeometrySender {

    public: 
        GeometrySender();
        ~GeometrySender();

    bool connect(char*, int);
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
  int getNumBytesPerPixel() { return m_geomClient->getNumImgBytesPerPixel(); } 
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
	bool setRotation(float* rotation);
	void setTrigDelay(int);
	void setRespPhase(int);

    protected:
        GeomClient* m_geomClient;   //!< the geometry communications helper
        int m_clientID;             //!< geometry client id	
};



#endif
