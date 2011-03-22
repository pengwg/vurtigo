/*******************************************************************************
    Vurtigo: Visualization software for interventional applications in medicine
    Copyright (C) 2009 Sunnybrook Health Sciences Centre

    This file is part of Vurtigo.

    Vurtigo is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/
#include "CathHistoryUI.h"
#include "rtObjectManager.h"
#include "rtBaseHandle.h"
#include "rtApplication.h"

#include <QList>

CathHistoryUI::CathHistoryUI() {
  setupUi(this);
  
  m_points = rtBaseHandle::instance().requestNewObject(rtConstants::OT_3DPointBuffer, "Catheter History");
  
  m_historyRecorder = NULL;

  m_numPoints = pointsSpin->value();
  m_interval = intervalSpin->value();
  m_timer = new QTimer(this);
  m_counter = 0;
  m_set = 0;

  markButton->setEnabled(false);
  pointsSpin->setEnabled(false);
  intervalSpin->setEnabled(false);
  autoSaveBox->setEnabled(false);
  
  updateCheckableStatus();

  populateLists();
  connectSignals();
}

CathHistoryUI::~CathHistoryUI() {
  if (m_historyRecorder != NULL)
    delete m_historyRecorder;
  m_historyRecorder = NULL;   
    
  if (m_points >= 0)
    {
      // delete it somehow
    }

  if (m_timer != NULL)
      delete m_timer;
 
}

void CathHistoryUI::connectSignals() {
  connect( rtApplication::instance().getObjectManager(), SIGNAL(objectCreated(int)), this, SLOT(objectAdded(int)) );
  connect( rtApplication::instance().getObjectManager(), SIGNAL(objectRemoved(int)), this, SLOT(objectRemoved(int)) );

  connect( pointSizeSlider, SIGNAL(valueChanged(int)), this, SLOT(pointSizeChanged(int)) );

  connect( cathComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(cathChanged(int)) );
  
  connect( cathAutoTrackCheckBox, SIGNAL(toggled(bool)), this, SLOT(cathAutoTrackChanged(bool)));

  connect( markButton, SIGNAL(clicked()), this, SLOT(mark()));
  connect( pointsSpin, SIGNAL(valueChanged(int)), this, SLOT(pointsChanged(int)));
  connect ( intervalSpin, SIGNAL(valueChanged(int)), this, SLOT(intervalChanged(int)));
}

void CathHistoryUI::updateCheckableStatus() {
  int cathPos;
  cathPos = cathComboBox->currentIndex();
  if (cathPos >= 0)
  {
      markButton->setEnabled(true);
      pointsSpin->setEnabled(true);
      intervalSpin->setEnabled(true);
      autoSaveBox->setEnabled(true);
  }
  
  cathAutoTrackCheckBox->setEnabled((cathPos < 0) ? false : true);
}

void CathHistoryUI::populateLists() {
  QList<int> cathObjs;
  QList<int> planeObjs;
  rtCathDataObject* cath=NULL;

  cathObjs = rtBaseHandle::instance().getObjectsOfType(rtConstants::OT_Cath);

  cathComboBox->clear();

  for (int ix1=0; ix1<cathObjs.size(); ix1++) {
    cath = static_cast<rtCathDataObject*>(rtBaseHandle::instance().getObjectWithID(cathObjs[ix1]));
    if (cath) {
      m_cathObjectMap.insert(ix1, cath);
      cathComboBox->insertItem(ix1, QString::number(cathObjs[ix1])+cath->getObjName());
    }
  }

  updateCheckableStatus();

}

void CathHistoryUI::objectAdded(int objID) {
  rtDataObject *data=NULL;
  rtCathDataObject *cath=NULL;

  data = rtBaseHandle::instance().getObjectWithID(objID);
  if (!data) return;

  if ( data->getObjectType() == rtConstants::OT_Cath ) {
    cath = static_cast<rtCathDataObject*>(data);
    if (!cath) return;
    m_cathObjectMap.insert(m_cathObjectMap.count(), cath);
    cathComboBox->insertItem(cathComboBox->count(), QString::number(objID)+cath->getObjName());
  }

  updateCheckableStatus();
}

//! The object manager has removed an object. It could be of a type we are interesed in.
void CathHistoryUI::objectRemoved(int objID) {
  int cathPos;

  cathPos = cathComboBox->currentIndex();

  populateLists();

  if ( cathPos >= 0 && cathPos < cathComboBox->count() ) 
    cathComboBox->setCurrentIndex(cathPos);
    
  updateCheckableStatus();
}

void CathHistoryUI::pointSizeChanged(int size) {
  if (m_historyRecorder) {
    m_historyRecorder->setPointSize(size);
  }
}

void CathHistoryUI::cathChanged(int newpos) {
  trackingPairChanged();
  
  updateCheckableStatus();
}

void CathHistoryUI::trackingPairChanged() {
  int cathPos;

  cathPos = cathComboBox->currentIndex();

  if (cathPos < 0 || m_points < 0) {
   // no selection
    markButton->setEnabled(false);
    pointsSpin->setEnabled(false);
    intervalSpin->setEnabled(false);
  } else {
   // valid selection
    markButton->setEnabled(true);
    pointsSpin->setEnabled(true);
    intervalSpin->setEnabled(true);
    
    rtCathDataObject *cath = m_cathObjectMap.value(cathPos);
    rt3DPointBufferDataObject *points = static_cast<rt3DPointBufferDataObject*>(rtBaseHandle::instance().getObjectWithID(m_points));

    if (m_historyRecorder == NULL) {
      m_historyRecorder = new HistoryData(cath, points);
      m_historyRecorder->setPointSize(pointSizeSlider->value());
    } else {
      m_historyRecorder->setCathObject(cath);
    }
  }
  
  updateCheckableStatus();
}

void CathHistoryUI::mark()
{
    if (m_historyRecorder != NULL && m_counter == 0)
    {
        if (m_interval == 0)
        {
            for (int ix1=0; ix1<m_numPoints; ix1++)
                 m_historyRecorder->saveSetPoint(m_set);
            savePointObject();
            savedLabel->setText("Marked " + QString::number(m_numPoints) + " points in set # " + QString::number(m_set));
            m_set++;

        }
        else
        {
            savedLabel->setText("Marking...");
            m_historyRecorder->saveSetPoint(m_set);
            savePointObject();
            m_counter++;
            connect(m_timer, SIGNAL(timeout()), this, SLOT(markNow()));
            m_timer->start(m_interval);
        }

    }
}

void CathHistoryUI::markNow()
{
    if (m_counter >= m_numPoints)
    {
        m_timer->stop();
        m_counter = 0;
        savedLabel->setText("Marked " + QString::number(m_numPoints) + " points in set # " + QString::number(m_set));
        m_set++;
        disconnect(m_timer, SIGNAL(timeout()), this, SLOT(markNow()));

    }
    else
    {
        savedLabel->setText("Marking...");
        m_historyRecorder->saveSetPoint(m_set);
        savePointObject();
        m_counter++;
    }
}

void CathHistoryUI::cathAutoTrackChanged(bool value) {
  m_historyRecorder->setAutoTrack(value);
}

void CathHistoryUI::savePointObject()
{
    if (autoSaveBox->isChecked())
    {
        if (m_historyRecorder->getPointBufferObject()->getFilename().isEmpty())
        {
            filenameLabel->setStyleSheet("QLabel { color : red; }");
            filenameLabel->setText("NO FILE SPECIFIED, NOT SAVING");
        }
        else
        {
            filenameLabel->setStyleSheet("");
            filenameLabel->setText("Saved Points to: " + QFileInfo(m_historyRecorder->getPointBufferObject()->getFilename()).fileName());
            m_historyRecorder->getPointBufferObject()->saveThisObject();
        }
    }
}
