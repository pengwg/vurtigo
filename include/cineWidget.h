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
#ifndef CINEWIDGET_H
#define CINEWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QList>

#include "ui_cineWidget.h"

class CineWidget : public QWidget {
Q_OBJECT
  public:
    CineWidget();
    ~CineWidget();

    //! Set the list of elements that defined the trigger times for each frame.
    /*!
      Setting the trigger list will reset the current phase to zero.
      \param trigList The new trigger list.
    */
    void setTriggerList(QList<int> trigList);

    //! Add a trigger to the list.
    void addTrigger(int trig);

  public slots:
    void backOne();
    void forwardOne();
    void goToFirst();
    void goToLast();
    void loopButtonChanged();
    void sliderValueChanged(int);
    void nextVisibleComponent();
  signals:
    void phaseChanged(int);
    void triggerChanged(int);

  protected:
    //! The objects that are built into the gui
    Ui::cineWidget m_guiObjects;

    //! A map of phases to trigger times. Empty by default.
    QList<int> m_triggerTimes;

    //! The current phase.
    int m_currentPhase;

    //! Timer that counts to the next frame.
    QTimer *m_cineFrameTimer;
};

#endif // CINEWIDGET_H
