#ifndef MESHFILEREADER_H
#define MESHFILEREADER_H

#include <QString>
#include "MeshPointSet.h"

class MeshFileReader
{
public:
  MeshFileReader();
  ~MeshFileReader();

  bool setDirectory(QString dirPath);

  QString getDefaultName() { return m_defaultName; }
  QString getComments() { return m_comments; }

  MeshPointSet* getPointSet(int phase) { if (m_pointSet.contains(phase)) return m_pointSet.value(phase); else return 0; }

  void setNumPhases(int numPhases) { m_numPhases = numPhases; }
  int getNumPhases() { return m_numPhases; }

protected:
  QString m_defaultName;
  QString m_comments;

  //! One point set per cardiac phase.
  QMap<int, MeshPointSet*> m_pointSet;

  int m_numPhases;
};

#endif // MESHFILEREADER_H
