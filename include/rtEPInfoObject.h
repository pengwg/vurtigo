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

#include <QList>

#include "vtkColorTransferFunction.h"
#include "vtkPolyData.h"

class rtEPInfoObject
{
  public:

  //! Structure that contains some kind of EP info at a certain point.
    struct InfoPoint{
      double location[3];
      int property;
    };

    rtEPInfoObject();
    ~rtEPInfoObject();

    //! Get the pointer to the color transfer function
    vtkColorTransferFunction* getColorFunction() { return m_defaultColorFunc; }

    //! Add an EP info point to the list of saved points.
    void addInfoPoint(InfoPoint p);

    bool getInfoPoint(double,double,double,InfoPoint&);
    void clearPointList();
    bool updateScalars(vtkPolyData*);

  protected:

    //! The default color transfer function
    vtkColorTransferFunction* m_defaultColorFunc;

    //! List of points that hold EP info
    QList<InfoPoint> m_infoList;

    //! Max value of the property
    int m_maxPropValue;
    //! Minimum value of the property
    int m_minPropValue;
};

#endif // RTEPINFOOBJECT_H
