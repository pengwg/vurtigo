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
      bool visible;
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
  bool setCoilSNR(int coilID, double SNR);
  bool setCoilAngles(int coilID, int a1, int a2);
  bool setCoilCoords(int coilID, double cx, double cy, double cz);
  CathCoilData* getCoilHandle(int coilID);
  bool removeCoil(int coilID);

  // List specific
  int getNumCoils();
  int getNumAllCoils();
  QHash<int, CathCoilData>* getListHandle();
  int getNumLocations();
  QList<int> getLocationList();
  bool getPositionAtLocation(int loc, double out[3]);
  bool getSNRAtLocation(int loc, double &SNR);

  //Property
  vtkProperty* getSplineProperty() { return m_splineProperty; }
  vtkProperty* getPointProperty() { return m_pointProperty; }
  vtkProperty* getTipProperty() { return m_tipProperty; }
  void refreshProperties() { m_pointPropertyDlg->settingsChanged(); }

  // Estimation Type
  EstimationType getEstimationType() { return m_eType; }
  void setEstimationType(EstimationType et) { m_eType = et; }

  int getPointSize() { return m_pointSize; }
  bool useSNRSize() { return m_useSNRSize; }

  inline double getTension() { return m_tension; }
  inline void setTension(double t) { m_cathGuiSetup.tensionSpinBox->setValue(t); }

  inline double getContinuity() { return m_continuity; }
  inline void setContinuity(double c) { m_cathGuiSetup.continuitySpinBox->setValue(c); }

  inline double getTipValue() { return m_tipValue; }
  inline void setTipValue(double val) { m_cathGuiSetup.tipSpinBox->setValue(val); }

  inline double getEndValue() { return m_endValue; }
  inline void setEndValue(double val) { m_cathGuiSetup.endSpinBox->setValue(val); }

  inline double getSplineThickness() { return m_splineThickness; }
  inline void setSplineThickness(double thickness) { m_cathGuiSetup.thicknessSpinBox->setValue(thickness); }

  inline int getBadSNR() { return m_badSNR; }
  inline void setBadSNR(int value) { badSNRChanged(value); }

  inline int getGoodSNR() { return m_goodSNR; }
  inline void setGoodSNR(int value) { goodSNRChanged(value); }

 public slots:
  void splinePropertyDialog();
  void pointPropertyDialog();
  void tipPropertyDialog();

  void pointSizeChanged(int size);

  void useSNRSizeChanged(int);

  void tableCellChanged(int row, int col);

  void updateCoilTable();

  void splineThicknessChanged(double thickness);
  void tensionChanged(double tension);
  void continuityChanged(double continuity);
  void tipValueChanged(double tip);
  void endValueChanged(double end);
  void badSNRChanged(int value);
  void goodSNRChanged(int value);

signals:
  void updateCoilTableSignal();
 protected:
  // Functions
  void setupGUI();
  void cleanupGUI();
  
  int getNewCoilID();



  //! A hash of coild ID with the coil data
  QHash<int, CathCoilData> m_coilList;

  //! Storage for coils that are not visible.
  QHash<int, CathCoilData> m_discardCoilList;

  //! A hash with coil location and coil ID. Only contains the locations for visible coils.
  QMultiMap<int, int> m_coilLocations;

  //! A list of all the coil IDs. This includes visible and not visible coils...
  QList<int> m_coilIDList;

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

  //! The tension of the spline.
  double m_tension;

  //! the continuity of the spline at the catheter points.
  double m_continuity;

  double m_tipValue;
  double m_endValue;

  double m_splineThickness;

  //! Use the SNR value to determine the size of the rendered coil.
  bool m_useSNRSize;
  int m_badSNR;
  int m_goodSNR;

  //! The objects that sets the widgets for the cath options.
  Ui::cathOptions m_cathGuiSetup;
};

#endif 
