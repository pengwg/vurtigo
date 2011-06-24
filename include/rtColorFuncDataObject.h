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
