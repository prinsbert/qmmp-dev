/***************************************************************************
 *   Copyright (C) 2009-2025 by Ilya Kotov                                 *
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
#include <qmmp/qmmp.h>
#include "ui_mplayersettingsdialog.h"
#include "mplayersettingsdialog.h"

MplayerSettingsDialog::MplayerSettingsDialog(QWidget *parent)
        : QDialog(parent), m_ui(new Ui::MplayerSettingsDialog)
{
    m_ui->setupUi(this);
    m_ui->videoComboBox->addItem(tr("default"));
    m_ui->videoComboBox->addItems( { u"xv"_s, u"x11"_s, u"gl"_s, u"gl2"_s, u"dga"_s, u"sdl"_s, u"null"_s });


    m_ui->audioComboBox->addItem(tr("default"));
    m_ui->audioComboBox->addItems( { u"oss"_s, u"alsa"_s, u"pulse"_s, u"jack"_s, u"nas"_s, u"null"_s });
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup(u"mplayer"_s);
    m_ui->audioComboBox->setEditText(settings.value(u"ao"_s, u"default"_s).toString().replace(u"default"_s, tr("default")));
    m_ui->videoComboBox->setEditText(settings.value(u"vo"_s, u"default"_s).toString().replace(u"default"_s, tr("default")));
    m_ui->autoSyncCheckBox->setChecked(settings.value(u"autosync"_s, false).toBool());
    m_ui->syncFactorSpinBox->setValue(settings.value(u"autosync_factor"_s, 100).toInt());
    m_ui->cmdOptionsLineEdit->setText(settings.value(u"cmd_options"_s).toString());
    settings.endGroup();
}

MplayerSettingsDialog::~MplayerSettingsDialog()
{
    delete m_ui;
}

void MplayerSettingsDialog::accept()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup(u"mplayer"_s);
    settings.setValue(u"ao"_s, m_ui->audioComboBox->currentText().replace(tr("default"), u"default"_s));
    settings.setValue(u"vo"_s, m_ui->videoComboBox->currentText().replace(tr("default"), u"default"_s));
    settings.setValue(u"autosync"_s, m_ui->autoSyncCheckBox->isChecked());
    settings.setValue(u"autosync_factor"_s, m_ui->syncFactorSpinBox->value());
    settings.setValue(u"cmd_options"_s, m_ui->cmdOptionsLineEdit->text());
    settings.endGroup();
    QDialog::accept();
}
