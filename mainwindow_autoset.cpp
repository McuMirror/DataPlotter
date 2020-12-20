//  Copyright (C) 2020  Jiří Maier

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

void MainWindow::on_pushButtonAutoset_clicked() {
  double recentOffset = 0;
  for (int i = LOGIC_GROUPS - 1; i >= 0; i--) {
    int bitsUsed = ui->plot->getLogicBitsUsed(i);
    if (bitsUsed > 0 && ui->plot->isLogicVisible(i)) {
      // Skupina logických kanálů má spodek na 0 a vršek je 3*počet kanálů (bitů)
      ui->plot->setLogicOffset(i, recentOffset);
      recentOffset += bitsUsed * 3 * ui->plot->getLogicScale(i);
    }
  }
  for (int i = ANALOG_COUNT + MATH_COUNT - 1; i >= 0; i--) {
    if (ui->plot->isChUsed(i) && ui->plot->isChVisible(i)) {
      bool foundRange; // Zbytečné, ale ta funkce to potřebuje.
      QCPRange range = ui->plot->graph(i)->data()->valueRange(foundRange);
      range.lower = GlobalFunctions::ceilToNiceValue(range.lower) * ui->plot->getChScale(i);
      range.upper = GlobalFunctions::ceilToNiceValue(range.upper) * ui->plot->getChScale(i);
      recentOffset -= range.lower;
      ui->plot->setChOffset(i, recentOffset);
      recentOffset += range.upper;
    }
  }
  if (recentOffset != 0) {
    on_pushButtonPositive_clicked();
    ui->checkBoxVerticalValues->setChecked(false);
    ui->doubleSpinBoxRangeVerticalRange->setValue(recentOffset);
  }
}