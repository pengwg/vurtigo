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
