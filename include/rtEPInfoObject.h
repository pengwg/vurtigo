#ifndef RTEPINFOOBJECT_H
#define RTEPINFOOBJECT_H

#include <QHash>
#include <vtkColorTransferFunction.h>
#include <vtkPolyData.h>
#include <vtkAppendPolyData.h>
#include <vtkSphereSource.h>

#include "rtEPPropertyPointList.h"
#include "rtNamedInfoPointData.h"

class rtEPInfoObject
{
  public:
    rtEPInfoObject();
    ~rtEPInfoObject();

    //! Get the pointer to the color transfer function
    vtkColorTransferFunction* getColorFunction() { return m_defaultColorFunc; }

    //! Add an EP info point to the list of saved points.
    void addInfoPoint(rtNamedInfoPointData p);

    bool getInfoPoint(double,double,double,rtNamedInfoPointData&, QString propName);
    bool removeInfoPoint(double,double,double,rtNamedInfoPointData&, QString propName);

    //! Remove all the point from one type of list.
    void clearPointList(QString propName);

    //! Remove all of the point from all of the lists.
    void clearAllPointList();

    bool updateScalars(vtkPolyData*);

    void setCurrentPropName(QString prop) { if(m_pointLists.contains(prop)) m_currentPropertyName = prop; }
    QString getCurrentPropName() { return m_currentPropertyName; }

    void setEffectRadius(unsigned int r) { if (r!=0) m_radius = r; }
    unsigned int getEffectRadius() { return m_radius; }

    vtkPolyData* getPointPolyData();

  protected:

    //! Cleanup the hash data structure.
    void cleanupHash();

    //! Cleanup the list of sphere objects
    void cleanupSphereList();

    //! Cleanup the list of scalars for the sphere list.
    void cleanupSphereScalarList();

    //! Update the poly data object based on the points in the current property.
    void updatePointPolyData();

    //! The default color transfer function
    vtkColorTransferFunction* m_defaultColorFunc;

    //! Hash table for the different properties.
    /*!
      Each property has its own point list.
      */
    QHash<QString, rtEPPropertyPointList*> m_pointLists;

    QString m_currentPropertyName;

    //! The effect radius for each sampled point.
    unsigned int m_radius;

    //! The poly data for the point set.
    vtkAppendPolyData* m_pointPolyData;

    //! List of sphere objects currently used as the point set.
    QList<vtkSphereSource*> m_sphereList;

    //! List of scalars that go with the spheres to define colors.
    QList<vtkDataArray*> m_sphereScalarList;
};

#endif // RTEPINFOOBJECT_H
