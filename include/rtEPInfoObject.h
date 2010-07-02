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
    void clearPointList(QString propName);

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
