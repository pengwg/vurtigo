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
