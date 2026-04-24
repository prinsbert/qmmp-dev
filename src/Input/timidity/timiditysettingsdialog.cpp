/***************************************************************************
 *   Copyright (C) 2010-2026 by Ilya Kotov                                 *
 *   forkotov02@ya.ru                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include <QSettings>
#include <QStringList>
#include <qmmp/qmmp.h>
#include "timidityhelper.h"
#include "ui_timiditysettingsdialog.h"
#include "timiditysettingsdialog.h"

TiMiditySettingsDialog::TiMiditySettingsDialog(QWidget *parent)
        : QDialog(parent), m_ui(new Ui::TiMiditySettingsDialog)
{
    m_ui->setupUi(this);

    QSettings settings;
    settings.beginGroup(u"TiMidity"_s);
    QStringList files = TiMidityHelper::instance()->configFiles();
    QString configPath = files.isEmpty() ? QString() : files.constFirst();
    m_ui->confPathComboBox->addItems(files);
    m_ui->confPathComboBox->setEditText(settings.value(u"conf_path"_s, configPath).toString());
    m_ui->sampleRateComboBox->addItem(tr("44100 Hz"), 44100);
    m_ui->sampleRateComboBox->addItem(tr("48000 Hz"), 48000);
    int i = m_ui->sampleRateComboBox->findData(settings.value(u"sample_rate"_s, 48000).toInt());
    m_ui->sampleRateComboBox->setCurrentIndex(i);
    settings.endGroup();
}

TiMiditySettingsDialog::~TiMiditySettingsDialog()
{
    delete m_ui;
}

void TiMiditySettingsDialog::accept()
{
    QSettings settings;
    settings.beginGroup(u"TiMidity"_s);
    settings.setValue(u"conf_path"_s, m_ui->confPathComboBox->currentText());
    settings.setValue(u"sample_rate"_s, m_ui->sampleRateComboBox->itemData(m_ui->sampleRateComboBox->currentIndex()));
    settings.endGroup();
    TiMidityHelper::instance()->readSettings();
    QDialog::accept();
}
