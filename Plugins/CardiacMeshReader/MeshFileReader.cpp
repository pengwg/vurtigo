#include "MeshFileReader.h"

#include <QDir>
#include <QTextStream>

#include <iostream>

MeshFileReader::MeshFileReader() {
  m_defaultName = "";
  m_comments = "";
  m_pointSet.clear();

  // Default the number of phases to 20.
  m_numPhases = 20;
}

MeshFileReader::~MeshFileReader() {
  QList<MeshPointSet*> temp = m_pointSet.values();
  while(!temp.empty()) delete temp.takeFirst();
  m_pointSet.clear();
}

bool MeshFileReader::setDirectory(QString dirPath) {
  QDir fileDir(dirPath);
  QStringList files;
  QStringList filters;
  filters << "*.txt";

  if (!fileDir.exists()) {
    std::cout << "No such directory: " << dirPath.toStdString() << std::endl;
    return false;
  }


  fileDir.setFilter(QDir::NoSymLinks | QDir::NoDotAndDotDot | QDir::Readable | QDir::Files);
  fileDir.setNameFilters(filters);
  files = fileDir.entryList();

  if (files.count() <= 0) {
    std::cout << "Directory found, but no text files could be found inside:  " << dirPath.toStdString() << std::endl;
    return false;
  }

  for (int ix1=0; ix1<files.count(); ix1++) {
    QString currFile;
    QStringList fileNameSplit;
    unsigned int fileNum;
    bool convOK;
    MeshPointSet::PointType ptType = MeshPointSet::PT_ICONTOUR;
    double xVal, yVal;

    currFile = files[ix1];
    fileNameSplit = currFile.split("-");

    if (fileNameSplit.count() < 4) {
      std::cout << "File Name Format not valid for: " << currFile.toStdString() << std::endl;
      continue;
    }

    fileNum = fileNameSplit[2].toUInt(&convOK);
    if (!convOK) {
      std::cout << "Number conversion not valid for: " << currFile.toStdString() << std::endl;
      continue;
    }

    if (fileNameSplit[3] == "icontour") {
      ptType = MeshPointSet::PT_ICONTOUR;
    } else if (fileNameSplit[3] == "ocontour") {
      ptType = MeshPointSet::PT_OCONTOUR;
    } else {
      std::cout << "Contour type not valid: " << currFile.toStdString() << " " << fileNameSplit[3].toStdString() << std::endl;
      continue;
    }

    QString fPath;
    fPath = fileDir.absoluteFilePath(currFile);
    QFile file(fPath);

    if (!file.exists()) {
      std::cout << "The text file: " << fPath.toStdString() << " does not exist." << std::endl;
      continue;
    }
    int phase = 0;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      QTextStream tStream(&file);
      int ptNum = 0;
      while(!tStream.atEnd()) {
        tStream >> xVal;
        tStream >> yVal;
        phase = (fileNum-1)%m_numPhases;

        if (!m_pointSet.contains(phase)) m_pointSet.insert(phase, new MeshPointSet());

        if( tStream.status() == QTextStream::Ok)
          m_pointSet.value(phase)->addPointToSet(ptType, (fileNum-1)/m_numPhases, ptNum, xVal, yVal);
        ptNum++;
      }
      file.close();
    }

    m_defaultName = fileDir.dirName();
  }
  return true;
}
