/*
  GeomServer2
  Copyright (C) 2008  Stefan Pintilie

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef CONSTANTS_H
#define CONSTANTS_H

enum CatheterMode { 
  NO_CATHETERS = -1,
  CATH_PROJECTION,
  PHASE_MODE
};


//! The default connection port. 
/*!
  The GeomServer will listen on this port for new connections. A different port may be specified at runtime via the command line.
  If the default port is used then all clients should connect to this port. 
 */
#define PORT 1777

//! Value used for the swap test.
/*!
  The GeomServer will use this value to determine if the client is big endian or little endian. This value will be passed in the initial handshake by the client and the server will check to see if the value is correct. 
 */
#define SWAP_TEST 1777

//! The id value for a GUI update event.
/*!
  The GeomServer needs to pass data to its GUI with a special internal event. This is the ID for that event. 
 */
#define SPECIAL_UPDATE_EVENT 1001

//! The number of miliseconds between each GUI update. 
/*!
  The GUI is updated by a timer that is triggered at regular intervals. This constant sets the time between consecutive GUI updates. 
 */
#define MSEC_DELAY_GUI_UPDATE 100

//! Update flags for the UI
/*!
  The GeomServer UI will not be updated if the information has not changed. Different parts of the GUI will be updated if the information contained in them has been updated.
 */
enum UIUpdateParam {
  USER_CHANGED = 0, /*!< The flag location in an array that indicates if user parameters have changed. */  
  IMAGE_CHANGED,    /*!< The location of the flag that indicates if image data has changed. */
  CATH_CHANGED      /*!< The location of the flag that indicates if catheter data has changed. */
};

//! The maximum number of images. 
/*!
  This value is used more as an error checking value. There should never be more than 100 realtime planes. (Usually no more than 8!)
 */
#define MAX_IMAGES 100

//! The maximum number of catheters. (Each catheter may have multiple coils).
/*!
  This value is an error checking parameter. In reality there are no more than 1 or 2 catheters. Note that this is not a limit on the number of coils each catheter may have. 
 */
#define MAX_CATH 100

//! The maximum number of client connections.
/*! 
  The GeomServer will limit the number of clients that are connected to it. This upper limit should never be met but it is in place as a safety. This number can be increased but should be done only if it is needed. A large number of clinets will significantly slow down the GeomServer. 
 */
#define MAX_USER_OBJECT 100

//! The size of the Rotation Matrix (3x3)
#define ROT_MATRIX_SIZE 9
//! The size of the translation matrix
#define TRANS_MATRIX_SIZE 3
//! The size of a set of coords.
#define COORDS_SIZE 3
//! The size of the angles array.
#define ANGLES_SIZE 2

//! The maximum size of an image. 
#define MAX_IMAGE_SIZE 4096
//! The maximum number of channels for each pixel in the image.
#define MAX_NUM_CHANNELS 32

//! The version of the server to use
/*!
  As the server moves from a polling architecture to one where the information is pushed to the client it is important to maintain backward compatibility. 
 */
enum ServerVersion {
  SERV_VER_1_0=0,
  SERV_VER_2_0
};

// Communication Parameters
enum CommParam {
  COM_GET=0,
  COM_SET,
  COM_IMAGE,
  COM_CATH,
  COM_REPLY_OK,     // What you sent to the geom server was ok and has been stored.
  COM_REPLY_SEND,   // What you reuested is new and available. Info to follow. 
  COM_REPLY_NOTNEW, // What you requested is not new so wait for a bit. (goes for recv as well as send!)
  COM_REPLY_ERR	    //  Error. Request not processed.  
};

enum ModifParam {
  MOD_TRIG = 0,
  MOD_RESP,
  MOD_ROT,
  MOD_TRANS,
  MOD_FOV,
  MOD_IMG,
  MOD_MODE,
  MOD_COORDS,
  MOD_ANGLES,
  MOD_LOCID,
  MOD_SNR,
  MOD_LAST  /// THIS MUST BE LAST AT ALL TIMES!!
};


// To Make Backwards compatible
#define SET_ALL           1
#define SET_ROTATION      2
#define SET_TRANSLATION   3
#define SET_FOV           4
#define SET_TRIGGER_DELAY 5
#define SET_RESP_PHASE    6
#define SET_IMAGE_SIZE    7
#define SET_IMAGE         8
#define GET_LAST_CLIENT_WRITE 9
#define GET_ROTATION      10
#define GET_TRANSLATION   11
#define GET_FOV           12
#define GET_TRIGGER_DELAY 13
#define GET_RESP_PHASE    14
#define GET_IMAGE_SIZE    15
#define GET_IMAGE         16
#define GET_NUM_IMAGE_CHANNELS 17
#define SET_PLANE_ID 18
#define GET_PLANE_ID 19
#define SET_NUM_OF_CATH_COILS 20
#define SET_COORDINATES 21
#define SET_ANGLES 22
#define SET_LOCATION_ID 23
#define SET_SNR 24
#define GET_NUM_OF_CATH_COILS 25
#define GET_COORDINATES 26
#define GET_ANGLES 27
#define GET_LOCATION_ID 28
#define GET_SNR 29
#define SET_ALL_CATH 30
#define SET_CATH_MODE 31
#define GET_CATH_MODE 32
#define SET_VOL_TRANSLATION 33
#define GET_VOL_TRANSLATION 34



#endif
