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

WildMidiHelper *WildMidiHelper::m_instance = 0;

WildMidiHelper::WildMidiHelper(QObject *parent) :
    QObject(parent)
{
    m_inited = false;
    m_sample_rate = 0;
    m_instance = this;
}

WildMidiHelper::~WildMidiHelper()
{
    if(m_inited)
        WildMidi_Shutdown();
    m_instance = 0;
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
    settings.beginGroup("Midi");
    unsigned short int mixer_options = 0;
    QString conf_path = configFiles().isEmpty() ? QString() : configFiles().first();
    conf_path = settings.value("conf_path", conf_path).toString();
    if(conf_path.isEmpty() || !QFile::exists(conf_path))
    {
        qWarning("WildMidiHelper: invalid config path: %s", qPrintable(conf_path));
        m_mutex.unlock();
        return false;
    }

    if(!validateConfigFile(configPath))
    {
        qWarning("WildMidiHelper: malformed wildmidi config: %s", qPrintable(configPath));
        m_mutex.unlock();
        return false;
    }

    unsigned short int sample_rate = settings.value("sample_rate", 44100).toInt();
    if(settings.value("enhanced_resampling", false).toBool())
        mixer_options |= WM_MO_ENHANCED_RESAMPLING;
    if(settings.value("reverberation", false).toBool())
        mixer_options |= WM_MO_REVERB;
    settings.endGroup();

    m_sample_rate = sample_rate;
    if (WildMidi_Init (qPrintable(conf_path), sample_rate, mixer_options) < 0)
    {
        qWarning("WildMidiHelper: unable to initialize WildMidi library");
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
    QStringList files = QStringList() << "/etc/wildmidi/wildmidi.cfg"
                                      << "/etc/timidity/freepats.cfg"
                                      << "/etc/timidity.cfg"
                                      << "/etc/timidity/timidity.cfg";
    foreach(QString path, files)
    {
        if(!QFile::exists(path))
            files.removeAll(path);
    }
    return files;
}

bool WildMidiHelper::validateConfigFile(const QString &path) const
{
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly))
    {
        qWarning("WildMidiHelper: unable to wildmidi file; error: %s", qPrintable(file.errorString()));
        return false;
    }

    //check 'dir' option only
    while(!file.atEnd())
    {
        QString line = QString::fromUtf8(file.readLine()).trimmed();

        if(line.startsWith("dir"))
        {
            QStringList args = line.split(QChar::Space, QString::SkipEmptyParts);
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
