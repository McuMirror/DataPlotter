//  Copyright (C) 2020-2024  Jiří Maier

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

#ifndef UPDATECHECKER_H
#define UPDATECHECKER_H

#include <QNetworkReply>
#include <QObject>

class QNetworkAccessManager;

class UpdateChecker : public QObject {
  Q_OBJECT

public:
  explicit UpdateChecker(QObject *parent = nullptr);

  void checkForUpdates(bool showOnlyPositiveResult);

private slots:
  void onRequestFinished(QNetworkReply *reply);

private:
  QNetworkAccessManager *m_networkManager;
  bool onlyPosRes = false;

signals:
  void checkedVersion(bool isNew, QString message);
};

#endif // UPDATECHECKER_H
