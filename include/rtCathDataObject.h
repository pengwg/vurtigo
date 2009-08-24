#ifndef RT_CATH_DATA_OBJECT_H
#define RT_CATH_DATA_OBJECT_H

#include "rtDataObject.h"
#include "rtPropertyChooserDialog.h"

#include "ui_cathOptions.h"

#include <QHash>
#include <QMultiMap>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

#include "vtkProperty.h"

//! Catheter Data Object
/*!
  Catheter information such as coil locations and SNR are kept here.
  */
class rtCathDataObject : public rtDataObject
{
Q_OBJECT
public:

  class CathCoilData {
    public:
      int locationID;
      int angles[2];
      double cx, cy, cz;
      double SNR;
  };

  enum EstimationType {
    ET_MEAN,
    ET_WTMEAN,
    ET_BESTSNR
  };

  rtCathDataObject();
  ~rtCathDataObject();

  void update();

  // Coil specific
  int addCoil(int loc);
  bool setCoilSNR(int coilID, int SNR);
  bool setCoilAngles(int coilID, int a1, int a2);
  bool setCoilCoords(int coilID, double cx, double cy, double cz);
  CathCoilData* getCoilHandle(int coilID);
  bool removeCoil(int coilID);

  // List specific
  int getNumCoils();
  QHash<int, CathCoilData>* getListHandle();
  int getNumLocations();
  QList<int> getLocationList();
  bool getPositionAtLocation(int loc, double out[3]);
  bool getSNRAtLocation(int loc, double &SNR);

  //Property
  vtkProperty* getSplineProperty() { return m_splineProperty; }
  vtkProperty* getPointProperty() { return m_pointProperty; }
  vtkProperty* getTipProperty() { return m_tipProperty; }

  // Estimation Type
  EstimationType getEstimationType() { return m_eType; }
  void setEstimationType(EstimationType et) { m_eType = et; }

  int getPointSize() { return m_pointSize; }
  bool useSNRSize() { return m_useSNRSize; }

 public slots:
  void splinePropertyDialog();
  void pointPropertyDialog();
  void tipPropertyDialog();

  void pointSizeChanged(int size);

  void useSNRSizeChanged(int);

 protected:
  // Functions
  void setupGUI();
  void cleanupGUI();
  
  int getNewCoilID();

  //! A hash of coild ID with the coil data
  QHash<int, CathCoilData> m_coilList;

  //! A hash with coil location and coil ID.
  QMultiMap<int, int> m_coilLocations;

  vtkProperty* m_splineProperty;
  vtkProperty* m_pointProperty;
  vtkProperty* m_tipProperty;

  rtPropertyChooserDialog* m_splinePropertyDlg;
  rtPropertyChooserDialog* m_pointPropertyDlg;
  rtPropertyChooserDialog* m_tipPropertyDlg;

  //! The metod to use to determine the catheter location when multiple coils exist with the same location ID.
  EstimationType m_eType;

  //! Maximum number of catheter coils
  int m_max_coils;

  //! Size multiplier for each of the points.
  int m_pointSize;

  bool m_useSNRSize;

  //! The objects that sets the widgets for the cath options.
  Ui::cathOptions m_cathGuiSetup;
};

#endif 
