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
