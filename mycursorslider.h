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

#ifndef MYCURSORSLIDER_H
#define MYCURSORSLIDER_H

#include <QObject>
#include <QSlider>

#include "enums_defines_constants.h"

class myCursorSlider : public QSlider {
  Q_OBJECT
public:
  explicit myCursorSlider(QWidget *parent = nullptr);
  int realValue;
  void updateRange(int min, int max);

private slots:
  void positionChanged(int newpos);
  void setRealValue(int newValue);

signals:
  void realValueChanged(int value);
};

#endif // MYCURSORSLIDER_H