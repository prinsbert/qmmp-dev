/***************************************************************************
 *   Copyright (C) 2010-2026 by Ilya Kotov                                 *
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

#include <QCoreApplication>
#include <QSettings>
#include <QFile>
#include <qmmp/qmmp.h>
#include "timidityhelper.h"

TiMidityHelper *TiMidityHelper::m_instance = nullptr;

bool TiMidityHelper::initialize()
{
    m_mutex.lock();
    if(m_inited)
    {
        m_mutex.unlock();
        return true;
    }

    QSettings settings;
    settings.beginGroup(u"TiMidity"_s);

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
        qCWarning(plugin, "malformed timidity config: %s", qPrintable(configPath));
        m_mutex.unlock();
        return false;
    }

    qCDebug(plugin) << "config path:" << configPath;

    m_sampleRate = settings.value(u"sample_rate"_s, 44100).toInt();

    if(mid_init(qPrintable(configPath)) < 0)
    {
        qCWarning(plugin, "unable to initialize libTiMidity library");
        m_mutex.unlock();
        return false;
    }
    m_inited = true;
    m_mutex.unlock();
    return true;
}

void TiMidityHelper::readSettings()
{
    m_mutex.lock();
    if(!m_ptrs.isEmpty())
    {
        m_mutex.unlock();
        return;
    }
    if(m_inited)
        mid_exit();
    m_inited = false;
    m_mutex.unlock();
    initialize();
}

void TiMidityHelper::addPtr(MidIStream *stream)
{
    m_mutex.lock();
    m_ptrs.append(stream);
    m_mutex.unlock();
}

void TiMidityHelper::removePtr(MidIStream *stream)
{
    m_mutex.lock();
    m_ptrs.removeAll(stream);
    m_mutex.unlock();
}

QStringList TiMidityHelper::configFiles() const
{
    static const QStringList paths = {
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

bool TiMidityHelper::validateConfigFile(const QString &path) const
{
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly))
    {
        qCWarning(plugin) << "unable to open MIDI file; error:" << file.errorString();
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

quint32 TiMidityHelper::sampleRate() const
{
    return m_sampleRate;
}

TiMidityHelper *TiMidityHelper::instance()
{
    if(!m_instance)
    {
        m_instance = new TiMidityHelper;
        qAddPostRoutine(TiMidityHelper::destroy);
    }
    return m_instance;
}

TiMidityHelper::TiMidityHelper()
{}

TiMidityHelper::~TiMidityHelper()
{
    if(m_inited)
        mid_exit();
    m_instance = nullptr;
}

void TiMidityHelper::destroy()
{
    if(m_instance)
    {
        delete m_instance;
        m_instance = nullptr;
    }
}
