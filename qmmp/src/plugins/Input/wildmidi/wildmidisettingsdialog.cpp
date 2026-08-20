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
#include <wildmidi_lib.h>
#include <qmmp/qmmp.h>
#include "wildmidihelper.h"
#include "ui_wildmidisettingsdialog.h"
#include "wildmidisettingsdialog.h"

WildMidiSettingsDialog::WildMidiSettingsDialog(QWidget *parent)
        : QDialog(parent), m_ui(new Ui::WildMidiSettingsDialog)
{
    m_ui->setupUi(this);

    QSettings settings;
    settings.beginGroup(u"Midi"_s);
    QStringList files = WildMidiHelper::instance()->configFiles();
    QString configPath = files.isEmpty() ? QString() : files.constFirst();
    m_ui->confPathComboBox->addItems(files);
    m_ui->confPathComboBox->setEditText(settings.value(u"conf_path"_s, configPath).toString());
    m_ui->sampleRateComboBox->addItem(tr("44100 Hz"), 44100);
    m_ui->sampleRateComboBox->addItem(tr("48000 Hz"), 48000);
    int i = m_ui->sampleRateComboBox->findData(settings.value(u"sample_rate"_s, 48000).toInt());
    m_ui->sampleRateComboBox->setCurrentIndex(i);
    m_ui->enhancedResamplingCheckBox->setChecked(settings.value(u"enhanced_resampling"_s, false).toBool());
    m_ui->reverbCheckBox->setChecked(settings.value(u"reverberation"_s, false).toBool());
#if LIBWILDMIDI_VERSION >= 0x000500L
    m_ui->opl3CheckBox->setChecked(settings.value(u"use_opl3"_s, false).toBool());
#else
    m_ui->opl3CheckBox->setVisible(false);
#endif
    settings.endGroup();
}

WildMidiSettingsDialog::~WildMidiSettingsDialog()
{
    delete m_ui;
}

void WildMidiSettingsDialog::accept()
{
    QSettings settings;
    settings.beginGroup(u"Midi"_s);
    settings.setValue(u"conf_path"_s, m_ui->confPathComboBox->currentText());
    settings.setValue(u"sample_rate"_s,
                      m_ui->sampleRateComboBox->itemData(m_ui->sampleRateComboBox->currentIndex()));
    settings.setValue(u"enhanced_resampling"_s, m_ui->enhancedResamplingCheckBox->isChecked());
    settings.setValue(u"reverberation"_s, m_ui->reverbCheckBox->isChecked());
#if LIBWILDMIDI_VERSION >= 0x000500L
    settings.setValue(u"use_opl3"_s, m_ui->opl3CheckBox->isChecked());
#endif
    settings.endGroup();
    WildMidiHelper::instance()->readSettings();
    QDialog::accept();
}
