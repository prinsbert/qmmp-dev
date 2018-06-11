/***************************************************************************
 *   Copyright (C) 2011-2018 by Ilya Kotov                                 *
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

#include <QMessageBox>
#include <QtPlugin>
#include <QTranslator>
#include <QRegExp>
#include <taglib/apefile.h>
#include <taglib/apetag.h>
#if (TAGLIB_MAJOR_VERSION > 1) || ((TAGLIB_MAJOR_VERSION == 1) && (TAGLIB_MINOR_VERSION >= 8))
#include <taglib/tfilestream.h>
#endif
#include "replaygainreader.h"
#include "ffapmetadatamodel.h"
#include "decoderffapfactory.h"
#include "decoder_ffap.h"
#include "decoder_ffapcue.h"
#include "cueparser.h"

// DecoderFFapFactory

bool DecoderFFapFactory::canDecode(QIODevice *input) const
{
    char buf[3];
    return (input->peek(buf, 3) == 3 && !memcmp(buf, "MAC", 3));
}

const DecoderProperties DecoderFFapFactory::properties() const
{
    DecoderProperties properties;
    properties.name = tr("FFap Plugin");
    properties.filters << "*.ape";
    properties.description = tr("Monkey's Audio Files");
    //properties.contentType = ;
    properties.shortName = "ffap";
    properties.hasAbout = true;
    properties.hasSettings = false;
    properties.noInput = false;
    properties.protocols << "ape";
    properties.priority = 9;
    return properties;
}

Decoder *DecoderFFapFactory::create(const QString &url, QIODevice *i)
{
    Decoder *d;
    if(url.contains("://"))
        d = new DecoderFFapCUE(url);
    else
    {
        d = new DecoderFFap(url, i);
        ReplayGainReader rg(url);
        d->setReplayGainInfo(rg.replayGainInfo());
    }
    return d;
}

QList<TrackInfo *> DecoderFFapFactory::createPlayList(const QString &path, TrackInfo::Parts parts, QStringList *)
{
    QList <TrackInfo*> list;

    //extract metadata of one cue track
    if(path.contains("://"))
    {
        QString filePath = path;
        filePath.remove("ape://");
        filePath.remove(QRegExp("#\\d+$"));
        int track = filePath.section("#", -1).toInt();
        list = createPlayList(filePath, parts, 0);
        if (list.isEmpty() || track <= 0 || track > list.count())
        {
            qDeleteAll(list);
            list.clear();
            return list;
        }
        TrackInfo *info = list.takeAt(track - 1);
        qDeleteAll(list);
        return QList<TrackInfo *>() << info;
    }

#if (TAGLIB_MAJOR_VERSION > 1) || ((TAGLIB_MAJOR_VERSION == 1) && (TAGLIB_MINOR_VERSION >= 8))
    TagLib::FileStream stream(QStringToFileName(path), true);
    TagLib::APE::File file(&stream);
#else
    TagLib::APE::File file(QStringToFileName(path));
#endif
    TagLib::APE::Tag *tag = file.APETag();
    TagLib::APE::Properties *ap = file.audioProperties();

    TrackInfo *info = new TrackInfo(path);
    if((parts & TrackInfo::MetaData) && tag && !tag->isEmpty())
    {
        info->setValue(Qmmp::ALBUM, TStringToQString(tag->album()));
        info->setValue(Qmmp::ARTIST, TStringToQString(tag->artist()));
        info->setValue(Qmmp::COMMENT, TStringToQString(tag->comment()));
        info->setValue(Qmmp::GENRE, TStringToQString(tag->genre()));
        info->setValue(Qmmp::TITLE, TStringToQString(tag->title()));
        info->setValue(Qmmp::YEAR, tag->year());
        info->setValue(Qmmp::TRACK, tag->track());

        //additional metadata
        TagLib::APE::Item fld;
        if(!(fld = tag->itemListMap()["COMPOSER"]).isEmpty())
            info->setValue(Qmmp::COMPOSER, TStringToQString(fld.toString()));

        if (tag->itemListMap().contains("CUESHEET"))
        {
            delete info;
            CUEParser parser(tag->itemListMap()["CUESHEET"].toString().toCString(true), path);
            return parser.createPlayList();
        }
    }

    if(ap)
    {
        info->setDuration(ap->lengthInMilliseconds());
        if(parts & TrackInfo::Properties)
        {
            info->setValue(Qmmp::BITRATE, ap->bitrate());
            info->setValue(Qmmp::SAMPLERATE, ap->sampleRate());
            info->setValue(Qmmp::CHANNELS, ap->channels());
            info->setValue(Qmmp::BITS_PER_SAMPLE, ap->bitsPerSample());
            info->setValue(Qmmp::FORMAT_NAME, "APE");
        }
    }

    list << info;
    return list;
}

MetaDataModel* DecoderFFapFactory::createMetaDataModel(const QString &path, QObject *parent)
{
    return new FFapMetaDataModel(path, parent);
}

void DecoderFFapFactory::showSettings(QWidget *)
{}

void DecoderFFapFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About FFap Audio Plugin"),
                        tr("Qmmp FFap Audio Plugin")+"\n"+
                        tr("This plugin provides Monkey's Audio (APE) support") +"\n"+
                        tr("Written by: Ilya Kotov <forkotov02@ya.ru>")  +"\n"+
                        tr("Based on code from deadbeef, FFmpeg and rockbox"));
}

QTranslator *DecoderFFapFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = Qmmp::systemLanguageID();
    translator->load(QString(":/ffap_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN2(ffap,DecoderFFapFactory)
