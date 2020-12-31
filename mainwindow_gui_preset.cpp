//  Copyright (C) 2020-2021  Jiří Maier

//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.

//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.

//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "mainwindow.h"

void MainWindow::connectSignals() {
  connect(ui->pushButtonPause, &QPushButton::clicked, ui->plot, &MyMainPlot::togglePause);
  connect(ui->pushButtonPause, &QPushButton::clicked, ui->plotxy, &MyXYPlot::togglePause);
  connect(ui->checkBoxVerticalValues, &QCheckBox::toggled, ui->plot, &MyPlot::setShowVerticalValues);
  connect(ui->plot, &MyMainPlot::showPlotStatus, this, &MainWindow::showPlotStatus);
  connect(ui->plot, &MyPlot::gridChanged, this, &MainWindow::updateDivs);
  connect(ui->doubleSpinBoxRangeHorizontal, SIGNAL(valueChanged(double)), ui->plot, SLOT(setRollingRange(double)));
  connect(ui->doubleSpinBoxRangeHorizontal, SIGNAL(valueChanged(double)), ui->plotxy, SLOT(setTimeRange(double)));
  connect(ui->doubleSpinBoxRangeVerticalRange, SIGNAL(valueChanged(double)), ui->plot, SLOT(setVerticalRange(double)));
  connect(ui->doubleSpinBoxRangeVerticalRange, SIGNAL(valueChanged(double)), ui->plotxy, SLOT(setRange(double)));
  connect(ui->doubleSpinBoxRangeHorizontal, SIGNAL(valueChanged(double)), ui->dialRollingRange, SLOT(updatePosition(double)));
  connect(ui->doubleSpinBoxRangeVerticalRange, SIGNAL(valueChanged(double)), ui->dialVerticalRange, SLOT(updatePosition(double)));
  connect(ui->doubleSpinBoxChScale, SIGNAL(valueChanged(double)), ui->dialChScale, SLOT(updatePosition(double)));
  connect(ui->dialVerticalCenter, &QDial::valueChanged, ui->plot, &MyMainPlot::setVerticalCenter);
  connect(ui->dialVerticalCenter, &QDial::valueChanged, ui->plotxy, &MyXYPlot::setCenter);
  connect(ui->horizontalScrollBarHorizontal, &QScrollBar::valueChanged, ui->plot, &MyMainPlot::setHorizontalPos);
  connect(ui->lineEditHtitle, &QLineEdit::textChanged, ui->plot, &MyPlot::setXTitle);
  connect(ui->lineEditVtitle, &QLineEdit::textChanged, ui->plot, &MyPlot::setYTitle);
  connect(ui->lineEditVtitle, &QLineEdit::textChanged, ui->plotxy, &MyPlot::setXTitle);
  connect(ui->lineEditVtitle, &QLineEdit::textChanged, ui->plotxy, &MyPlot::setYTitle);
  connect(ui->myTerminal, &MyTerminal::sendMessage, this, &MainWindow::printMessage);
  connect(ui->dialVerticalDiv, &QDial::valueChanged, ui->plot, &MyPlot::setGridHintY);
  connect(ui->dialhorizontalDiv, &QDial::valueChanged, ui->plot, &MyPlot::setGridHintX);
  connect(ui->spinBoxShiftStep, SIGNAL(valueChanged(int)), ui->plot, SLOT(setShiftStep(int)));
  connect(ui->plot, &MyMainPlot::requestCursorUpdate, this, &MainWindow::updateCursors);
  connect(ui->myTerminal, &MyTerminal::sendDebug, this, &MainWindow::printTerminalDebug);

  connect(&portsRefreshTimer, &QTimer::timeout, this, &MainWindow::comRefresh);
  connect(&activeChRefreshTimer, &QTimer::timeout, this, &MainWindow::updateUsedChannels);
  connect(&cursorRangeUpdateTimer, &QTimer::timeout, this, &MainWindow::updateCursorRange);
  connect(&measureRefreshTimer, &QTimer::timeout, this, &MainWindow::updateMeasurements);
  connect(&fftTimer, &QTimer::timeout, this, &::MainWindow::updateFFT);
}

void MainWindow::setAdaptiveSpinBoxes() {
// Adaptivní krok není v starším Qt (Win XP)
#if QT_VERSION >= 0x050C00
  ui->doubleSpinBoxChScale->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
  ui->doubleSpinBoxRangeHorizontal->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
  ui->doubleSpinBoxRangeVerticalRange->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
#endif
}

void MainWindow::startTimers() {
  portsRefreshTimer.start(500);
  activeChRefreshTimer.start(500);
  cursorRangeUpdateTimer.start(100);
  measureRefreshTimer.start(250);
  fftTimer.start(500);
}

void MainWindow::setGuiDefaults() {
  ui->tabs_right->setCurrentIndex(0);
  ui->tabsControll->setCurrentIndex(0);
  ui->comboBoxOutputLevel->setCurrentIndex((int)OutputLevel::info);
  ui->radioButtonFixedRange->setChecked(true);
  ui->plotxy->setHidden(true);
  ui->plotFFT->setHidden(true);
  ui->frameTermanalDebug->setVisible(ui->pushButtonTerminalDebug->isChecked());
  ui->labelBuildDate->setText(tr("Build: ") + QString(__DATE__) + " " + QString(__TIME__));
  ui->pushButtonPause->setIcon(iconRun);
  ui->pushButtonMultiplInputs->setChecked(false);
}

void MainWindow::setGuiArrays() {
  mathEn[0] = ui->pushButtonMath1;
  mathEn[1] = ui->pushButtonMath2;
  mathEn[2] = ui->pushButtonMath3;

  mathFirst[0] = ui->spinBoxMath1First;
  mathFirst[1] = ui->spinBoxMath2First;
  mathFirst[2] = ui->spinBoxMath3First;

  mathSecond[0] = ui->spinBoxMath1Second;
  mathSecond[1] = ui->spinBoxMath2Second;
  mathSecond[2] = ui->spinBoxMath3Second;

  mathOp[0] = ui->comboBoxMath1Op;
  mathOp[1] = ui->comboBoxMath2Op;
  mathOp[2] = ui->comboBoxMath3Op;
}

void MainWindow::fillChannelSelect() {
  ui->comboBoxSelectedChannel->blockSignals(true);
  ui->comboBoxCursor1Channel->blockSignals(true);
  ui->comboBoxCursor2Channel->blockSignals(true);
  ui->comboBoxMeasure1->blockSignals(true);
  ui->comboBoxMeasure2->blockSignals(true);

  for (int i = 0; i < ANALOG_COUNT + MATH_COUNT; i++) {
    ui->comboBoxSelectedChannel->addItem(GlobalFunctions::getChName(i));
    ui->comboBoxCursor1Channel->addItem(GlobalFunctions::getChName(i));
    ui->comboBoxCursor2Channel->addItem(GlobalFunctions::getChName(i));
    ui->comboBoxMeasure1->addItem(GlobalFunctions::getChName(i));
    ui->comboBoxMeasure2->addItem(GlobalFunctions::getChName(i));
  }
  for (int i = 1; i <= LOGIC_GROUPS; i++) {
    ui->comboBoxSelectedChannel->addItem(tr("Logic %1").arg(i));
    ui->comboBoxCursor1Channel->addItem(tr("Logic %1").arg(i));
    ui->comboBoxCursor2Channel->addItem(tr("Logic %1").arg(i));
  }
  ui->comboBoxCursor1Channel->addItem(iconXY, (tr("XY")));
  ui->comboBoxCursor2Channel->addItem(iconXY, tr("XY"));
  ui->comboBoxCursor1Channel->addItem(iconFFT, (tr("FFT")));
  ui->comboBoxCursor2Channel->addItem(iconFFT, tr("FFT"));

  ui->comboBoxMeasure1->addItem(iconCross, tr("Off"));
  ui->comboBoxMeasure2->addItem(iconCross, tr("Off"));
  ui->comboBoxMeasure1->setCurrentIndex(ui->comboBoxMeasure1->count() - 1);
  ui->comboBoxMeasure2->setCurrentIndex(ui->comboBoxMeasure2->count() - 1);

  // Skryje XY kanál z nabýdky.
  auto *model = qobject_cast<QStandardItemModel *>(ui->comboBoxCursor1Channel->model());
  auto *item = model->item(XYID);
  item->setEnabled(false);
  QListView *view = qobject_cast<QListView *>(ui->comboBoxCursor1Channel->view());
  view->setRowHidden(XYID, !false);

  model = qobject_cast<QStandardItemModel *>(ui->comboBoxCursor2Channel->model());
  item = model->item(XYID);
  item->setEnabled(false);
  view = qobject_cast<QListView *>(ui->comboBoxCursor2Channel->view());
  view->setRowHidden(XYID, !false);

  // Skryje FFT kanál z nabýdky.
  model = qobject_cast<QStandardItemModel *>(ui->comboBoxCursor1Channel->model());
  item = model->item(FFTID);
  item->setEnabled(false);
  view = qobject_cast<QListView *>(ui->comboBoxCursor1Channel->view());
  view->setRowHidden(FFTID, !false);

  model = qobject_cast<QStandardItemModel *>(ui->comboBoxCursor2Channel->model());
  item = model->item(FFTID);
  item->setEnabled(false);
  view = qobject_cast<QListView *>(ui->comboBoxCursor2Channel->view());
  view->setRowHidden(FFTID, !false);

  ui->comboBoxSelectedChannel->blockSignals(false);
  ui->comboBoxCursor1Channel->blockSignals(false);
  ui->comboBoxCursor2Channel->blockSignals(false);
  ui->comboBoxMeasure1->blockSignals(false);
  ui->comboBoxMeasure2->blockSignals(false);
}
