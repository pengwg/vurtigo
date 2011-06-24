/***************************************************************************
Vurtigo: Visualization software for interventional applications in medicine


Copyright (c) 2011, Sunnybrook Research Institute
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Sunnybrook Research Institute nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Sunnybrook Research Institute BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************************/
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
