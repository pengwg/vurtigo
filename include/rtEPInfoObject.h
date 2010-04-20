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

#include "rtEPPropertyPointList.h"

class rtEPInfoObject
{
  public:
    rtEPInfoObject();
    ~rtEPInfoObject();

    //! Get the pointer to the color transfer function
    vtkColorTransferFunction* getColorFunction() { return m_defaultColorFunc; }

    //! Add an EP info point to the list of saved points.
    void addInfoPoint(rtEPPropertyPointList::InfoPoint p, QString propName);

    bool getInfoPoint(double,double,double,rtEPPropertyPointList::InfoPoint&, QString propName);
    bool removeInfoPoint(double,double,double,rtEPPropertyPointList::InfoPoint&, QString propName);
    void clearPointList(QString propName);

    bool updateScalars(vtkPolyData*);

  protected:

    void cleanupHash();

    //! The default color transfer function
    vtkColorTransferFunction* m_defaultColorFunc;

    //! Hash table for the different properties.
    /*!
      Each property has its own point list.
      */
    QHash<QString, rtEPPropertyPointList*> m_pointLists;

    QString m_currentPropertyName;
};

#endif // RTEPINFOOBJECT_H
