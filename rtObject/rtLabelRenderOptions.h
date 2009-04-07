#ifndef RT_LABEL_RENDER_OPTIONS_H
#define RT_LABEL_RENDER_OPTIONS_H

#include "rtRenderOptions.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>

class rtLabelRenderOptions : public rtRenderOptions {
 public:
  rtLabelRenderOptions();
  ~rtLabelRenderOptions();

  void apply();
  void update();

  QLineEdit* getChooseTextEdit() { return m_chooseTextEdit; }

 protected:
  void setupGUI();
  void cleanupGUI();

 private:
  //! The master vertical layout
  QVBoxLayout* m_vbox;

  //! Just a title label.
  QLabel* m_nameLabel;

  //! Layout for text choice
  QHBoxLayout* m_chooseTextLayout;
  //! Widget for text choice
  QWidget* m_chooseTextWidget;
  //! Label for text choice
  QLabel* m_chooseTextLabel;
  //! Edit box for text choice
  QLineEdit* m_chooseTextEdit;
};

#endif
