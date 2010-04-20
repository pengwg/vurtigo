#ifndef RTEPPROPERTYPOINTLIST_H
#define RTEPPROPERTYPOINTLIST_H

#include <QString>
#include <QList>

class rtEPPropertyPointList
{
 public:

  //! Structure that contains some kind of EP info at a certain point.
  struct InfoPoint{
    double location[3];
    double property;
  };

  //! Constructor
  rtEPPropertyPointList(QString name);
  ~rtEPPropertyPointList();

  //! Add an info point to this list.
  /*!
    Adding a point will update the min and max values for this property.
    */
  void addPointToList(InfoPoint p);

  //! Get the point at the specified set of 3d coordinates
  /*!
    \param x The x coord for the search point.
    \param y The y coord for the search point.
    \param z The z coord for the search point.
    \param p The point that is found will be stored in p.
    \return True if a point is found at that location. False otherwise.
    */
  bool getInfoPoint(double x,double y,double z,InfoPoint& p);

  bool removeInfoPoint(double,double,double,InfoPoint&);

  void clearPointList();

  //! Get the number of points in this list.
  int getNumPoints() { return m_infoList.size(); }

  double getMinValue() { return m_minPropValue; }
  double getMaxValue() { return m_maxPropValue; }

  //! Get the point at a position in the list.
  InfoPoint getPointAt(int pos) { return m_infoList.at(pos); }

 protected:

  //! The name of this property
  QString m_propName;
  //! List of points that hold EP info
  QList<InfoPoint> m_infoList;
  //! Max value of the property
  double m_maxPropValue;
  //! Minimum value of the property
  double m_minPropValue;
};

#endif // RTEPPROPERTYPOINTLIST_H
