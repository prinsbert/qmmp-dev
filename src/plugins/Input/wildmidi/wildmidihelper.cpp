/***************************************************************************
 *   Copyright (C) 2010-2024 by Ilya Kotov                                 *
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
extern "C"{
#include <wildmidi_lib.h>
}
#include <qmmp/qmmp.h>
#include "wildmidihelper.h"

WildMidiHelper *WildMidiHelper::m_instance = nullptr;

WildMidiHelper::WildMidiHelper(QObject *parent) :
    QObject(parent)
{
    m_instance = this;
}

WildMidiHelper::~WildMidiHelper()
{
    if(m_inited)
        WildMidi_Shutdown();
    m_instance = nullptr;
}

bool WildMidiHelper::initialize()
{
    m_mutex.lock();
    if(m_inited)
    {
        m_mutex.unlock();
        return true;
    }

    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup(u"Midi"_s);
    unsigned short int mixer_options = 0;
    QString conf_path = configFiles().isEmpty() ? QString() : configFiles().constFirst();
    conf_path = settings.value(u"conf_path"_s, conf_path).toString();
    if(conf_path.isEmpty() || !QFile::exists(conf_path))
    {
        qCWarning(plugin, "invalid config path: %s", qPrintable(conf_path));
        m_mutex.unlock();
        return false;
    }
    unsigned short int sample_rate = settings.value(u"sample_rate"_s, 44100).toInt();
    if(settings.value(u"enhanced_resampling"_s, false).toBool())
        mixer_options |= WM_MO_ENHANCED_RESAMPLING;
    if(settings.value(u"reverberation"_s, false).toBool())
        mixer_options |= WM_MO_REVERB;
    settings.endGroup();

    m_sample_rate = sample_rate;
    if(WildMidi_Init(qPrintable(conf_path), sample_rate, mixer_options) < 0)
    {
        qCWarning(plugin, "unable to initialize WildMidi library");
        m_mutex.unlock();
        return false;
    }
    m_inited = true;
    m_mutex.unlock();
    return true;
}

void WildMidiHelper::readSettings()
{
    m_mutex.lock();
    if(!m_ptrs.isEmpty())
    {
        m_mutex.unlock();
        return;
    }
    if(m_inited)
        WildMidi_Shutdown();
    m_inited = false;
    m_mutex.unlock();
    initialize();
}

void WildMidiHelper::addPtr(void *t)
{
    m_mutex.lock();
    m_ptrs.append(t);
    m_mutex.unlock();
}

void WildMidiHelper::removePtr(void *t)
{
    m_mutex.lock();
    m_ptrs.removeAll(t);
    m_mutex.unlock();
}

QStringList WildMidiHelper::configFiles() const
{
    static const QStringList paths = {
        u"/etc/timidity.cfg"_s,
        u"/etc/timidity/timidity.cfg"_s,
        u"/etc/wildmidi/wildmidi.cfg"_s
    };
    QStringList filtered;
    for(const QString &path : std::as_const(paths))
    {
        if(QFile::exists(path))
            filtered << path;
    }
    return filtered;
}

quint32 WildMidiHelper::sampleRate()
{
    return m_sample_rate;
}

WildMidiHelper *WildMidiHelper::instance()
{
    return m_instance;
}
