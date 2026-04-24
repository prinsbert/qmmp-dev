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

#include "timidityhelper.h"
#include "decoder_timidity.h"

// Decoder class
DecoderTiMidity::DecoderTiMidity(QIODevice *input) : Decoder(input)
{}

DecoderTiMidity::~DecoderTiMidity()
{
    if(m_stream)
    {
        TiMidityHelper::instance()->removePtr(m_stream);
        mid_song_free(m_song);
        mid_istream_close(m_stream);
    }
}

bool DecoderTiMidity::initialize()
{
    m_totalTime = 0;

    if(!TiMidityHelper::instance()->initialize())
    {
        qCWarning(plugin, "initialization failed");
        return false;
    }
    TiMidityHelper::instance()->readSettings();

    m_buffer = input()->readAll();
    if(m_buffer.isEmpty())
    {
        qCWarning(plugin, "unable to read file");
        return false;
    }

    if(!(m_stream = mid_istream_open_mem((void *)(m_buffer.constData()), m_buffer.size())))
    {
        qCWarning(plugin, "unable to open file");
        return false;
    }
    TiMidityHelper::instance()->addPtr(m_stream);

    MidSongOptions options;
    options.rate = TiMidityHelper::instance()->sampleRate();
    options.format = MID_AUDIO_S16LSB;
    options.channels = 2;
    options.buffer_size = options.rate * options.channels * 2; //2 seconds

    if(!(m_song = mid_song_load(m_stream, &options)))
    {
        qCWarning(plugin, "invalid MIDI file");
        return false;
    }

    mid_song_start(m_song);
    m_sample_rate = options.rate;
    m_totalTime = mid_song_get_total_time(m_song);
    configure(m_sample_rate, options.channels, Qmmp::PCM_S16LE);
    qCDebug(plugin, "initialize succes");
    return true;
}

qint64 DecoderTiMidity::totalTime() const
{
    return m_totalTime;
}

void DecoderTiMidity::seek(qint64 pos)
{
    mid_song_seek(m_song, pos);
}

int DecoderTiMidity::bitrate() const
{
    return 8;
}

qint64 DecoderTiMidity::read(unsigned char *data, qint64 size)
{
    return mid_song_read_wave(m_song, (sint8 *)data, size);
}
