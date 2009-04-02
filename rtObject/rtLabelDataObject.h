#ifndef RT_LABEL_DATA_OBJECT_H
#define RT_LABEL_DATA_OBJECT_H

#include "rtDataObject.h"
#include <QString>
#include "vtkTextProperty.h"

class rtLabelDataObject : public rtDataObject {
 public:
  rtLabelDataObject();
  ~rtLabelDataObject();

  void setText(QString text);
  QString getText();

  void setColor(double, double, double);
  void setOpacity(double);

  vtkTextProperty* getTextProperty();

 protected:
  QString m_labelText;
  vtkTextProperty* m_textProp;
 private:
};

#endif
