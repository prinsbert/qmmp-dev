/***************************************************************************
 *   Copyright (C) 2008-2024 by Ilya Kotov                                 *
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

#include <QDir>
#include <QList>
#include <QFileInfo>
#include <QProcess>
#include <QByteArray>
#include <QApplication>
#include <QFile>
#include <QSet>
#include <algorithm>
#include <qmmp/qmmp.h>
#include "skinreader.h"

SkinReader::SkinReader(QObject *parent)
        : QObject(parent)
{
    //create cache dir
    QDir dir(Qmmp::cacheDir());
    dir.mkdir(u"skinned"_s);
    dir.cd(u"skinned"_s);
    dir.mkdir(u"thumbs"_s);
    dir.mkdir(u"skin"_s);
}

SkinReader::~SkinReader()
{}

void SkinReader::loadSkins(const QStringList &paths)
{
    m_skins.clear();
    m_previewHash.clear();
    QFileInfoList infoList;
    //find all file and directories
    for(const QString &path : qAsConst(paths))
    {
        QDir dir(path);
        dir.setSorting(QDir::Name);
        infoList << dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::Dirs | QDir::NoDotAndDotDot);
    }

    //find skins and generate thumbnailes
    QDir cacheDir(Qmmp::cacheDir() + QStringLiteral("/skinned/thumbs"));
    QHash<QString, QString> thumbnailHash; //base name, full path
    QFileInfoList thumbnailes = cacheDir.entryInfoList(QDir::Files | QDir::Hidden);

    for(const QFileInfo &i : qAsConst(thumbnailes))
        thumbnailHash.insert(i.baseName(), i.canonicalFilePath());

    for(const QFileInfo &info : qAsConst(infoList))
    {
        if(info.isDir())
        {
            QDir dir(info.canonicalFilePath());
            const QFileInfoList files = dir.entryInfoList({ QStringLiteral("main.*") }, QDir::Files | QDir::Hidden);
            if(!files.isEmpty())
            {
                m_skins << info.canonicalFilePath();
                m_previewHash.insert(info.canonicalFilePath(), files.constFirst().canonicalFilePath());
            }
        }
        else if(info.isFile())
        {
            QString thumbnailPath = thumbnailHash.value(info.baseName());

            if(thumbnailPath.isEmpty())
            {
                QString name = info.fileName().toLower();

                if(name.endsWith(u".tgz"_s) || name.endsWith(u".tar.gz"_s) || name.endsWith(u".tar.bz2"_s))
                {
                    untar(info.filePath(), cacheDir.absolutePath(), true);
                    m_skins << info.canonicalFilePath();
                    m_previewHash.insert(info.canonicalFilePath(), QString());
                }
                else if(name.endsWith(u".zip"_s) || name.endsWith(u".wsz"_s))
                {
                    unzip(info.filePath(), cacheDir.absolutePath(), true);
                    m_skins << info.canonicalFilePath();
                    m_previewHash.insert(info.canonicalFilePath(), QString());
                }
            }
            else
            {
                m_skins << info.canonicalFilePath();
                m_previewHash.insert(info.canonicalFilePath(), thumbnailPath);
            }
        }
    }


    //add new skins to cache
    cacheDir.refresh();
    thumbnailHash.clear();
    thumbnailes = cacheDir.entryInfoList(QDir::Files | QDir::Hidden);

    for(const QFileInfo &i : qAsConst(thumbnailes))
        thumbnailHash.insert(i.baseName(), i.canonicalFilePath());

    for(const QFileInfo &info : qAsConst(infoList))
    {
        if(info.isDir())
            continue;

        if(m_previewHash.contains(info.canonicalFilePath()) && m_previewHash.value(info.canonicalFilePath()).isEmpty())
        {
            QString thumbnailPath = thumbnailHash.value(info.baseName());
            if(!thumbnailPath.isEmpty())
            {
                qCDebug(plugin, "adding %s to cache", qPrintable(info.fileName()));
                m_previewHash.insert(info.canonicalFilePath(), thumbnailPath);
            }
        }
    }

    //clear removed skins from cache
    const QSet<QString> usedThumbnails(m_previewHash.constBegin(), m_previewHash.constEnd());
    for(const QFileInfo &i : qAsConst(thumbnailes))
    {
        if(!usedThumbnails.contains(i.canonicalFilePath()))
        {
            QFile::remove(i.canonicalFilePath());
            qCDebug(plugin, "deleting %s from cache", qPrintable(i.fileName()));
        }
    }
}

const QStringList &SkinReader::skins() const
{
    return m_skins;
}

const QPixmap SkinReader::getPreview(const QString &skinPath) const
{
    return QPixmap(m_previewHash.value(skinPath));
}

void SkinReader::unpackSkin(const QString &path)
{
    //remove old skin
    QDir dir(unpackedSkinPath());
    dir.setFilter(QDir::Files | QDir::Hidden);
    const QFileInfoList f = dir.entryInfoList();
    for(const QFileInfo &file : qAsConst(f))
        dir.remove(file.fileName());
    //create skin cache directory
    if(!QFile::exists(unpackedSkinPath()))
        QDir::root().mkpath(unpackedSkinPath());
    //unpack
    QString name = QFileInfo(path).fileName().toLower();
    if (name.endsWith(u".tgz"_s) || name.endsWith(u".tar.gz"_s) || name.endsWith(u".tar.bz2"_s))
        untar(path, unpackedSkinPath(), false);
    else if (name.endsWith(u".zip"_s) || name.endsWith(u".wsz"_s))
        unzip(path, unpackedSkinPath(), false);
}

QPixmap SkinReader::getPixmapFromDirectory(const QString &name, const QString &path)
{
    QDir dir(path);
    QFileInfoList f = dir.entryInfoList({ name + QStringLiteral(".*") }, QDir::Files | QDir::Hidden);
    if(!f.isEmpty())
        return QPixmap(f.constFirst().filePath());
    return QPixmap();
}

QString SkinReader::unpackedSkinPath()
{
    return Qmmp::cacheDir() + QStringLiteral("/skinned/skin");
}

QString SkinReader::defaultSkinPath()
{
    return QStringLiteral(":/glare");
}

void SkinReader::untar(const QString &from, const QString &to, bool preview)
{
    QProcess process;
    process.start(u"tar"_s, { u"tf"_s, from }); //list archive
    process.waitForFinished();
    QByteArray array = process.readAllStandardOutput ();
    const QStringList outputList = QString::fromLocal8Bit(array).split(QChar::LineFeed, Qt::SkipEmptyParts);

    for(QString str : qAsConst(outputList))
    {
        str = str.trimmed();

        if(str.endsWith(QLatin1Char('/')))
            continue;

        if (!preview || (str.contains(u"/main."_s, Qt::CaseInsensitive)
                         || str.startsWith(u"main."_s, Qt::CaseInsensitive)))
        {
            QStringList args = { u"xvfk"_s , from , u"-O"_s , str };
            process.start(u"tar"_s, args);
            process.waitForStarted();
            process.waitForFinished();
            array = process.readAllStandardOutput();

            QString name;
            if (preview)
                name = from.section(QLatin1Char('/'), -1) + QLatin1Char('.') + str.section(QLatin1Char('.'), -1);
            else
                name = str.contains(QLatin1Char('/')) ? str.section(QLatin1Char('/'), -1).toLower() : str.toLower();

            QFile file(to + QLatin1Char('/') + name);
            file.open(QIODevice::WriteOnly);
            file.write(array);
            file.close();
        }
    }
}

void SkinReader::unzip(const QString &from, const QString &to, bool preview)
{
    if (preview)
    {
        QStringList args = { u"-C"_s, u"-j"_s, u"-o"_s, u"-qq"_s, u"-d"_s, to, from, u"main.???"_s, u"*/main.???"_s };
        QProcess::execute(u"unzip"_s, args);
        QDir dir(to);
        dir.setFilter(QDir::Files | QDir::Hidden);
        const QFileInfoList fileList = dir.entryInfoList();
        for(const QFileInfo &thumbInfo : qAsConst(fileList))
        {
            if (thumbInfo.fileName().startsWith(u"main."_s, Qt::CaseInsensitive))
            {
                dir.rename(thumbInfo.fileName(), from.section(QLatin1Char('/'), -1) + QLatin1Char('.') + thumbInfo.suffix ());
            }
        }
    }
    else
    {
        QStringList args = { u"-j"_s, u"-o"_s, u"-qq"_s, u"-d"_s, to, from };
        QProcess::execute(u"unzip"_s, args);
    }
}
