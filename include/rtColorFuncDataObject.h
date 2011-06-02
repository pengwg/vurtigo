#ifndef RT_COLOR_FUNC_DATA_OBJECT_H
#define RT_COLOR_FUNC_DATA_OBJECT_H

#include "rtDataObject.h"
#include "VtkColorTable.h"

#include <vtkSmartPointer.h>
#include <vtkColorTransferFunction.h>

//! The data portion of the color function
/*!
  Holds the points that determine how scalars will be colored.
  */
class rtColorFuncDataObject : public rtDataObject
{
Q_OBJECT

public:
  rtColorFuncDataObject();
  ~rtColorFuncDataObject();

  void update();

  //! Get the value of the current color function.
  vtkColorTransferFunction* getColorFunction() {
    m_ctf->DeepCopy(m_table->getColorFunction());
    return m_ctf;
  }
  bool setColorFunction(vtkColorTransferFunction* ctf);

  virtual bool saveFile(QFile *file);
  virtual bool loadFile(QFile *file);

 protected:
  // Functions
  void setupGUI();
  void cleanupGUI();
  
  vtkSmartPointer<vtkColorTransferFunction> m_ctf;

  VtkColorTable* m_table;
  QBoxLayout *m_mainLayout;

  void readFuncData(QXmlStreamReader* reader);
  void readPointData(QXmlStreamReader* reader);
};

#endif 
