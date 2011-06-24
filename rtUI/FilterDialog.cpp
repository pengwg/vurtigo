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
#include <QList>
#include <QSet>

#include "FilterDialog.h"
#include "ui_FilterDialog.h"

FilterDialog::FilterDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::FilterDialog),
    m_ptList(NULL)
{
    m_ui->setupUi(this);

    m_possibleOps[0] = "Between";
    m_possibleOps[1] = "Less Than/EQ";
    m_possibleOps[2] = "Greater Than/EQ";

    for (int ix1=0; ix1<3; ix1++) {
      m_ui->filterOpComboBox->addItem(m_possibleOps[ix1]);
    }

    connect(m_ui->filterOpComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(filterOpChanged(int)));
    connect(m_ui->filterByComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(filterByChanged(int)));
    connect(m_ui->filterPushButton, SIGNAL(clicked()), this, SLOT(filterButton()));
    connect(m_ui->cancelPushButton, SIGNAL(clicked()), this, SLOT(cancelButton()));
}

FilterDialog::~FilterDialog()
{
    delete m_ui;
}

void FilterDialog::setPointList(QHash<int, rtNamedInfoPointData>* ptList) {
  if (!ptList) return;
  if (ptList->size() <= 0) return;

  m_ptList = ptList;

  setupCategories();
  resetMinMaxValues();
}

//////////////////
// Public Slots
//////////////////

void FilterDialog::filterButton() {
  QString text = m_ui->filterByComboBox->currentText();
  int op = m_ui->filterOpComboBox->currentIndex();
  bool delMissing = m_ui->deleteMissingCheckBox->isChecked();
  double minVal=0, maxVal=0;

  if (text != "" && op >= 0) {
    switch(op) {
    case 0:
      // Between
      minVal = m_ui->minDoubleSpinBox->value();
      maxVal = m_ui->maxDoubleSpinBox->value();
      break;
    case 1:
      // Less
      minVal = m_ui->minDoubleSpinBox->minimum();
      maxVal = m_ui->maxDoubleSpinBox->value();
      break;
    case 2:
      // Greater
      minVal = m_ui->minDoubleSpinBox->value();
      maxVal = m_ui->maxDoubleSpinBox->maximum();
      break;
    default:
      break;
    }
  }

  QList<int> keys = m_ptList->keys();

  for (int ix1=0; ix1<keys.size(); ix1++) {
    if( (*m_ptList)[keys[ix1]].tagExists(text) ) {
      double val = (*m_ptList)[keys[ix1]].getValue(text);

      // If it falls outside remove the point.
      if (val < minVal || val > maxVal) {
        m_ptList->remove(keys[ix1]);
      }

    } else if ( delMissing ) {
      // Tag does not exist but the points with this value missing should be deleted.
      m_ptList->remove(keys[ix1]);
    }
  }

  this->done(QDialog::Accepted);
}

void FilterDialog::cancelButton() {
  this->done(QDialog::Rejected);
}

void FilterDialog::filterOpChanged(int op) {
  if (op < 0) return;

  switch(op) {
  case 0:
    // Between
    m_ui->minDoubleSpinBox->setVisible(true);
    m_ui->maxDoubleSpinBox->setVisible(true);
    m_ui->andLabel->setVisible(true);
    break;
  case 1:
    // Less
    m_ui->minDoubleSpinBox->setVisible(false);
    m_ui->maxDoubleSpinBox->setVisible(true);
    m_ui->andLabel->setVisible(false);
    break;
  case 2:
    // Greater
    m_ui->minDoubleSpinBox->setVisible(true);
    m_ui->maxDoubleSpinBox->setVisible(false);
    m_ui->andLabel->setVisible(false);
    break;
  default:
    break;
  }
}

void FilterDialog::filterByChanged(int by) {
  if (by < 0) return;

  QString text = m_ui->filterByComboBox->currentText();
  if (text != "") {
    resetMinMaxValues();
  }
}

//////////////////
// Protected
//////////////////

void FilterDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void FilterDialog::setupCategories() {
  if (!m_ptList) return;

  m_ui->filterByComboBox->clear();

  if (m_ptList->size() <= 0) {
    return;
  }

  QSet<QString> categorySet;
  QSet<QString> tempSet;
  QList<int> keys = m_ptList->keys();

  categorySet.clear();
  for (int ix1=0; ix1<keys.size(); ix1++) {
    tempSet = QSet<QString>::fromList( (*m_ptList)[keys[ix1]].getTagList());
    categorySet.unite(tempSet);
  }

  QSetIterator<QString> i(categorySet);
  while (i.hasNext()) {
    m_ui->filterByComboBox->addItem(i.next());
  }

  // Disable the filter button if there are no items to filter by.
  if (m_ui->filterByComboBox->count() <= 0) {
    m_ui->minDoubleSpinBox->setEnabled(false);
    m_ui->maxDoubleSpinBox->setEnabled(false);
    m_ui->filterPushButton->setEnabled(false);
  } else {
    m_ui->minDoubleSpinBox->setEnabled(true);
    m_ui->maxDoubleSpinBox->setEnabled(true);
    m_ui->filterPushButton->setEnabled(true);
  }
}

void FilterDialog::resetMinMaxValues() {
  if (!m_ptList) return;
  if (m_ptList->size() <= 0) return;
  if (m_ui->filterByComboBox->count() <= 0) return;

  double minVal=0, maxVal=0;
  bool firstValue = true;

  int index = m_ui->filterByComboBox->currentIndex();
  QString text = m_ui->filterByComboBox->currentText();
  if (index!=-1 && text != "") {
    QList<int> keys = m_ptList->keys();

    for (int ix1=0; ix1<keys.size(); ix1++) {
      if ((*m_ptList)[keys[ix1]].tagExists(text)) {
        double val = (*m_ptList)[keys[ix1]].getValue(text);
        if (firstValue) {
          // First valid tag
          firstValue = false;
          minVal = val;
          maxVal = val;
        } else {
          if (val < minVal) minVal = val;
          else if (val > maxVal) maxVal = val;
        }
      }
    }

    m_ui->minDoubleSpinBox->setMinimum(minVal);
    m_ui->minDoubleSpinBox->setMaximum(maxVal);

    m_ui->maxDoubleSpinBox->setMinimum(minVal);
    m_ui->maxDoubleSpinBox->setMaximum(maxVal);

    m_ui->minDoubleSpinBox->setValue(minVal);
    m_ui->maxDoubleSpinBox->setValue(maxVal);
  }
}
