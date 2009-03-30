#ifndef RT_RENDER_OBJECT_H
#define RT_RENDER_OBJECT_H

#include <QString>
#include <vtkProp.h>
#include "rtDataObject.h"

class rtRenderObject {
 public:
  //! Destructor
  ~rtRenderObject();

  vtkProp* get3DPipeline();
  vtkProp* get2DPipeline();
  rtDataObject* getDataObject();
  QString getName();
  rtConstants::rtObjectType getObjectType();


  void setDataObject(rtDataObject* dataObj);
  

 protected:
  //! The rtRenderObject constructor.
  /*!
    The protected rtRenderObject constructor. The rtRenderObject class is not meant to be instantiated. The proper use of this class is to derive from it a specific type of renderer. 
  */
  rtRenderObject();

  void set3DPipeline(vtkProp* pipe3D);
  void set2DPipeline(vtkProp* pipe2D);
  void setName(QString renName);
  void setObjectType(rtConstants::rtObjectType objType);

 private:
  vtkProp *m_pipe3D;
  vtkProp *m_pipe2D;
  rtDataObject* m_dataObj;
  QString m_renderName;
  rtConstants::rtObjectType m_objType;
};

#endif
