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

/* Based on Modplug XMMS Plugin
 * Authors: Kenton Varda <temporal@gauge3d.org>
 */

#include <QObject>
#include <QIODevice>
#include <QFile>
#include <QDir>
#include <QSettings>
#include <math.h>
#include <libmodplug/stdafx.h>
#include <libmodplug/it_defs.h>
#include <libmodplug/sndfile.h>
#include <qmmp/buffer.h>
#include <qmmp/output.h>
#include <stdint.h>
#include "archivereader.h"
#include "decoder_modplug.h"

// Decoder class

DecoderModPlug* DecoderModPlug::m_instance = nullptr;

DecoderModPlug::DecoderModPlug(const QString &path) : Decoder(nullptr),
    m_path(path)
{
    m_instance = this;  
}

DecoderModPlug::~DecoderModPlug()
{
    deinit();
    if(m_instance == this)
        m_instance = nullptr;
}

bool DecoderModPlug::initialize()
{
    m_freq = m_bitrate = 0;
    m_chan = 0;
    m_totalTime = 0;

    ArchiveReader reader;
    if (reader.isSupported(m_path))
        m_input_buf = reader.unpack(m_path);
    else
    {
        QFile file(m_path);
        if (!file.open(QIODevice::ReadOnly))
        {
            qCWarning(plugin) << "error:" << file.errorString();
            return false;
        }
        m_input_buf = file.readAll();
        file.close();
    }
    if (m_input_buf.isEmpty())
    {
        qCWarning(plugin) << "error while reading module file";
        return false;
    }
    m_soundFile = new CSoundFile();
    readSettings();
    m_sampleSize = m_bps / 8 * m_chan;
    m_soundFile->Create((uchar*) m_input_buf.data(), m_input_buf.size());
    m_bitrate = m_soundFile->GetNumChannels();
    m_totalTime = (qint64) m_soundFile->GetSongTime() * 1000;
    configure(m_freq, m_chan, (m_bps == 8 ? Qmmp::PCM_S8 : Qmmp::PCM_S16LE));
    return true;
}

qint64 DecoderModPlug::totalTime() const
{
    return m_totalTime;
}

int DecoderModPlug::bitrate() const
{
    return m_bitrate;
}

qint64 DecoderModPlug::read(unsigned char *audio, qint64 maxSize)
{
    long len = m_soundFile->Read (audio, maxSize) * m_sampleSize;
    if (m_usePreamp)
    {
        {
            //apply preamp
            if (m_bps == 16)
            {
                long n = len >> 1;
                for (long i = 0; i < n; i++)
                {
                    short old = ((short*)audio)[i];
                    ((short*)audio)[i] *= m_preampFactor;
                    // detect overflow and clip!
                    if ((old & 0x8000) !=
                            (((short*)audio)[i] & 0x8000))
                        ((short*)audio)[i] = old | 0x7FFF;
                }
            }
            else
            {
                for (long i = 0; i < len; i++)
                {
                    uchar old = ((uchar*)audio)[i];
                    ((uchar*)audio)[i] *= m_preampFactor;
                    // detect overflow and clip!
                    if ((old & 0x80) !=
                            (((uchar*)audio)[i] & 0x80))
                        ((uchar*)audio)[i] = old | 0x7F;
                }
            }
        }
    }
    return len;
}

void DecoderModPlug::seek(qint64 pos)
{
    quint32 lMax;
    quint32 lMaxtime;
    double lPostime;

    if (pos > (lMaxtime = m_soundFile->GetSongTime()) * 1000)
        pos = lMaxtime * 1000;
    lMax = m_soundFile->GetMaxPosition();
    lPostime = float(lMax) / lMaxtime;
    m_soundFile->SetCurrentPos(int(pos * lPostime / 1000));
}

void DecoderModPlug::deinit()
{
    m_freq = m_bitrate = 0;
    m_chan = 0;
    if (m_soundFile)
    {
        m_soundFile->Destroy();
        delete m_soundFile;
        m_soundFile = nullptr;
    }
    m_input_buf.clear();
}

void DecoderModPlug::readSettings()
{
    if (!m_soundFile)
        return;
    QSettings settings;
    settings.beginGroup("ModPlug"_L1);
    CSoundFile::SetWaveConfig
    (
        m_freq = settings.value("Frequency"_L1, 44100).toInt(),
        m_bps = settings.value("Bits"_L1, 16).toInt(),
        m_chan = settings.value("Channels"_L1, 2).toInt()
    );

    CSoundFile::SetWaveConfigEx
    (
        settings.value("Surround"_L1, true).toBool(),
        true,
        settings.value("Reverb"_L1, false).toBool(),
        true,
        settings.value("Megabass"_L1, false).toBool(),
        settings.value("NoiseReduction"_L1, false).toBool(),
        false
    );
    if (settings.value("Reverb"_L1, false).toBool())
    {
        CSoundFile::SetReverbParameters
        (
            settings.value("ReverbDepth"_L1, 30).toInt(),
            settings.value("ReverbDelay"_L1, 100).toInt()
        );
    }
    if (settings.value("Megabass"_L1, false).toBool())
    {
        CSoundFile::SetXBassParameters
        (
            settings.value("BassAmount"_L1, 40).toInt(),
            settings.value("BassRange"_L1, 30).toInt()
        );
    }
    if (settings.value("Surround"_L1, true).toBool())
    {
        CSoundFile::SetSurroundParameters
        (
            settings.value("SurroundDepth"_L1, 20).toInt(),
            settings.value("SurroundDelay"_L1, 20).toInt()
        );
    }
    CSoundFile::SetResamplingMode(settings.value("ResamplineMode"_L1, SRCMODE_POLYPHASE).toInt());
    m_soundFile->SetRepeatCount(settings.value("LoopCount"_L1, 0).toInt());


    //general
    /*
     settings.value("GrabAmigaMOD", true).toBool());*/
    //preamp
    m_usePreamp = settings.value("PreAmp"_L1, false).toBool();
    m_preampFactor = exp(settings.value("PreAmpLevel"_L1, 0.0f).toDouble());
    settings.endGroup();
}

DecoderModPlug* DecoderModPlug::instance()
{
    return m_instance;
}
