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

#include <QProcess>
#include <QSet>
#include <qmmp/qmmp.h>
#include "archivereader.h"

ArchiveReader::ArchiveReader(QObject *parent)
        : QObject(parent)
{
    m_process = new QProcess(this);
}


ArchiveReader::~ArchiveReader()
{}

bool ArchiveReader::isSupported(const QString &path) const
{
    static const QSet<QString> supportedExts = {
        u".mdz"_s, u".s3z"_s, u".xmz"_s, u".itz"_s, u".mdgz"_s, u".s3gz"_s, u".xmgz"_s,
        u".itgz"_s, u".mdbz"_s, u".s3bz"_s, u".xmbz"_s, u".itbz"_s,
    };

    return supportedExts.contains(path.toLower().section(QLatin1Char('.'), -1));
}

QByteArray ArchiveReader::unpack(const QString &path)
{
    QString lPath = path.toLower();
    if (path.endsWith(u".mdz"_s) ||
            lPath.endsWith(u".s3z"_s) ||
            lPath.endsWith(u".xmz"_s) ||
            lPath.endsWith(u".itz"_s))
        return unzip(path);

    if (lPath.endsWith(u".mdgz"_s) ||
             lPath.endsWith(u".s3gz"_s) ||
             lPath.endsWith(u".xmgz"_s) ||
             lPath.endsWith(u".itgz"_s))
        return gunzip(path);

    if (lPath.endsWith(u".mdbz"_s))
        return bunzip2(path);

    return QByteArray();
}

QByteArray ArchiveReader::unzip(const QString &path)
{
    QStringList args = { u"-p"_s, path };
    m_process->start(u"unzip"_s, args);
    m_process->waitForFinished();
    return m_process->readAllStandardOutput ();
}

QByteArray ArchiveReader::gunzip(const QString &path)
{
    QStringList args = { u"-c"_s, path };
    m_process->start(u"gunzip"_s, args);
    m_process->waitForFinished();
    return m_process->readAllStandardOutput ();
}

QByteArray ArchiveReader::bunzip2(const QString &path)
{
    QStringList args = { u"-c"_s, path };
    m_process->start(u"bunzip2"_s, args);
    m_process->waitForFinished();
    return m_process->readAllStandardOutput ();
}
