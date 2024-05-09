/***************************************************************************
 *   Copyright (C) 2009-2022 by Ilya Kotov                                 *
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

#include <QFile>
#include <stdint.h>
#include <libmodplug/stdafx.h>
#include <libmodplug/it_defs.h>
#include <libmodplug/sndfile.h>
#include <libmodplug/modplug.h>
#include "archivereader.h"
#include "modplugmetadatamodel.h"

#define MAX_MESSAGE_LENGTH 4000

ModPlugMetaDataModel::ModPlugMetaDataModel(const QString &path) : MetaDataModel(true),
    m_path(path)
{
    ArchiveReader reader(nullptr);
    if(reader.isSupported(m_path))
    {
        m_buffer = reader.unpack(m_path);
    }
    else
    {
        QFile file(m_path);
        if(!file.open(QIODevice::ReadOnly))
        {
            qWarning("DetailsDialog: error: %s", qPrintable(file.errorString ()));
            return;
        }
        m_buffer = file.readAll();
        file.close();
    }
    m_soundFile = new CSoundFile();
    m_soundFile->Create((uchar*) m_buffer.data(), m_buffer.size());
}

ModPlugMetaDataModel::~ModPlugMetaDataModel()
{
    if(m_soundFile)
    {
        m_soundFile->Destroy();
        delete m_soundFile;
    }
}

QList<MetaDataItem> ModPlugMetaDataModel::extraProperties() const
{
    QList<MetaDataItem> ep;
    if(!m_soundFile)
        return ep;

    ep << MetaDataItem(tr("Speed"), m_soundFile->GetMusicSpeed());
    ep << MetaDataItem(tr("Tempo"), m_soundFile->GetMusicTempo());
    ep << MetaDataItem(tr("Samples"), m_soundFile->GetNumSamples());
    ep << MetaDataItem(tr("Instruments"), m_soundFile->GetNumInstruments());
    ep << MetaDataItem(tr("Patterns"), m_soundFile->GetNumPatterns());
    ep << MetaDataItem(tr("Channels"), m_soundFile->GetNumChannels());
    return ep;
}

QList<MetaDataItem> ModPlugMetaDataModel::descriptions() const
{
    QList<MetaDataItem> desc;
    if(!m_soundFile)
        return desc;
    char lBuffer[33];
    QString text;
    for(uint i = 0; i < m_soundFile->GetNumSamples(); i++)
    {
        m_soundFile->GetSampleName(i, lBuffer);
        text += QString::fromUtf8(lBuffer) + QChar::LineFeed;
    }
    text = text.trimmed();
    if(!text.isEmpty())
        desc << MetaDataItem(tr("Samples"), text);
    text.clear();
    for(uint i = 0; i < m_soundFile->GetNumInstruments(); i++)
    {
        m_soundFile->GetInstrumentName(i, lBuffer);
        text += QString::fromUtf8(lBuffer) + QChar::LineFeed;
    }
    text = text.trimmed();
    if(!text.isEmpty())
        desc << MetaDataItem(tr("Instruments"), text);
    text.clear();
    char message[MAX_MESSAGE_LENGTH];
    int length = m_soundFile->GetSongComments(message, MAX_MESSAGE_LENGTH, 80);
    if (length != 0)
        desc << MetaDataItem(tr("Comment"), QString::fromUtf8(message).trimmed ());
    return desc;
}

QString ModPlugMetaDataModel::getTypeName(quint32 type)
{
    switch (type) {
    case MOD_TYPE_MOD:
        return u"ProTracker"_s;
    case MOD_TYPE_S3M:
        return u"Scream Tracker 3"_s;
    case MOD_TYPE_XM:
        return u"Fast Tracker 2"_s;
    case MOD_TYPE_IT:
        return u"Impulse Tracker"_s;
    case MOD_TYPE_MED:
        return u"OctaMed"_s;
    case MOD_TYPE_MTM:
        return u"MTM"_s;
    case MOD_TYPE_669:
        return u"669 Composer / UNIS 669"_s;
    case MOD_TYPE_ULT:
        return u"ULT"_s;
    case MOD_TYPE_STM:
        return u"Scream Tracker"_s;
    case MOD_TYPE_FAR:
        return u"Farandole"_s;
    case MOD_TYPE_AMF:
        return u"ASYLUM Music Format"_s;
    case MOD_TYPE_AMS:
        return u"AMS module"_s;
    case MOD_TYPE_DSM:
        return u"DSIK Internal Format"_s;
    case MOD_TYPE_MDL:
        return u"DigiTracker"_s;
    case MOD_TYPE_OKT:
        return u"Oktalyzer"_s;
    case MOD_TYPE_DMF:
        return u"Delusion Digital Music Fileformat (X-Tracker)"_s;
    case MOD_TYPE_PTM:
        return u"PolyTracker"_s;
    case MOD_TYPE_DBM:
        return u"DigiBooster Pro"_s;
    case MOD_TYPE_MT2:
        return u"MT2"_s;
    case MOD_TYPE_AMF0:
        return u"AMF0"_s;
    case MOD_TYPE_PSM:
        return u"PSM"_s;
    default:
        ;
    }
    return u"Unknown"_s;
}
