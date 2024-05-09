/***************************************************************************
 *   Copyright (C) 2007-2024 by Ilya Kotov                                 *
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
#include "ui_srconvertersettingsdialog.h"
#include "srconvertersettingsdialog.h"

SRConverterSettingsDialog::SRConverterSettingsDialog(QWidget *parent) : QDialog(parent),
    m_ui(new Ui::SRConverterSettingsDialog)
{
    m_ui->setupUi(this);
    QSettings settings;
    m_ui->srSpinBox->setValue(settings.value("SRC/sample_rate"_L1, 48000).toInt());
    m_ui->engineComboBox->setCurrentIndex(settings.value("SRC/engine"_L1, 0).toInt());
}


SRConverterSettingsDialog::~SRConverterSettingsDialog()
{
    delete m_ui;
}

void SRConverterSettingsDialog::accept()
{
    QSettings settings;
    settings.setValue("SRC/sample_rate"_L1, m_ui->srSpinBox->value());
    settings.setValue("SRC/engine"_L1, m_ui->engineComboBox->currentIndex());
    QDialog::accept();
}
