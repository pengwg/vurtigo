#include "rtWindowLevelDialog.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QGraphicsItem>

#include "rtConfigOptions.h"
#include "rtApplication.h"

rtWindowLevelDialog::rtWindowLevelDialog(QString fName) : c_histogramSize(50) {
  setupUi(this);
  m_defWindow = 1.0f;
  m_defLevel = 0.5f;

  // Check if a parameter was given
  if (fName == "") {
    m_fName = rtApplication::instance().getConfigOptions()->getWLFileName();
  } else {
    m_fName = fName;
  }

  m_histogram = new int[c_histogramSize];

  m_currWindow = 1.0f;
  m_currLevel = 0.5f;

  m_levelList.clear();
  m_currSelection = 0;

  m_range[0] = 0;
  m_range[1] = 0;

  // Setup the default
  WLObject def;

  def.name = "Default";
  def.window = m_defWindow;
  def.level = m_defLevel;
  def.old_window = m_defWindow;
  def.old_level = m_defLevel;
  def.modified = false;

  m_levelList.append(def);

  loadFile();
  updateTable();

  m_scene = new QGraphicsScene(this);
  m_scene->setItemIndexMethod(QGraphicsScene::NoIndex);

  this->wlFileList->selectRow(m_currSelection);
  this->graphicsView->setScene(m_scene);

  for (int ix1=0; ix1<c_histogramSize; ix1++) {
    m_scene->addItem(new QGraphicsRectItem(ix1*4, 0, 2, 1));
  }
  m_linePen.setColor(Qt::blue);
  QGraphicsLineItem *line = new QGraphicsLineItem(0, 0, 0, -200);
  line->setPen(m_linePen);
  m_scene->addItem(line);
  QGraphicsLineItem *line2 = new QGraphicsLineItem(4*c_histogramSize, 0, 4*c_histogramSize, -200);
  line2->setPen(m_linePen);
  m_scene->addItem(line2);

  connect( this->newButton, SIGNAL(pressed()), this, SLOT(newLine()) );
  connect( this->saveLineButton, SIGNAL(pressed()), this, SLOT(saveLine()) );
  connect( this->saveAllButton, SIGNAL(pressed()), this, SLOT(saveAll()) );
  connect( this->windowSlider, SIGNAL(sliderMoved(int)), this, SLOT(windowSliderChange(int)));
  connect( this->levelSlider, SIGNAL(sliderMoved(int)), this, SLOT(levelSliderChange(int)));
  connect( this->wlFileList, SIGNAL(currentCellChanged(int,int,int,int)), this, SLOT(selectNewRow(int,int,int,int)));
  connect( this->wlFileList, SIGNAL(cellChanged(int,int)), this, SLOT(changeCell(int,int)));
}


rtWindowLevelDialog::~rtWindowLevelDialog() {
  delete m_scene;
  delete[] m_histogram;
  disconnect( this->newButton, SIGNAL(pressed()), this, SLOT(newLine()) );
  disconnect( this->saveLineButton, SIGNAL(pressed()), this, SLOT(saveLine()) );
  disconnect( this->saveAllButton, SIGNAL(pressed()), this, SLOT(saveAll()) );
  disconnect( this->windowSlider, SIGNAL(sliderMoved(int)), this, SLOT(windowSliderChange(int)));
  disconnect( this->levelSlider, SIGNAL(sliderMoved(int)), this, SLOT(levelSliderChange(int)));
  disconnect( this->wlFileList, SIGNAL(currentCellChanged(int,int,int,int)), this, SLOT(selectNewRow(int,int,int,int)));
  disconnect( this->wlFileList, SIGNAL(cellChanged(int,int)), this, SLOT(changeCell(int,int)));
}


void rtWindowLevelDialog::setImageData(vtkImageData* dat) {
  if(!dat) return;

  double rangeI[2];

  dat->GetScalarRange(rangeI);

  // Re-work the histogram.
  for (int ix1=0; ix1<c_histogramSize; ix1++) {
    m_histogram[ix1] = 0;
  }
  int dims[3];
  unsigned char* ucharP;
  unsigned short* ushortP;
  float loc;
  unsigned short spot;
  dat->GetDimensions(dims);

  if (dat->GetScalarType() == VTK_UNSIGNED_CHAR) {
    ucharP = static_cast<unsigned char*>(dat->GetScalarPointer());
    for (int ix1=0; ix1<dims[0]*dims[1]*dims[2]; ix1++) {
      loc=((float)ucharP[ix1])/((float)(rangeI[1]-rangeI[0]))*c_histogramSize;
      spot = floor(loc);
      if (spot < c_histogramSize) {
        m_histogram[spot] = m_histogram[spot]+1;
      } else {
        // Fail silently. Data may have a couple 'bad points'.
        //std::cout << "Spot for histogram is off: " << spot << " and loc: " << loc << std::endl;
      }
    }
  } else if (dat->GetScalarType() == VTK_UNSIGNED_SHORT) {
    ushortP = static_cast<unsigned short*>(dat->GetScalarPointer());
    for (int ix1=0; ix1<dims[0]*dims[1]*dims[2]; ix1++) {
      loc=((float)ushortP[ix1])/((float)(rangeI[1]-rangeI[0]))*c_histogramSize;
      spot = floor(loc);
      if (spot < c_histogramSize) {
        m_histogram[spot] = m_histogram[spot]+1;
      } else {
        // Fail silently. Data may have a couple 'bad points'.
        //std::cout << "Spot for histogram is off: " << spot << " and loc: " << loc << std::endl;
        //std::cout << "ushort: " << ushortP[ix1] << " range " << rangeI[1]-rangeI[0] << std::endl;
      }
    }
  }

  QList<QGraphicsItem *> test = m_scene->items();
  int barLocation;
  barLocation = 0;
  float height;

  for (int ix1=0; ix1<test.size(); ix1++) {
    QGraphicsRectItem* temp = qgraphicsitem_cast<QGraphicsRectItem*>(test[ix1]);

    if (temp) {
      height = ((double)m_histogram[barLocation])/((double)dims[0]*dims[1]*dims[2])*-500.0f;

      // Very strange... but this update call is requied otherwise the program crashes on the setRect...
      m_scene->update();
      temp->setRect(barLocation*4, 0, 2, height);
      barLocation++;
    }
  }

  // Multiply the range by 1000 to minimize rounding errors.
  rangeI[0] = rangeI[0]*1000.0f;
  rangeI[1] = rangeI[1]*1000.0f;

   // Check if the range needs to be reset.
  if ( (rangeI[1]-rangeI[0]) > m_range[1]) {

    m_range[0] = 0;
    m_range[1] = rangeI[1]-rangeI[0];

    this->windowSlider->setMinimum(m_range[0]);
    this->windowSlider->setMaximum(m_range[1]);

    this->levelSlider->setMinimum(m_range[0]);
    this->levelSlider->setMaximum(m_range[1]);

    this->windowMinLabel->setText(QString::number(m_range[0]));
    this->windowMaxLabel->setText(QString::number(m_range[1]/1000.0f));
    this->levelMinLabel->setText(QString::number(m_range[0]));
    this->levelMaxLabel->setText(QString::number(m_range[1]/1000.0f));

    this->windowSlider->setValue(m_currWindow*m_range[1]);
    this->levelSlider->setValue(m_currLevel*m_range[1]);
    emit valuesChanged(this->windowSlider->value()/1000.0f, this->levelSlider->value()/1000.0f);

    updateRangeBars(m_currWindow, m_currLevel);

  }
}

void rtWindowLevelDialog::loadFile() {
  QFile file(m_fName);

  if (file.exists()) {
    if (file.open(QIODevice::ReadOnly)) {
      QXmlStreamReader reader(&file);
      QXmlStreamAttributes attribs;

      // Read the XML into the list.
      while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement()) {
          if (reader.name() == "WL") {
            attribs = reader.attributes();
#if QT_VERSION >= 0x040500
            if ( attribs.hasAttribute("name") && attribs.hasAttribute("window") && attribs.hasAttribute("level") ) {
#endif
              // The tags are ok so we can read the window level.
              WLObject def;
              def.name = attribs.value("name").toString();
              def.window = attribs.value("window").toString().toDouble();
              def.level = attribs.value("level").toString().toDouble();
              def.old_window = def.window;
              def.old_level = def.level;
              def.modified = false;

              m_levelList.append(def);
#if QT_VERSION >= 0x040500
            }
#endif
          }
        }
      }
      if (reader.hasError()) {

      }

      file.close();
      updateTable();
    }
  }
}

void rtWindowLevelDialog::newLine() {
  WLObject def;

  def.name = "New WL";
  def.window = m_defWindow;
  def.level = m_defLevel;
  def.old_window = m_defWindow;
  def.old_level = m_defLevel;
  def.modified = true;

  m_levelList.append(def);
  updateTable();
}

void rtWindowLevelDialog::saveLine() {
  QFile file(m_fName);

  if (file.exists()) {
    if (file.open(QIODevice::WriteOnly|QIODevice::Truncate)) {
      QXmlStreamWriter stream(&file);
      stream.setAutoFormatting(true);
      stream.writeStartDocument();

      stream.writeStartElement("WindowLevelConfig");

      // Skip the first entry it is the default.
      for (int ix1=1; ix1<m_levelList.size(); ix1++) {
        if (ix1 == m_currSelection) {
          stream.writeStartElement("WL");
          stream.writeAttribute("name", m_levelList[ix1].name);
          stream.writeAttribute("window", QString::number(m_levelList[ix1].window));
          stream.writeAttribute("level", QString::number(m_levelList[ix1].level));
          stream.writeEndElement();

          m_levelList[ix1].old_window = m_levelList[ix1].window;
          m_levelList[ix1].old_level = m_levelList[ix1].level;
          m_levelList[ix1].modified = false;
        } else {
          stream.writeStartElement("WL");
          stream.writeAttribute("name", m_levelList[ix1].name);
          stream.writeAttribute("window", QString::number(m_levelList[ix1].old_window));
          stream.writeAttribute("level", QString::number(m_levelList[ix1].old_level));
          stream.writeEndElement();
        }
      }
      stream.writeEndElement(); // bookmark

      stream.writeEndDocument();
      file.close();
    }
  }
  updateTable();
}

void rtWindowLevelDialog::saveAll() {
  QFile file(m_fName);
  if (file.exists()) {
    if (file.open(QIODevice::WriteOnly|QIODevice::Truncate)) {
      QXmlStreamWriter stream(&file);
      stream.setAutoFormatting(true);
      stream.writeStartDocument();

      stream.writeStartElement("WindowLevelConfig");

      // Skip the first entry it is the default.
      for (int ix1=1; ix1<m_levelList.size(); ix1++) {
        stream.writeStartElement("WL");
        stream.writeAttribute("name", m_levelList[ix1].name);
        stream.writeAttribute("window", QString::number(m_levelList[ix1].window));
        stream.writeAttribute("level", QString::number(m_levelList[ix1].level));
        stream.writeEndElement();

        m_levelList[ix1].old_window = m_levelList[ix1].window;
        m_levelList[ix1].old_level = m_levelList[ix1].level;
        m_levelList[ix1].modified = false;
      }
      stream.writeEndElement(); // bookmark

      stream.writeEndDocument();
      file.close();
    }
  }
  updateTable();
}

void rtWindowLevelDialog::windowSliderChange(int winVal) {
  m_currWindow = ((double)winVal)/m_range[1];
  emit valuesChanged(this->windowSlider->value()/1000.0f, this->levelSlider->value()/1000.0f);
  updateRangeBars(m_currWindow, m_currLevel);

  // Not yet been modified.
  if (!m_levelList[m_currSelection].modified) {
    m_levelList[m_currSelection].old_window = m_levelList[m_currSelection].window;
    m_levelList[m_currSelection].old_level = m_levelList[m_currSelection].level;
    m_levelList[m_currSelection].modified = true;

    QTableWidgetItem* item4 = new QTableWidgetItem();
    item4->setText(QString::number(m_levelList[m_currSelection].modified));
    item4->setFlags(Qt::ItemIsSelectable);
    this->wlFileList->setItem(m_currSelection, 3, item4);
  }
  m_levelList[m_currSelection].window = m_currWindow;

  QTableWidgetItem* item2 = new QTableWidgetItem();
  item2->setText(QString::number(m_levelList[m_currSelection].window));
  item2->setFlags(Qt::ItemIsSelectable);
  this->wlFileList->setItem(m_currSelection, 1, item2);
}

void rtWindowLevelDialog::levelSliderChange(int levelVal) {
  m_currLevel = ((double)levelVal)/m_range[1];
  emit valuesChanged(this->windowSlider->value()/1000.0f, this->levelSlider->value()/1000.0f);
  updateRangeBars(m_currWindow, m_currLevel);

    // Not yet been modified.
  if (!m_levelList[m_currSelection].modified) {
    m_levelList[m_currSelection].old_window = m_levelList[m_currSelection].window;
    m_levelList[m_currSelection].old_level = m_levelList[m_currSelection].level;
    m_levelList[m_currSelection].modified = true;

    QTableWidgetItem* item4 = new QTableWidgetItem();
    item4->setText(QString::number(m_levelList[m_currSelection].modified));
    item4->setFlags(Qt::ItemIsSelectable);
    this->wlFileList->setItem(m_currSelection, 3, item4);
  }
  m_levelList[m_currSelection].level = m_currLevel;

  QTableWidgetItem* item3 = new QTableWidgetItem();
  item3->setText(QString::number(m_levelList[m_currSelection].level));
  item3->setFlags(Qt::ItemIsSelectable);
  this->wlFileList->setItem(m_currSelection, 2, item3);
}

void rtWindowLevelDialog::selectNewRow(int row, int col, int old_row, int old_col) {
  // Check that the row exists.
  if (row >= m_levelList.size()) return;

  // No update needed.
  if (m_currSelection == row) return;

  m_currSelection = row;

  m_currWindow = m_levelList[m_currSelection].window;
  m_currLevel = m_levelList[m_currSelection].level;
  this->windowSlider->setValue(m_currWindow*m_range[1]);
  this->levelSlider->setValue(m_currLevel*m_range[1]);
  emit valuesChanged(this->windowSlider->value()/1000.0f, this->levelSlider->value()/1000.0f);
  updateRangeBars(m_currWindow, m_currLevel);
}

void rtWindowLevelDialog::changeCell(int row, int col) {
  if (row >= m_levelList.size() || col != 0) return;

  // An update is not needed
  if (m_levelList[row].name == this->wlFileList->item(row, col)->text()) return;

  m_levelList[row].name = this->wlFileList->item(row, col)->text();
  m_levelList[row].modified = true;

  this->wlFileList->item(row, 3)->setText(QString::number(true));
}

void rtWindowLevelDialog::updateRangeBars(double w, double l) {
  //return;
  QList<QGraphicsItem *> test = m_scene->items();
  int barLocation[2];
  barLocation[0] = (l-w/2.0f)*4.0f*c_histogramSize;
  barLocation[1] = (l+w/2.0f)*4.0f*c_histogramSize;
  int loc = 0;
  for (int ix1=0; ix1<test.size(); ix1++) {
    QGraphicsLineItem* temp = qgraphicsitem_cast<QGraphicsLineItem*>(test[ix1]);
    if (temp && loc < 2) {
      temp->setLine(barLocation[loc], 0, barLocation[loc], -200.0f);
      loc++;
    }
  }
}

void rtWindowLevelDialog::updateTable() {
  if (this->wlFileList->rowCount() != m_levelList.size()) {
    this->wlFileList->setRowCount(m_levelList.size());
  }

  for (int ix1=0; ix1<m_levelList.size(); ix1++) {
    QTableWidgetItem* item1 = new QTableWidgetItem();
    item1->setText(m_levelList[ix1].name);
    if (ix1 == 0) item1->setFlags(Qt::ItemIsSelectable);
    this->wlFileList->setItem(ix1, 0, item1);

    QTableWidgetItem* item2 = new QTableWidgetItem();
    item2->setText(QString::number(m_levelList[ix1].window));
    item2->setFlags(Qt::ItemIsSelectable);
    this->wlFileList->setItem(ix1, 1, item2);

    QTableWidgetItem* item3 = new QTableWidgetItem();
    item3->setText(QString::number(m_levelList[ix1].level));
    item3->setFlags(Qt::ItemIsSelectable);
    this->wlFileList->setItem(ix1, 2, item3);

    QTableWidgetItem* item4 = new QTableWidgetItem();
    item4->setText(QString::number(m_levelList[ix1].modified));
    item4->setFlags(Qt::ItemIsSelectable);
    this->wlFileList->setItem(ix1, 3, item4);
  }

}
