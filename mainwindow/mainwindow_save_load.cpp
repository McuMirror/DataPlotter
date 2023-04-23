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

void MainWindow::initSetables() {
    // Range
    setables["vrange"] = {ui->doubleSpinBoxRangeVerticalRange,false};
    setables["hrange"] = {ui->doubleSpinBoxRangeHorizontal,false};
    setables["vcenter"] = {ui->doubleSpinBoxViewOffset,false};

    // Plot settings
    setables["vaxis"] = {ui->checkBoxVerticalValues,false};
    setables["haxis"] = {ui->comboBoxHAxisType,false};
    setables["hlabel"] = {ui->lineEditHtitle,false};
    setables["hunit"] = {ui->lineEditHUnit,false};
    setables["vlabel"] = {ui->lineEditVtitle,false};
    setables["vunit"] = {ui->lineEditVUnit,false};

    setables["opengl"] = {ui->checkBoxOpenGL,true};
    setables["filter"] = {ui->comboBoxFIR,true};

    // Settings
    setables["clearonrec"] = {ui->checkBoxClearOnReconnect,true};
    setables["rstcmd"] = {ui->lineEditResetCmd,true};
    setables["autoautoset"] = {ui->checkBoxAutoAutoSet,true};
    setables["nofreeze"] = {ui->checkBoxFreezeSafe,true};
}

void MainWindow::applyGuiElementSettings(QWidget* target, QString value) {
    if (QDoubleSpinBox* newTarget = dynamic_cast<QDoubleSpinBox*>(target))
        newTarget->setValue(value.toDouble());
    else if (QComboBox* newTarget = dynamic_cast<QComboBox*>(target))
        newTarget->setCurrentIndex(value.toUInt());
    else if (QSpinBox* newTarget = dynamic_cast<QSpinBox*>(target))
        newTarget->setValue(value.toInt());
    else if (QScrollBar* newTarget = dynamic_cast<QScrollBar*>(target))
        newTarget->setValue(value.toInt());
    else if (QSlider* newTarget = dynamic_cast<QSlider*>(target))
        newTarget->setValue(value.toInt());
    else if (QCheckBox* newTarget = dynamic_cast<QCheckBox*>(target))
        newTarget->setChecked((bool)value.toUInt());
    else if (QPushButton* newTarget = dynamic_cast<QPushButton*>(target))
        newTarget->setChecked((bool)value.toUInt());
    else if (QDial* newTarget = dynamic_cast<QDial*>(target))
        newTarget->setValue(value.toInt());
    else if (QLineEdit* newTarget = dynamic_cast<QLineEdit*>(target))
        newTarget->setText(value);
}

QByteArray MainWindow::readGuiElementSettings(QWidget* target) {
    if (QDoubleSpinBox* newTarget = dynamic_cast<QDoubleSpinBox*>(target))
        return (QString::number(newTarget->value()).toUtf8());
    if (QSpinBox* newTarget = dynamic_cast<QSpinBox*>(target))
        return (QString::number(newTarget->value()).toUtf8());
    if (QScrollBar* newTarget = dynamic_cast<QScrollBar*>(target))
        return (QString::number(newTarget->value()).toUtf8());
    if (QSlider* newTarget = dynamic_cast<QSlider*>(target))
        return (QString::number(newTarget->value()).toUtf8());
    if (QCheckBox* newTarget = dynamic_cast<QCheckBox*>(target))
        return (newTarget->isChecked() ? "1" : "0");
    if (QPushButton* newTarget = dynamic_cast<QPushButton*>(target))
        return (newTarget->isChecked() ? "1" : "0");
    if (QComboBox* newTarget = dynamic_cast<QComboBox*>(target))
        return (QString::number(newTarget->currentIndex()).toUtf8());
    if (QDial* newTarget = dynamic_cast<QDial*>(target))
        return (QString::number(newTarget->value()).toUtf8());
    else if (QLineEdit* newTarget = dynamic_cast<QLineEdit*>(target))
        return (newTarget->text().toUtf8());
    return "";
}

QByteArray MainWindow::getSettings() {
    QByteArray settings;

    for (auto it = setables.begin(); it != setables.end(); it++)
        if(it.value().second == true)
            settings.append(QString(it.key() + ':' + readGuiElementSettings(it.value().first) + ";\n").toUtf8());

    settings.append(QString("baud:%1;\n").arg(ui->comboBoxBaud->currentText().toUInt()).toLocal8Bit());

    if (ui->checkBoxTriggerLineEn->checkState() == Qt::Checked)
        settings.append("trigline:on;\n");
    else if (ui->checkBoxTriggerLineEn->checkState() == Qt::Unchecked)
        settings.append("trigline:off;\n");
    else if (ui->checkBoxTriggerLineEn->checkState() == Qt::PartiallyChecked)
        settings.append("trigline:auto;\n");

    if (!recommendOpenGL)
        settings.append("noopengldialog;\n");

    settings.append(ui->radioButtonEn->isChecked() ? "lang:en" : "lang:cz");
    settings.append(";\n");
    settings.append(ui->radioButtonCSVDot->isChecked() ? "csvsep:dc" : "csvsep:cs");
    settings.append(";\n");
    settings.append(ui->radioButtonLight->isChecked() ? "theme:light" : "theme:dark");
    settings.append(";\n");

    QColor xyclr = ui->plotxy->graphXY->pen().color();
    settings.append(QString("xyclr:%1,%2,%3").arg(xyclr.red()).arg(xyclr.green()).arg(xyclr.blue()).toUtf8());
    settings.append(";\n");

    for (int i = 0; i < ANALOG_COUNT + MATH_COUNT; i++) {
        settings.append("ch:" + QString::number(i + 1).toUtf8());
        QColor clr = ui->plot->getChColor(i);
        settings.append(QString(":clr:%1,%2,%3").arg(clr.red()).arg(clr.green()).arg(clr.blue()).toUtf8());
        settings.append(";\n");
    }

    for (int i = 0; i < LOGIC_GROUPS; i++) {
        settings.append("log:" + QString::number(i + 1).toUtf8());
        QColor clr = ui->plot->getLogicColor(i);
        settings.append(QString(":clr:%1,%2,%3").arg(clr.red()).arg(clr.green()).arg(clr.blue()).toUtf8());
        settings.append(";\n");
    }

    return settings;
}

void MainWindow::useSettings(QByteArray settings, MessageTarget::enumMessageTarget source = MessageTarget::manual) {
    settings.replace('\n', "");
    settings.replace('\r', "");

    if (settings == "clearlog") {
        ui->plot->clearLogicGroup(2, 0);
    }

    else if (settings == "noopengldialog") {
        recommendOpenGL = false;
    }

    else {
        if (!settings.contains(':')) {
            if (source == MessageTarget::manual || ui->comboBoxOutputLevel->currentIndex() >= MessageLevel::error)
                printMessage(tr("Invalid settings").toUtf8(), settings, MessageLevel::error, source);
            return;
        }
        QByteArray type = settings.left(settings.indexOf(':', 0));
        QByteArray value = settings.mid(settings.indexOf(':', 0) + 1);
        type = type.simplified().toLower();

        if (setables.contains(type))
            applyGuiElementSettings(setables[type].first, value);

        else if (type == "baud") {
            ui->comboBoxBaud->setEditText(QString::number(value.toUInt()));
        }

        else if (type == "vpos") {
            if(value.toInt()>0)
                ui->pushButtonPositive->setChecked(true);
            else if(value.toInt()<0)
                ui->pushButtonNegative->setChecked(true);
            else
                ui->pushButtonCenter->setChecked(true);
            printMessage(tr("\"vpos\" has different meaning than in previous versions").toUtf8(), settings, MessageLevel::warning, source);
        }

        else if (type == "trigline") {
            if (value == "on")
                ui->checkBoxTriggerLineEn->setCheckState(Qt::Checked);
            if (value == "off")
                ui->checkBoxTriggerLineEn->setCheckState(Qt::Unchecked);
            if (value == "auto")
                ui->checkBoxTriggerLineEn->setCheckState(Qt::PartiallyChecked);
        }

        else if (type == "trigch") {
            int chid = value.toUInt() - 1;
            if (chid < 0)
                chid = 0;
            if (chid >= ANALOG_COUNT)
                chid = ANALOG_COUNT - 1;
            ui->plot->setTriggerLineChannel(chid);
            if (ui->checkBoxTriggerLineEn->checkState() == Qt::PartiallyChecked) {
                ui->plot->setTriggerLineVisible(true);
                triggerLineTimer.start();
            }
        }

        else if (type == "trigpos") {
            ui->plot->setTriggerLineValue(value.toDouble());
            if (ui->checkBoxTriggerLineEn->checkState() == Qt::PartiallyChecked) {
                ui->plot->setTriggerLineVisible(true);
                triggerLineTimer.start();
            }
        }

        else if (type == "lang") {
            if (value == "en")
                ui->radioButtonEn->setChecked(true);
            if (value == "cz")
                ui->radioButtonCz->setChecked(true);
        }

        else if (type == "theme") {
            if (value == "light")
                ui->radioButtonLight->setChecked(true);
            if (value == "dark")
                ui->radioButtonDark->setChecked(true);
        }

        else if (type == "csvsep") {
            if (value == "cs")
                ui->radioButtonCSVComma->setChecked(true);
            if (value == "dc")
                ui->radioButtonCSVDot->setChecked(true);
        }

        else if (type == "plotrange") {
            if (value == "fix")
                ui->radioButtonFixedRange->setChecked(true);
            if (value == "free")
                ui->radioButtonFreeRange->setChecked(true);
            if (value == "roll")
                ui->radioButtonRollingRange->setChecked(true);
        }

        else if (type == "layout") {
            if (value == "all")
                ui->radioButtonLayoutAll->setChecked(true);
            if (value == "time")
                ui->radioButtonLayoutTime->setChecked(true);
            if (value == "fft")
                ui->radioButtonLayoutFFT->setChecked(true);
            if (value == "xy")
                ui->radioButtonLayoutXY->setChecked(true);
        }

        else if (type == "noclickclr" || type == "clickclr") {
            QByteArrayList list = value.replace('.', ';').split(',');
            ui->listWidgetTerminalBlacklist->clear();
            foreach (QByteArray item, list) {
                if (!addColorToBlacklist(item))
                    printMessage(tr("Invalid color-code").toUtf8(), item, MessageLevel::error, source);
            }

            ui->comboBoxTerminalColorListMode->setCurrentIndex(type == "noclickclr" ? 0 : 1);
            updateColorBlacklist();
        }

        else if (type == "clearch") {
            ui->plot->clearCh(value.toUInt() - 1);
        }

        else if (type == "xyclr") {
            QByteArrayList rgb = value.split(',');
            if (rgb.length() != 3) {
                if (source == MessageTarget::manual || ui->comboBoxOutputLevel->currentIndex() >= MessageLevel::error)
                    printMessage(tr("Invalid color: ").toUtf8(), settings, MessageLevel::error, source);
                return;
            }
            QColor clr = QColor(rgb.at(0).toInt(), rgb.at(1).toInt(), rgb.at(2).toInt());
            ui->plotxy->graphXY->setPen(clr);
        }

        else if (type == "ch") {
            int ch = value.left(value.indexOf(':', 0)).toUInt();
            if (ch > ANALOG_COUNT + MATH_COUNT || ch == 0) {
                if (source == MessageTarget::manual || ui->comboBoxOutputLevel->currentIndex() >= MessageLevel::error)
                    printMessage(tr("Invalid channel in settings").toUtf8(), QString::number(ch).toUtf8(), MessageLevel::error, source);
                return;
            }
            // V nastavení se čísluje od jedné, ale v příkazech od nuly
            ch = ch - 1;
            QByteArray subtype = value.mid(value.indexOf(':', 0) + 1).toLower();
            subtype = subtype.left(subtype.indexOf(':'));
            QByteArray subvalue = value.mid(value.lastIndexOf(':') + 1);

            if (subtype == "sty")
                ui->plot->setChStyle(ch, subvalue.toUInt());
            else if (subtype == "clr") {
                QByteArrayList rgb = subvalue.mid(subvalue.indexOf(':')).split(',');
                if (rgb.length() != 3) {
                    if (source == MessageTarget::manual || ui->comboBoxOutputLevel->currentIndex() >= MessageLevel::error)
                        printMessage(tr("Invalid color: ").toUtf8(), settings, MessageLevel::error, source);
                    return;
                }
                QColor clr = QColor(rgb.at(0).toInt(), rgb.at(1).toInt(), rgb.at(2).toInt());
                ui->plot->setChColor(ch, clr);
                colorUpdateNeeded = true;
            }
            if (ui->comboBoxSelectedChannel->currentIndex() == ch)
                on_comboBoxSelectedChannel_currentIndexChanged(ui->comboBoxSelectedChannel->currentIndex());
        }

        else if (type == "log") {
            int group = value.left(value.indexOf(':', 0)).toUInt() - 1;
            if (group >= LOGIC_GROUPS) {
                if (source == MessageTarget::manual || ui->comboBoxOutputLevel->currentIndex() >= MessageLevel::error)
                    printMessage(tr("Invalid logic in settings").toUtf8(), QString::number(group).toUtf8(), MessageLevel::error, source);
                return;
            }
            QByteArray subtype = value.mid(value.indexOf(':', 0) + 1).toLower();
            subtype = subtype.left(subtype.indexOf(':'));
            QByteArray subvalue = value.mid(value.lastIndexOf(':') + 1);

            if (subtype == "sty")
                ui->plot->setLogicStyle(group, subvalue.toUInt());
            else if (subtype == "clr") {
                QByteArrayList rgb = subvalue.mid(subvalue.indexOf(':')).split(',');
                if (rgb.length() != 3) {
                    if (source == MessageTarget::manual || ui->comboBoxOutputLevel->currentIndex() >= MessageLevel::error)
                        printMessage(tr("Invalid color: ").toUtf8(), settings, MessageLevel::error, source);
                    return;
                }
                QColor clr = QColor(rgb.at(0).toInt(), rgb.at(1).toInt(), rgb.at(2).toInt());
                ui->plot->setLogicColor(group, clr);
                colorUpdateNeeded = true;
            }
            if (ui->comboBoxSelectedChannel->currentIndex() == group + ANALOG_COUNT + MATH_COUNT)
                on_comboBoxSelectedChannel_currentIndexChanged(ui->comboBoxSelectedChannel->currentIndex());
        }

        // Error
        else {
            if (source == MessageTarget::manual || ui->comboBoxOutputLevel->currentIndex() >= MessageLevel::error)
                printMessage(tr("Unknown setting").toUtf8(), type, MessageLevel::error, source);
            return;
        }
    }
    if (source == MessageTarget::manual || ui->comboBoxOutputLevel->currentIndex() == MessageLevel::info)
        printMessage(tr("Applied settings").toUtf8(), settings, MessageLevel::info, source);
}

void MainWindow::on_pushButtonLoadFile_clicked() {
    QString defaultName = QString(QCoreApplication::applicationDirPath()) + QString("/settings/");
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load file"), defaultName, tr("Settings file (*.cfg);;Comma separated values (*.csv);;Any file (*.*)"));
    if (fileName.isEmpty())
        return;
    QFile file(fileName);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QByteArray text = file.readAll();
        file.close();
        if (fileName.right(4) == ".cfg") {
            if (text.contains(';'))
                text.replace("\n", "");
            else
                text.replace("\n", ";");
            text.push_front("$$S");
            text.push_back("$$U");
        }
        if (fileName.right(4) == ".csv") {
            QByteArray firstline = text.left(text.indexOf('\n'));
            foreach (char ch, firstline) {
                // Zjistí jestli je první řádek záhlaví (obsahuje jiný text než čísla) a odebere ho.
                if (!isdigit(ch) && ch != '.' && ch != ',' && ch != ';' && ch != '-' && ch != '+' && ch != 'e' && ch != 'E') {
                    text = text.mid(text.indexOf('\n') + 1); break;
                }
            }
            if (text.contains(';')) {
                // Variantu 1,2;1,3; předělá na 1.2,1.3,
                text.replace(',', '.');
                text.replace(';', ',');
            }
            text.replace('\n', ';');
            text.push_front("$$P");
            text.push_back("$$U");
        }
        emit sendManualInput(text);
    } else {
        QMessageBox msgBox;
        msgBox.setText(tr("Cant open file."));
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
    }
}

void MainWindow::on_pushButtonDefaults_clicked() {
    QString defaultName = QString(QCoreApplication::applicationDirPath()) + QString("/settings/default.cfg");
    QFile file(defaultName);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        emit sendManualInput("$$S" + file.readAll().replace("\n", "") + "$$U");
        file.close();
    } else {
        QMessageBox msgBox(this);
        msgBox.setText(tr("Cant open file."));
        msgBox.setInformativeText(defaultName);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
    }
}

void MainWindow::on_pushButtonSaveSettings_clicked() {
    QString defaultName = QString(QCoreApplication::applicationDirPath()) + QString("/settings/default.cfg");
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save settings"), defaultName, tr("Settings file (*.cfg)"));
    if (fileName.isEmpty())
        return;
    QFile file(fileName);
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        file.write(getSettings());
        file.close();
    } else {
        QMessageBox msgBox(this);
        msgBox.setText(tr("Cant write to file."));
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
    }
}

void MainWindow::on_pushButtonReset_clicked() {
    QFile defaults(":/text/settings/default.cfg");
    if (defaults.open(QFile::ReadOnly | QFile::Text)) {
        emit sendManualInput("$$S" + defaults.readAll().replace("\n", "") + "$$U");
        defaults.close();
    }
    QString defaultName = QString(QCoreApplication::applicationDirPath()) + QString("/settings/default.cfg");
    QFile file(defaultName);
    if (!QDir(QCoreApplication::applicationDirPath() + "/settings").exists())
        QDir().mkdir((QCoreApplication::applicationDirPath() + "/settings"));
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        QFile defaults(":/text/settings/default.cfg");
        if (defaults.open(QFile::ReadOnly | QFile::Text)) {
            QString defaultName = QString(QCoreApplication::applicationDirPath()) + QString("/settings/default.cfg");
            QFile file(defaultName);
            if (file.open(QFile::WriteOnly | QFile::Truncate)) {
                file.write(defaults.readAll());
                file.close();
            }
            defaults.close();
        }
    } else {
        QMessageBox msgBox(this);
        msgBox.setText(tr("Can not create default settings file (if installed in ProgramFiles, admin rights are required)"));
        msgBox.setDetailedText(defaultName);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
    }
}

void MainWindow::setUp() {
    QString userDefaults = QString(QCoreApplication::applicationDirPath()) + QString("/settings/default.cfg");
    QFile userDefaultsFile(userDefaults);
    if (userDefaultsFile.open(QFile::ReadOnly | QFile::Text)) {
        emit sendManualInput("$$S" + userDefaultsFile.readAll().replace("\n", "") + "$$U");
        userDefaultsFile.close();
    } else {
        QFile defaults(":/text/settings/default.cfg");
        if (defaults.open(QFile::ReadOnly | QFile::Text)) {
            QByteArray defaultSettings = defaults.readAll();
            defaults.close();

            // Create defaults file
            if (!QDir(QCoreApplication::applicationDirPath() + "/settings").exists())
                QDir().mkdir((QCoreApplication::applicationDirPath() + "/settings"));
            QString defaultName = QString(QCoreApplication::applicationDirPath()) + QString("/settings/default.cfg");
            QFile file(defaultName);
            if (file.open(QFile::WriteOnly | QFile::Truncate)) {
                file.write(defaultSettings);
                file.close();
            }

            // Apply defaults
            emit sendManualInput("$$S" + defaultSettings.replace("\n", "") + "$$U");
        }
    }
}

void MainWindow::saveToFile(QByteArray data) {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save file"), "", tr("Text file (*.txt);;Any file (*.*)"));
    if (fileName.isEmpty())
        return;
    QFile file(fileName);
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        file.write(data);
        file.close();
    } else {
        QMessageBox msgBox(this);
        msgBox.setText(tr("Cant write to file. (if installed in ProgramFiles, admin rights are required)"));
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
    }
}
