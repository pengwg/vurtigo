#ifndef GENERIC_MODE_H
#define GENERIC_MODE_H

#include "geom_client.h"
#include "sender.h"
#include "arguments.h"

#include <QList>
#include <QSemaphore>

#include "SenderThread.h"

class Converter;

//** Defines the superclass for all modes. This class cannot and should not be instantiated directly.
class GenericMode{
 private:

 protected:
  // Variables that are common to all modes.
  //! Object that communicates with the Geometry Server.
  GeometrySender* m_sender;
  //! All data related to the image.
  std::vector<IMAGEDATA> m_imgDataArray;
  //! All data related to the catheter
  std::vector<CATHDATA> m_cathDataArray;
  int m_planeID;

  QList<SenderThread::ListObject> m_planeList;
  QList<SenderThread::ListObject> m_cathList;

  Converter* converter;
  QSemaphore m_planeListLock;
  QSemaphore m_cathListLock;

  bool m_zeroCathOnly;
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
  bool sendPlanePosition(int id);
  bool receivePlanePosition(int id);
  bool receivePlane(int id);
  bool receivePlaneAndPosition(int id);
  bool receiveCatheter(int id);

  void setPlaneList(QList<SenderThread::ListObject>  plane);
  void setCathList(QList<SenderThread::ListObject> cath);
  inline void setZeroCathOnly(bool zeroCath) { m_zeroCathOnly = zeroCath; }

  std::vector<CATHDATA> & getCaths() {
    return m_cathDataArray;
  }

  std::vector<IMAGEDATA> & getImages() {
    return m_imgDataArray;
  }
};


#endif
