#ifndef GENERIC_MODE_H
#define GENERIC_MODE_H

#include "geom_client.h"
#include "sender.h"
#include "arguments.h"

//** Defines the superclass for all modes. This class cannot and should not be instantiated directly.
class GenericMode{
 private:

 protected:
  // Variables that are common to all modes.
  //! Object that communicates with the Geometry Server.
  GeometrySender* m_sender;
  std::vector<IMAGEDATA> m_imgDataArray; //! All data related to the image.
  std::vector<CATHDATA> m_cathDataArray; //! All data related to the catheter
  int m_planeID;
  int m_numPlanes;
 public:
  // This class cannot be created directly.
  GenericMode();
  virtual ~GenericMode();

  // Init the mode based on the command line arguments.
  bool init(GeometrySender* geom, struct arguments* args);

  // Start the mode running.
  void runMode();

  // Print the data.
  void print();
  bool receivePlane(int id);
  bool receiveCatheter();

  std::vector<CATHDATA> & getCaths() {
    return m_cathDataArray;
  }

  std::vector<IMAGEDATA> & getImages() {
    return m_imgDataArray;
  }
};


#endif
