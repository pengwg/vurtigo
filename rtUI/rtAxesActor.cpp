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

#include <rtAxesActor.h>

#include <vtkActor.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkObjectFactory.h>

#include <cmath>

vtkStandardNewMacro(rtAxesActor);

// Overloaded to make the axes actor orientable. The vtkAxesActor does not compute its
// bounds in a way that allows it to be re-oriented.
double *rtAxesActor::GetBounds()
{
  double bounds[6];
  int i;

  this->XAxisShaft->GetBounds(this->Bounds);

  this->YAxisShaft->GetBounds(bounds);
  for ( i = 0; i < 3; ++i )
    this->Bounds[2*i+1] = std::max(std::abs(bounds[2*i+1]), std::abs(this->Bounds[2*i+1]));

  this->ZAxisShaft->GetBounds(bounds);
  for ( i = 0; i < 3; ++i )
    this->Bounds[2*i+1] = std::max(std::abs(bounds[2*i+1]), std::abs(this->Bounds[2*i+1]));

  this->XAxisTip->GetBounds(bounds);
  for ( i = 0; i < 3; ++i )
    this->Bounds[2*i+1] = std::max(std::abs(bounds[2*i+1]), std::abs(this->Bounds[2*i+1]));

  this->YAxisTip->GetBounds(bounds);
  for ( i = 0; i < 3; ++i )
    this->Bounds[2*i+1] = std::max(std::abs(bounds[2*i+1]), std::abs(this->Bounds[2*i+1]));

  this->ZAxisTip->GetBounds(bounds);
  for ( i = 0; i < 3; ++i )
    this->Bounds[2*i+1] = std::max(std::abs(bounds[2*i+1]), std::abs(this->Bounds[2*i+1]));

  double dbounds[6];
  (vtkPolyDataMapper::SafeDownCast(this->YAxisShaft->GetMapper()))->GetInput()->GetBounds( dbounds );
  for ( i = 0; i < 3; ++i )
    this->Bounds[2*i+1] = std::max(std::abs(dbounds[2*i+1]), std::abs(this->Bounds[2*i+1]));

  for ( i = 0; i < 3; ++i )
    this->Bounds[2*i] = -this->Bounds[2*i+1];

  return this->Bounds;
}
