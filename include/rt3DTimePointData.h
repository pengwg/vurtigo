#ifndef RT3DTIMEPOINTDATA_H
#define RT3DTIMEPOINTDATA_H

#include "rtBasic3DPointData.h"

class rt3DTimePointData : public rtBasic3DPointData
{
public:
  rt3DTimePointData();

  //! Copy constructor
  rt3DTimePointData(const rt3DTimePointData& sp);
  //! Equals operator. Checks if two objects are equal.
  bool operator==(const rt3DTimePointData &other) const;
  //! Assignment operator
  rt3DTimePointData& operator=(const rt3DTimePointData& sp);

  inline void setTriggerDelay(unsigned int td) { m_triggerDelay = td; }
  inline void setPhase(unsigned int phase) { m_phase = phase; }
  inline void setResp(unsigned int resp) { m_resp = resp; }

  inline unsigned int getTriggerDelay() { return m_triggerDelay; }
  inline unsigned int getPhase() { return m_phase; }
  inline unsigned int getResp() { return m_resp; }

protected:
  unsigned int m_triggerDelay;
  unsigned int m_phase;
  unsigned int m_resp;
};

#endif // RT3DTIMEPOINTDATA_H
