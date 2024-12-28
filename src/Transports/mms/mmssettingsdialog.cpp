/***************************************************************************
 *   Copyright (C) 2010-2025 by Ilya Kotov                                 *
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
#include "ui_mmssettingsdialog.h"
#include "mmssettingsdialog.h"

MmsSettingsDialog::MmsSettingsDialog(QWidget *parent)
        : QDialog(parent), m_ui(new Ui::MmsSettingsDialog)
{
    m_ui->setupUi(this);
    QSettings settings;
    m_ui->bufferSizeSpinBox->setValue(settings.value("MMS/buffer_size"_L1, 384).toInt());
}


MmsSettingsDialog::~MmsSettingsDialog()
{
    delete m_ui;
}

void MmsSettingsDialog::accept()
{
    QSettings settings;
    settings.setValue("MMS/buffer_size"_L1, m_ui->bufferSizeSpinBox->value());
    QDialog::accept();
}
