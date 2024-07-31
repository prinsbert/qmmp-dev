/***************************************************************************
 *   Copyright (C) 2008-2024 by Ilya Kotov                                 *
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
#include <QRegularExpression>
#include <QTextCodec>
#include <qmmp/qmmp.h>

#ifdef WITH_ENCA
#include <enca.h>
#endif

#include "ui_cuesettingsdialog.h"
#include "cuesettingsdialog.h"

CueSettingsDialog::CueSettingsDialog(QWidget *parent)
        : QDialog(parent), m_ui(new Ui::CueSettingsDialog)
{
    m_ui->setupUi(this);
    findCodecs();
    for(const QTextCodec *codec : qAsConst(m_codecs))
        m_ui->cueEncComboBox->addItem(QString::fromLatin1(codec->name()));

#ifdef WITH_ENCA
    size_t n = 0;
    const char **langs = enca_get_languages(&n);
    for (size_t i = 0; i < n; ++i)
        m_ui->encaAnalyserComboBox->addItem(QString::fromLatin1(langs[i]));
#endif
    QSettings settings;
    settings.beginGroup(u"CUE"_s);
    int pos = m_ui->cueEncComboBox->findText(settings.value(u"encoding"_s, u"UTF-8"_s).toString());
    m_ui->cueEncComboBox->setCurrentIndex(pos);
#ifdef WITH_ENCA
    m_ui->autoCharsetCheckBox->setChecked(settings.value(u"use_enca"_s, false).toBool());
    pos = m_ui->encaAnalyserComboBox->findText(settings.value(u"enca_lang"_s, QString::fromLatin1(langs[n - 1])).toString());
    m_ui->encaAnalyserComboBox->setCurrentIndex(pos);
#else
    m_ui->autoCharsetCheckBox->setEnabled(false);
#endif
    m_ui->dirtyCueCheckBox->setChecked(settings.value(u"dirty_cue"_s, false).toBool());
    settings.endGroup();
}

CueSettingsDialog::~CueSettingsDialog()
{
    delete m_ui;
}

void CueSettingsDialog::accept()
{
    QSettings settings;
    settings.beginGroup(u"CUE"_s);
    settings.setValue(u"encoding"_s, m_ui->cueEncComboBox->currentText());
#ifdef WITH_ENCA
    settings.setValue(u"use_enca"_s, m_ui->autoCharsetCheckBox->isChecked());
    settings.setValue(u"enca_lang"_s, m_ui->encaAnalyserComboBox->currentText());
#endif
    settings.setValue(u"dirty_cue"_s, m_ui->dirtyCueCheckBox->isChecked());
    settings.endGroup();
    QDialog::accept();
}

void CueSettingsDialog::findCodecs()
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
