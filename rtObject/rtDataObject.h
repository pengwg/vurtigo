#ifndef RT_DATA_OBJECT_H
#define RT_DATA_OBJECT_H

#include "rtRenderOptions.h"
#include <QString>

class rtDataObject {
 public:
  ~rtDataObject();

  virtual rtRenderOptions* getRenderOptions();
  virtual void setRenderOptions(rtRenderOptions* renOpt);

  int getId();
  void setId(int id);
  
  QString getObjName();
  void setObjName(QString name);

  rtConstants::rtObjectType getObjectType();
  void setObjectType(rtConstants::rtObjectType ot);

 protected:
  rtDataObject();

 private:
  rtRenderOptions* m_renderOptions;
  int m_objId;
  QString m_objName;
  rtConstants::rtObjectType m_objType;
};

#endif
