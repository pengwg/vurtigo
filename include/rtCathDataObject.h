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

class rtColorFuncDataObject;

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
      QHash<QString,double> coilPropValues;
      bool visible;
  };

  enum EstimationType {
    ET_MEAN,
    ET_WTMEAN,
    ET_BEST
  };

  rtCathDataObject();
  ~rtCathDataObject();

  void update();

  virtual bool saveFile(QFile* file);
  virtual bool loadFile(QFile* file);

  // Coil specific
  int addCoil(int loc);
  bool setCoilPropValue(int coilID, QString property, double value);
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
  bool getValueAtLocation(int loc, QString property, double &value);
  bool getColorAtLocation(int loc, QString property, double rgb[3]);

  //Property
  vtkProperty* getSplineProperty() { return m_splineProperty; }
  vtkProperty* getPointProperty() { return m_pointProperty; }
  vtkProperty* getTipProperty() { return m_tipProperty; }
  void refreshProperties() { m_pointPropertyDlg->settingsChanged(); }

  //! Add a new catheter property
  bool addCathProperty(QString property);


  // Estimation Type
  EstimationType getEstimationType() { return m_eType; }
  void setEstimationType(EstimationType et) { m_eType = et; }

  int getPointSize() { return m_pointSize; }

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

  inline QString getCurrProperty() {return m_currProperty; }
  inline rtColorFuncDataObject* getCurrColor() {return m_currColor; }


 public slots:
  void splinePropertyDialog();
  void pointPropertyDialog();
  void tipPropertyDialog();

  void pointSizeChanged(int size);

  //! The property to color by has changed
  void propertyChanged();
  //! The color function has changed
  void colorChanged(int);

  void tableCellChanged(int row, int col);

  void updateCoilTable();
  void updatePropertyBox();
  void updateColorBox();

  void splineThicknessChanged(double thickness);
  void tensionChanged(double tension);
  void continuityChanged(double continuity);
  void tipValueChanged(double tip);
  void endValueChanged(double end);

  void gateBoundsChanged();

signals:
  void updateCoilTableSignal();
  void cathPropsChanged();
 protected:
  // Functions
  void setupGUI();
  void cleanupGUI();
  
  int getNewCoilID();



  //! A hash of coild ID with the coil data
  QHash<int, CathCoilData> m_coilList;

  //! Storage for coils that are not visible.
  QHash<int, CathCoilData> m_discardCoilList;

  //! A List of properties
  QList<QString>  m_cathProperties;

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

  //! The currently selected property
  QString m_currProperty;
  //! The currently selected color function
  rtColorFuncDataObject *m_currColor;
  //! A hash to map indices to color function object ids
  QHash<int,int> m_colorIDs;

  //! The objects that sets the widgets for the cath options.
  Ui::cathOptions m_cathGuiSetup;

private:
  bool withinBounds(int val) {return ( (val > m_cathGuiSetup.minBox->value()) && (val < m_cathGuiSetup.maxBox->value()) ); }
};

#endif 
