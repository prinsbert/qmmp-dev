/***************************************************************************
 *   Copyright (C) 2008-2025 by Ilya Kotov                                 *
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
#include <QDir>
#include <libmodplug/stdafx.h>
#include <libmodplug/it_defs.h>
#include <libmodplug/sndfile.h>
#include "decoder_modplug.h"
#include "ui_modplugsettingsdialog.h"
#include "modplugsettingsdialog.h"

ModPlugSettingsDialog::ModPlugSettingsDialog(QWidget *parent)
        : QDialog(parent), m_ui(new Ui::ModPlugSettingsDialog)
{
    m_ui->setupUi(this);

    connect(m_ui->surDepthSlider, &QSlider::valueChanged, m_ui->label_11, qOverload<int>(&QLabel::setNum));
    connect(m_ui->surDelaySlider, &QSlider::valueChanged, m_ui->label_12, qOverload<int>(&QLabel::setNum));
    connect(m_ui->reverbDepthSlider, &QSlider::valueChanged, m_ui->label_9, qOverload<int>(&QLabel::setNum));
    connect(m_ui->reverbDelaySlider, &QSlider::valueChanged, m_ui->label_10, qOverload<int>(&QLabel::setNum));
    connect(m_ui->bassRangeSlider, &QSlider::valueChanged, m_ui->label_14, qOverload<int>(&QLabel::setNum));
    connect(m_ui->bassAmountSlider, &QSlider::valueChanged, m_ui->label_13, qOverload<int>(&QLabel::setNum));

    QSettings settings;
    settings.beginGroup("ModPlug"_L1);
    //general
    m_ui->noiseCheckBox->setChecked(settings.value("NoiseReduction"_L1, false).toBool());
    m_ui->fileNameCheckBox->setChecked(settings.value("UseFileName"_L1, false).toBool());
    m_ui->amigaCheckBox->setChecked(settings.value("GrabAmigaMOD"_L1, true).toBool());
    //settings.value("Oversampling", true).toBool();
    //settings.value("VolumeRamp", true).toBool();
    //settings.value("FastInfo", true).toBool();
    //channels number
    if (settings.value("Channels"_L1, 2).toInt() == 2)
        m_ui->stereoRadioButton->setChecked(true);
    else
        m_ui->monoRadioButton->setChecked(true);
    //bits number
    if (settings.value("Bits"_L1, 16).toInt() == 8)
        m_ui->bit8RadioButton->setChecked(true);
    else
        m_ui->bit16RadioButton->setChecked(true);
    //resampling frequency
    int freq = settings.value("Frequency"_L1, 44100).toInt();
    if (freq == 48000)
        m_ui->khz48RadioButton->setChecked(true);
    else if (freq == 44100)
        m_ui->khz44RadioButton->setChecked(true);
    else if (freq == 22050)
        m_ui->khz22RadioButton->setChecked(true);
    else
        m_ui->khz11RadioButton->setChecked(true);
    //resampling mode
    int res = settings.value("ResamplineMode"_L1, SRCMODE_POLYPHASE).toInt();
    if (res == SRCMODE_NEAREST)
        m_ui->resampNearestRadioButton->setChecked(true);
    else if (res == SRCMODE_LINEAR)
        m_ui->resampLinearRadioButton->setChecked(true);
    else if (res == SRCMODE_SPLINE)
        m_ui->resampSplineRadioButton->setChecked(true);
    else
        m_ui->resampPolyphaseRadioButton->setChecked(true);
    //reverberation
    m_ui->reverbGroupBox->setChecked(settings.value("Reverb"_L1, false).toBool());
    m_ui->reverbDepthSlider->setValue(settings.value("ReverbDepth"_L1, 30).toInt());
    m_ui->reverbDelaySlider->setValue(settings.value("ReverbDelay"_L1, 100).toInt());
    //surround
    m_ui->surGroupBox->setChecked(settings.value("Surround"_L1, true).toBool());
    m_ui->surDepthSlider->setValue(settings.value("SurroundDepth"_L1, 20).toInt());
    m_ui->surDelaySlider->setValue(settings.value("SurroundDelay"_L1, 20).toInt());
    //bass
    m_ui->bassGroupBox->setChecked(settings.value("Megabass"_L1, false).toBool());
    m_ui->bassAmountSlider->setValue(settings.value("BassAmount"_L1, 40).toInt());
    m_ui->bassRangeSlider->setValue(settings.value("BassRange"_L1, 30).toInt());
    //preamp
    m_ui->preampGroupBox->setChecked(settings.value("PreAmp"_L1, false).toBool());
    connect(m_ui->preampSlider, &QSlider::valueChanged, this, &ModPlugSettingsDialog::setPreamp);
    m_ui->preampSlider->setValue(int(settings.value("PreAmpLevel"_L1, 0.0f).toDouble() * 10));
    //looping
    int l = settings.value("LoopCount"_L1, 0).toInt();
    if (l == 0)
        m_ui->dontLoopRadioButton->setChecked(true);
    else if (l < 0)
        m_ui->loopForeverRadioButton->setChecked(true);
    else
    {
        m_ui->loopRadioButton->setChecked(true);
        m_ui->loopSpinBox->setValue(l);
    }
    settings.endGroup();
}


ModPlugSettingsDialog::~ModPlugSettingsDialog()
{
    delete m_ui;
}

void ModPlugSettingsDialog::writeSettings()
{
    QSettings settings;
    settings.beginGroup("ModPlug"_L1);
    //general
    settings.setValue("NoiseReduction"_L1, m_ui->noiseCheckBox->isChecked());
    settings.setValue("UseFileName"_L1, m_ui->fileNameCheckBox->isChecked());
    settings.setValue("GrabAmigaMOD"_L1, m_ui->amigaCheckBox->isChecked());
    //settings.value("Oversampling", true).toBool();
    //settings.value("VolumeRamp", true).toBool();
    //settings.value("FastInfo", true).toBool();
    //channels number
    settings.setValue("Channels"_L1, m_ui->stereoRadioButton->isChecked() ? 2 : 1 );
    //bits number
    settings.setValue("Bits"_L1, m_ui->bit8RadioButton->isChecked() ? 8 : 16 );
    //resampling frequency
    if (m_ui->khz48RadioButton->isChecked())
        settings.setValue("Frequency"_L1, 48000);
    else if (m_ui->khz44RadioButton->isChecked())
        settings.setValue("Frequency"_L1, 44100);
    else if (m_ui->khz22RadioButton->isChecked())
        settings.setValue("Frequency"_L1, 22050);
    else
        settings.setValue("Frequency"_L1, 11025);
    //resampling mode
    if (m_ui->resampNearestRadioButton->isChecked())
        settings.setValue("ResamplineMode"_L1, SRCMODE_NEAREST);
    else if (m_ui->resampLinearRadioButton->isChecked())
        settings.setValue("ResamplineMode"_L1, SRCMODE_LINEAR);
    else if (m_ui->resampSplineRadioButton->isChecked())
        settings.setValue("ResamplineMode"_L1, SRCMODE_SPLINE);
    else
        settings.setValue("ResamplineMode"_L1, SRCMODE_POLYPHASE);
    //reverberation
    settings.setValue("Reverb"_L1,  m_ui->reverbGroupBox->isChecked());
    settings.setValue("ReverbDepth"_L1, m_ui->reverbDepthSlider->value());
    settings.setValue("ReverbDelay"_L1, m_ui->reverbDelaySlider->value());
    //surround
    settings.setValue("Surround"_L1, m_ui->surGroupBox->isChecked());
    settings.setValue("SurroundDepth"_L1, m_ui->surDepthSlider->value());
    settings.setValue("SurroundDelay"_L1, m_ui->surDelaySlider->value());
    //bass
    settings.setValue("Megabass"_L1, m_ui->bassGroupBox->isChecked());
    settings.setValue("BassAmount"_L1, m_ui->bassAmountSlider->value());
    settings.setValue("BassRange"_L1, m_ui->bassRangeSlider->value());
    //preamp
    settings.setValue("PreAmp"_L1, m_ui->preampGroupBox->isChecked());
    settings.setValue("PreAmpLevel"_L1, (double) m_ui->preampSlider->value() / 10);
    //looping
    if (m_ui->dontLoopRadioButton->isChecked())
        settings.setValue("LoopCount"_L1, 0);
    else if (m_ui->loopForeverRadioButton->isChecked())
        settings.setValue("LoopCount"_L1, -1);
    else
        settings.setValue("LoopCount"_L1, m_ui->loopSpinBox->value());
    settings.endGroup();
    //apply settings for the created decoder
    if (DecoderModPlug::instance())
    {
        //DecoderModPlug::instance()->mutex()->lock();
        DecoderModPlug::instance()->readSettings();
        //DecoderModPlug::instance()->mutex()->unlock();
    }
}

void ModPlugSettingsDialog::setPreamp(int preamp)
{
    m_ui->preampLabel->setText(QStringLiteral("%1").arg((double) preamp/10));
}

void ModPlugSettingsDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    switch ((int) m_ui->buttonBox->buttonRole(button))
    {
    case QDialogButtonBox::AcceptRole:
        writeSettings();
        accept();
        break;
    case QDialogButtonBox::ApplyRole:
        writeSettings();
        break;
    }
}
