/***************************************************************************
 *   Copyright (C) 2008-2026 by Ilya Kotov                                 *
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
#include "timidityhelper.h"
#include "decoder_timidity.h"
#include "timiditysettingsdialog.h"
#include "decodertimidityfactory.h"

// DecoderTiMidityFactory

bool DecoderTiMidityFactory::canDecode(QIODevice *input) const
{
    char buf[4];
    if(input->peek(buf, 4) != 4)
        return false;

    return !memcmp(buf, "MThd", 4);
}

DecoderProperties DecoderTiMidityFactory::properties() const
{
    DecoderProperties properties;
    properties.name = tr("TiMidity Plugin");
    properties.filters = QStringList { u"*.mid"_s };
    properties.description = tr("Midi Files");
    //properties.contentType = ;
    properties.priority = 10;
    properties.shortName = "timidity"_L1;
    properties.hasAbout = true;
    properties.hasSettings = true;
    properties.noInput = false;
    properties.protocols = QStringList { u"file"_s };
    properties.priority = 10;
    return properties;
}

Decoder *DecoderTiMidityFactory::create(const QString &path, QIODevice *input)
{
    Q_UNUSED(path);
    return new DecoderTiMidity(input);
}

QList<TrackInfo> DecoderTiMidityFactory::createPlayList(const QString &path, TrackInfo::Parts parts, QStringList *)
{
    TrackInfo info(path);
    TiMidityHelper *helper = TiMidityHelper::instance();

    if((parts & TrackInfo::Properties) && helper->initialize() && helper->sampleRate())
    {
        MidIStream *stream = mid_istream_open_file(path.toLocal8Bit().constData());
        if(stream)
        {
            TiMidityHelper::instance()->addPtr(stream);

            MidSongOptions options;
            options.rate = TiMidityHelper::instance()->sampleRate();
            options.format = MID_AUDIO_U16LSB;
            options.channels = 2;
            options.buffer_size = TiMidityHelper::instance()->sampleRate();

            MidSong *song = mid_song_load(stream, &options);

            if(song)
            {
                info.setValue(Qmmp::SAMPLERATE, helper->sampleRate());
                info.setValue(Qmmp::FORMAT_NAME, u"midi"_s);
                info.setDuration(mid_song_get_total_time(song));
                mid_song_free(song);
            }

            TiMidityHelper::instance()->removePtr(stream);
            mid_istream_close(stream);
        }
    }
    return { info };
}

MetaDataModel* DecoderTiMidityFactory::createMetaDataModel(const QString &path, bool readOnly)
{
    Q_UNUSED(path);
    Q_UNUSED(readOnly);
    return nullptr;
}

QDialog *DecoderTiMidityFactory::createSettings(QWidget *parent)
{
    return new TiMiditySettingsDialog(parent);
}

void DecoderTiMidityFactory::showAbout(QWidget *parent)
{
    QMessageBox::about(parent, tr("About TiMidity Audio Plugin"),
                       tr("Qmmp TiMidity Audio Plugin") + QChar::LineFeed +
                           tr("This plugin uses libTiMidity library to play midi files") + QChar::LineFeed +
                           tr("Compiled against libTiMidity-%1.%2.%3")
                                .arg(LIBTIMIDITY_VERSION_MAJOR)
                                .arg(LIBTIMIDITY_VERSION_MINOR)
                                .arg(LIBTIMIDITY_PATCHLEVEL) + QChar::LineFeed +
                       tr("Written by: Ilya Kotov <forkotov02@ya.ru>"));
}

QString DecoderTiMidityFactory::translation() const
{
    return QLatin1String(":/timidity_plugin_");
}
