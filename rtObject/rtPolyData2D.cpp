#include <vtkMath.h>

#include "rtPolyData2D.h"

rtPolyData2D::rtPolyData2D() {
  // Create the objects
  m_polyData = vtkPolyData::New();
  m_mapper = vtkPolyDataMapper::New();
  m_actor = vtkActor::New();
  m_property = vtkProperty::New();

  m_transform = vtkTransform::New();
  m_transform->Identity();

  m_xsize = 1.0;
  m_ysize = 1.0;

  // Connect the pipeline
  m_mapper->SetInput(m_polyData);
  m_actor->SetMapper(m_mapper);
  m_actor->SetProperty(m_property);
}

rtPolyData2D::~rtPolyData2D() {
  if (m_polyData) m_polyData->Delete();
  if (m_mapper) m_mapper->Delete();
  if (m_actor) m_actor->Delete();
  if (m_property) m_property->Delete();
  if (m_transform) m_transform->Delete();
}

void rtPolyData2D::setTransform( vtkTransform* t ) {
  m_transform->SetMatrix(t->GetMatrix());

  double orig[3], pt1[3], pt2[3], opp[3];

  orig[0]=0.0; orig[1]=0.0; orig[2]=0.0;
  pt1[0]=1.0*m_xsize; pt1[1]=0.0; pt1[2]=0.0;
  pt2[0]=0.0; pt2[1]=1.0*m_ysize; pt2[2]=0.0;
  opp[0]=1.0*m_xsize; opp[1]=1.0*m_ysize; opp[2]=0.0;

  m_transform->TransformPoint(orig, orig);
  m_transform->TransformPoint(pt1, pt1);
  m_transform->TransformPoint(pt2, pt2);
  m_transform->TransformPoint(opp, opp);

  // Setup the parameters.
  for (int ix1=0; ix1<3; ix1++) {
    m_origin[ix1] = orig[ix1];
    m_pt1[ix1] = pt1[ix1];
    m_pt2[ix1] = pt2[ix1];
    m_opposite[ix1] = opp[ix1];
    m_midpoint[ix1] = (m_origin[ix1]+m_opposite[ix1])/2.0f;
  }

  m_ysize = sqrt(vtkMath::Distance2BetweenPoints(m_pt2, m_origin));
  m_xsize = sqrt(vtkMath::Distance2BetweenPoints(m_pt1, m_origin));

  update();
}

void rtPolyData2D::setSize( double xsize, double ysize ) {
  m_xsize = xsize;
  m_ysize = ysize;

  double orig[3], pt1[3], pt2[3], opp[3];

  orig[0]=0.0; orig[1]=0.0; orig[2]=0.0;
  pt1[0]=1.0*m_xsize; pt1[1]=0.0; pt1[2]=0.0;
  pt2[0]=0.0; pt2[1]=1.0*m_ysize; pt2[2]=0.0;
  opp[0]=1.0*m_xsize; opp[1]=1.0*m_ysize; opp[2]=0.0;

  m_transform->TransformPoint(orig, orig);
  m_transform->TransformPoint(pt1, pt1);
  m_transform->TransformPoint(pt2, pt2);
  m_transform->TransformPoint(opp, opp);

  // Setup the parameters.
  for (int ix1=0; ix1<3; ix1++) {
    m_origin[ix1] = orig[ix1];
    m_pt1[ix1] = pt1[ix1];
    m_pt2[ix1] = pt2[ix1];
    m_opposite[ix1] = opp[ix1];
    m_midpoint[ix1] = (m_origin[ix1]+m_opposite[ix1])/2.0f;
  }

  update();
}

void rtPolyData2D::setCorners(double orig[3], double pt1[3], double opp[3], double pt2[3]) {

  // Setup the parameters.
  for (int ix1=0; ix1<3; ix1++) {
    m_origin[ix1] = orig[ix1];
    m_pt1[ix1] = pt1[ix1];
    m_pt2[ix1] = pt2[ix1];
    m_opposite[ix1] = opp[ix1];
    m_midpoint[ix1] = (m_origin[ix1]+m_opposite[ix1])/2.0f;
  }

  m_xsize = sqrt(vtkMath::Distance2BetweenPoints(m_pt1, m_origin));
  m_ysize = sqrt(vtkMath::Distance2BetweenPoints(m_pt2, m_origin));
  // Update the sizes

  // Update the transform too.
  m_transform->Identity();

  double xd[3];
  double yd[3];
  double zd[3];

  for (int ix1=0; ix1<3; ix1++) {
    xd[ix1]=pt1[ix1]-orig[ix1];
    yd[ix1]=pt2[ix1]-orig[ix1];
  }

  vtkMath::Normalize(xd);
  vtkMath::Normalize(yd);
  vtkMath::Cross(xd, yd, zd);

  vtkMatrix4x4 *temp = vtkMatrix4x4::New();
  for (int ix1=0; ix1<3; ix1++) {
    temp->SetElement(ix1, 0, xd[ix1]);
    temp->SetElement(ix1, 1, yd[ix1]);
    temp->SetElement(ix1, 2, zd[ix1]);
    temp->SetElement(ix1, 3, orig[ix1]);
  }
  m_transform->SetMatrix(temp);
  temp->Delete();

  update();
}

void rtPolyData2D::setColor(double r, double g, double b) {
  m_property->SetColor(r,g,b);
}

void rtPolyData2D::getOrigin( double orig[3] ) {
  orig[0] = m_origin[0];
  orig[1] = m_origin[1];
  orig[2] = m_origin[2];
}

void rtPolyData2D::getMidpoint( double mid[3] ) {
  mid[0] = m_midpoint[0];
  mid[1] = m_midpoint[1];
  mid[2] = m_midpoint[2];
}

void rtPolyData2D::getOpposite( double opp[3] ) {
  opp[0] = m_opposite[0];
  opp[1] = m_opposite[1];
  opp[2] = m_opposite[2];
}

void rtPolyData2D::getVector1( double v1[3] ) {
  v1[0] = m_origin[0] - m_pt1[0];
  v1[1] = m_origin[1] - m_pt1[1];
  v1[2] = m_origin[2] - m_pt1[2];
}

void rtPolyData2D::getVector2( double v2[3] ) {
  v2[0] = m_origin[0] - m_pt2[0];
  v2[1] = m_origin[1] - m_pt2[1];
  v2[2] = m_origin[2] - m_pt2[2];
}

void rtPolyData2D::getNormal( double n[3] ) {
  double v1[3];
  double v2[3];
  getVector1(v1);
  getVector2(v2);
  vtkMath::Cross(v1, v2, n);
}

void rtPolyData2D::update() {
}
