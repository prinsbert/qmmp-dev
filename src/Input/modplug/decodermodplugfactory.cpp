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
#include <QFile>
#include <QMessageBox>
#include <QStringList>
#include <libmodplug/stdafx.h>
#include <libmodplug/it_defs.h>
#include <libmodplug/sndfile.h>
#include "modplugsettingsdialog.h"
#include "modplugmetadatamodel.h"
#include "decoder_modplug.h"
#include "archivereader.h"
#include "decodermodplugfactory.h"


// DecoderModPlugFactory

bool DecoderModPlugFactory::canDecode(QIODevice *) const
{
    return false;
}

DecoderProperties DecoderModPlugFactory::properties() const
{
    DecoderProperties properties;
    properties.name = tr("ModPlug Plugin");
    properties.filters = QStringList { u"*.amf"_s, u"*.ams"_s, u"*.dbm"_s, u"*.dbf"_s, u"*.dsm"_s, u"*.far"_s, u"*.mdl"_s,
            u"*.stm"_s, u"*.ult"_s, u"*.j2b"_s, u"*.mt2"_s, u"*.mdz"_s, u"*.mdr"_s, u"*.mdgz"_s,
            u"*.mdbz"_s, u"*.mod"_s, u"*.s3z"_s, u"*.s3r"_s, u"*.s3gz"_s, u"*.s3m"_s, u"*.xmz"_s,
            u"*.xmr"_s, u"*.xmgz"_s, u"*.itz"_s, u"*.itr"_s, u"*.itgz"_s, u"*.dmf"_s, u"*.umx"_s,
            u"*.it"_s, u"*.669"_s, u"*.xm"_s, u"*.mtm"_s, u"*.psm"_s, u"*.ft2"_s, u"*.med"_s };
    properties.description = tr("ModPlug Files");
    //properties.contentType = ;
    properties.shortName = "modplug"_L1;
    properties.hasAbout = true;
    properties.hasSettings = true;
    properties.noInput = true;
    properties.protocols = QStringList { u"file"_s };
    properties.priority = 10;
    return properties;
}

Decoder *DecoderModPlugFactory::create(const QString &path, QIODevice *input)
{
    Q_UNUSED(input);
    return new DecoderModPlug(path);
}

QList<TrackInfo *> DecoderModPlugFactory::createPlayList(const QString &path, TrackInfo::Parts parts, QStringList *)
{
    QList <TrackInfo*> list;
    QSettings settings;
    bool useFileName = settings.value("UseFileName"_L1, false).toBool();

    QByteArray buffer;

    ArchiveReader reader(nullptr);
    if (reader.isSupported(path))
    {
        buffer = reader.unpack(path);
    }
    else
    {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly))
        {
            qCWarning(plugin) << "error:" << file.errorString();
            return list;
        }
        buffer = file.readAll();
        file.close();
    }

    if(!buffer.isEmpty())
    {
        CSoundFile *soundFile = new CSoundFile();
        soundFile->Create((uchar*) buffer.data(), buffer.size() + 1);
        TrackInfo *info = new TrackInfo(path);
        info->setDuration((qint64)soundFile->GetSongTime() * 1000);

        if(parts & TrackInfo::MetaData)
        {
            info->setValue(Qmmp::TITLE, useFileName ? path.section(QLatin1Char('/'), -1) :
                                                      QString::fromUtf8(soundFile->GetTitle()));
        }

        if(parts & TrackInfo::Properties)
        {
            //info->setValue(Qmmp::BITRATE);
            //info->setValue(Qmmp::SAMPLERATE);
            //info->setValue(Qmmp::CHANNELS);
            //info->setValue(Qmmp::BITS_PER_SAMPLE);
            info->setValue(Qmmp::FORMAT_NAME, ModPlugMetaDataModel::getTypeName(soundFile->GetType()));
        }

        list << info;
        soundFile->Destroy();
        delete soundFile;
    }
    return list;
}

MetaDataModel* DecoderModPlugFactory::createMetaDataModel(const QString &path, bool readOnly)
{
    Q_UNUSED(readOnly);
    return new ModPlugMetaDataModel(path);
}

QDialog *DecoderModPlugFactory::createSettings(QWidget *parent)
{
    return new ModPlugSettingsDialog(parent);
}

void DecoderModPlugFactory::showAbout(QWidget *parent)
{
    QMessageBox::about(parent, tr("About ModPlug Audio Plugin"),
                       tr("Qmmp ModPlug Audio Plugin") + QChar::LineFeed +
                       tr("Written by: Ilya Kotov <forkotov02@ya.ru>") + QChar::LineFeed +
                       tr("Based on the Modplug Plugin for Xmms") + QChar::LineFeed +
                       tr("Modplug Plugin developers:") + QChar::LineFeed +
                       tr("Olivier Lapicque <olivierl@jps.net>") + QChar::LineFeed +
                       tr("Kenton Varda <temporal@gauge3d.org>") + QChar::LineFeed +
                       tr("Konstanty Bialkowski <konstanty@ieee.org>"));
}

QString DecoderModPlugFactory::translation() const
{
    return QLatin1String(":/modplug_plugin_");
}
