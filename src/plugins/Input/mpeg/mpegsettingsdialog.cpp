/***************************************************************************
 *   Copyright (C) 2006-2024 by Ilya Kotov                                 *
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
#include <QTextCodec>
#include <qmmp/qmmp.h>
#include "ui_mpegsettingsdialog.h"
#include "mpegsettingsdialog.h"

MpegSettingsDialog::MpegSettingsDialog(bool using_rusxmms, QWidget *parent)
        : QDialog(parent), m_ui(new Ui::MpegSettingsDialog)
{
    m_ui->setupUi(this);
    findCodecs();
    for(const QTextCodec *codec : qAsConst(m_codecs))
    {
        m_ui->id3v1EncComboBox->addItem(QString::fromLatin1(codec->name()));
        m_ui->id3v2EncComboBox->addItem(QString::fromLatin1(codec->name()));
    }

    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup(u"MPEG"_s);

#if defined(WITH_MAD) && defined(WITH_MPG123)
    QString decoderName = settings.value(u"decoder"_s, u"mad"_s).toString();
    m_ui->madRadioButton->setChecked(true);
    m_ui->mpg123RadioButton->setChecked(decoderName == "mpg123"_L1);
    m_ui->enableCrcCheckBox->setChecked(settings.value(u"enable_crc"_s, false).toBool());
#elif defined(WITH_MAD)
    m_ui->madRadioButton->setChecked(true);
    m_ui->decoderGroupBox->setEnabled(false);
#elif defined(WITH_MPG123)
    m_ui->mpg123RadioButton->setChecked(true);
    m_ui->decoderGroupBox->setEnabled(false);
#endif

    int pos = m_ui->id3v1EncComboBox->findText(settings.value(u"ID3v1_encoding"_s, u"ISO-8859-1"_s).toString());
    m_ui->id3v1EncComboBox->setCurrentIndex(pos);
    pos = m_ui->id3v2EncComboBox->findText(settings.value(u"ID3v2_encoding"_s, u"UTF-8"_s).toString());
    m_ui->id3v2EncComboBox->setCurrentIndex(pos);

    m_ui->firstTagComboBox->setCurrentIndex(settings.value(u"tag_1"_s, ID3v2).toInt());
    m_ui->secondTagComboBox->setCurrentIndex(settings.value(u"tag_2"_s, APE).toInt());
    m_ui->thirdTagComboBox->setCurrentIndex(settings.value(u"tag_3"_s, ID3v1).toInt());
    m_ui->mergeTagsCheckBox->setChecked(settings.value(u"merge_tags"_s, false).toBool());
    m_ui->detectEncodingCheckBox->setChecked(settings.value(u"detect_encoding"_s, false).toBool());

    settings.endGroup();

    if(using_rusxmms)
    {
        m_ui->id3v1EncComboBox->setEnabled(false);
        m_ui->id3v2EncComboBox->setEnabled(false);
        m_ui->detectEncodingCheckBox->setEnabled(false);
    }
}

MpegSettingsDialog::~MpegSettingsDialog()
{
    delete m_ui;
}

void MpegSettingsDialog::accept()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup(u"MPEG"_s);
    settings.setValue(u"decoder"_s, m_ui->mpg123RadioButton->isChecked() ? u"mpg123"_s : u"mad"_s);
    settings.setValue(u"enable_crc"_s, m_ui->enableCrcCheckBox->isChecked());
    settings.setValue(u"ID3v1_encoding"_s, m_ui->id3v1EncComboBox->currentText());
    settings.setValue(u"ID3v2_encoding"_s, m_ui->id3v2EncComboBox->currentText());
    settings.setValue(u"detect_encoding"_s, m_ui->detectEncodingCheckBox->isChecked());
    settings.setValue(u"tag_1"_s, m_ui->firstTagComboBox->currentIndex());
    settings.setValue(u"tag_2"_s, m_ui->secondTagComboBox->currentIndex());
    settings.setValue(u"tag_3"_s, m_ui->thirdTagComboBox->currentIndex());
    settings.setValue(u"merge_tags"_s, m_ui->mergeTagsCheckBox->isChecked());
    settings.endGroup();
    QDialog::accept();
}

void MpegSettingsDialog::findCodecs()
{
    QMap<QString, QTextCodec *> codecMap;
    static const QRegularExpression iso8859RegExp(u"ISO[- ]8859-([0-9]+).*"_s);

    for(int mib : QTextCodec::availableMibs())
    {
        QTextCodec *codec = QTextCodec::codecForMib(mib);

        QString sortKey = QString::fromLatin1(codec->name().toUpper());
        int rank;
        QRegularExpressionMatch match;

        if (sortKey.startsWith(u"UTF-8"_s))
        {
            rank = 1;
        }
        else if (sortKey.startsWith(u"UTF-16"_s))
        {
            rank = 2;
        }
        else if ((match = iso8859RegExp.match(sortKey)).hasMatch())
        {
            if (match.captured(1).size() == 1)
                rank = 3;
            else
                rank = 4;
        }
        else
        {
            rank = 5;
        }
        sortKey.prepend(QChar('0' + rank));

        codecMap.insert(sortKey, codec);
    }
    m_codecs = codecMap.values();
}
