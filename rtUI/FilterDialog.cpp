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
    m_possibleOps[1] = "Less Than";
    m_possibleOps[2] = "Greater Than";

    for (int ix1=0; ix1<3; ix1++) {
      m_ui->filterOpComboBox->addItem(m_possibleOps[ix1]);
    }

    connect(m_ui->filterPushButton, SIGNAL(clicked()), this, SLOT(filterButton()));
    connect(m_ui->cancelPushButton, SIGNAL(clicked()), this, SLOT(cancelButton()));
    connect(m_ui->filterOpComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(filterOpChanged(int)));
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
  this->done(0);
}

void FilterDialog::cancelButton() {
  this->done(0);
}

void FilterDialog::filterOpChanged(int op) {
  if (op == 0) {
    // Between
    m_ui->minDoubleSpinBox->setVisible(true);
    m_ui->maxDoubleSpinBox->setVisible(true);
    m_ui->andLabel->setVisible(true);
  } else if (op == 1) {
    // Less
    m_ui->minDoubleSpinBox->setVisible(false);
    m_ui->maxDoubleSpinBox->setVisible(true);
    m_ui->andLabel->setVisible(false);
  } else if (op == 2) {
    // Greater
    m_ui->minDoubleSpinBox->setVisible(true);
    m_ui->maxDoubleSpinBox->setVisible(false);
    m_ui->andLabel->setVisible(false);
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
