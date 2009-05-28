#ifndef GENERIC_MODE_H
#define GENERIC_MODE_H

#include "geom_client.h"
#include "sender.h"
#include "arguments.h"

//** Defines the superclass for all modes. This class cannot and should not be instantiated directly. 
class GenericMode{
 private:

 protected:
  // This class cannot be created directly. 
  GenericMode();
  virtual ~GenericMode();

  // Variables that are common to all modes. 
  GeometrySender* m_sender; //! Object that communicates with the Geometry Server. 
  int m_lastClientIndex; //! The index of the last client that used this data. 
  std::vector<IMAGEDATA> m_imgDataArray; //! All data related to the image.
  std::vector<CATHDATA> m_cathDataArray; //! All data related to the catheter
  float m_volTransData[TRANS_MATRIX_SIZE]; //! The volume translation coords.
  int m_planeID;
 public:

  // All functions that have =0 are pure virtual. They MUST be implemented by the base class. void getAllGeom();

  // Init the mode based on the command line arguments.  
  virtual bool init(GeometrySender* geom, struct arguments* args)=0;

  // Start the mode running. 
  virtual void runMode()=0;

  // Print the data. 
  void print();
  void getAllGeom();
};


#endif
