#include "rtWindowLevelDialog.h"


rtWindowLevelDialog::rtWindowLevelDialog(QString fName) {
  m_tempData = vtkImageData::New();
}


rtWindowLevelDialog::~rtWindowLevelDialog() {
  m_tempData->Delete();
}


void rtWindowLevelDialog::setImageData(vtkImageData* dat) {
  if(!dat) return;
  m_tempData->DeepCopy(dat);
}
