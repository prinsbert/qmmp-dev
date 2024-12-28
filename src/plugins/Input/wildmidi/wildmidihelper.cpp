/***************************************************************************
 *   Copyright (C) 2010-2025 by Ilya Kotov                                 *
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

    QSettings settings;
    settings.beginGroup(u"Midi"_s);

    QStringList availableConfigFiles = configFiles();
    QString configPath = availableConfigFiles.isEmpty() ? QString() : availableConfigFiles.constFirst();
    configPath = settings.value(u"conf_path"_s, configPath).toString();
    if(configPath.isEmpty() || !QFile::exists(configPath))
    {
        qCWarning(plugin, "missing config file path: %s", qPrintable(configPath));
        m_mutex.unlock();
        return false;
    }

    if(!validateConfigFile(configPath))
    {
        qCWarning(plugin, "malformed wildmidi config: %s", qPrintable(configPath));
        m_mutex.unlock();
        return false;
    }

    unsigned short int mixer_options = 0;
    unsigned short int sample_rate = settings.value(u"sample_rate"_s, 44100).toInt();
    if(settings.value(u"enhanced_resampling"_s, false).toBool())
        mixer_options |= WM_MO_ENHANCED_RESAMPLING;
    if(settings.value(u"reverberation"_s, false).toBool())
        mixer_options |= WM_MO_REVERB;
    settings.endGroup();

    m_sample_rate = sample_rate;
    if(WildMidi_Init(qPrintable(configPath), sample_rate, mixer_options) < 0)
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
        u"/etc/wildmidi/wildmidi.cfg"_s,
        u"/etc/timidity/freepats.cfg"_s,
        u"/etc/timidity.cfg"_s,
        u"/etc/timidity/timidity.cfg"_s,
    };
    QStringList filtered;
    for(const QString &path : std::as_const(paths))
    {
        if(QFile::exists(path))
            filtered << path;
    }
    return filtered;
}

bool WildMidiHelper::validateConfigFile(const QString &path) const
{
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly))
    {
        qCWarning(plugin) << "unable to wildmidi file; error:" << file.errorString();
        return false;
    }

    //check 'dir' option only
    while(!file.atEnd())
    {
        QString line = QString::fromUtf8(file.readLine()).trimmed();

        if(line.startsWith(u"dir"_s))
        {
            QStringList args = line.split(QChar::Space, Qt::SkipEmptyParts);
            if (args.count() != 2)
                continue;

            //check 'dir' option
            if (QFile::exists(args.at(1)))
                return true;
        }
    }

    return false;
}

quint32 WildMidiHelper::sampleRate()
{
    return m_sample_rate;
}

WildMidiHelper *WildMidiHelper::instance()
{
    return m_instance;
}
