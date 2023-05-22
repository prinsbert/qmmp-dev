/***************************************************************************
 *   Copyright (C) 2008-2022 by Ilya Kotov                                 *
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
#include <QtDebug>
#include <QSet>
#include <algorithm>
#include <qmmp/qmmp.h>
#include "skinreader.h"

SkinReader::SkinReader(QObject *parent)
        : QObject(parent)
{
    //create cache dir
    QDir dir(Qmmp::cacheDir());
    dir.mkdir("skinned");
    dir.cd("skinned");
    dir.mkdir("thumbs");
    dir.mkdir("skin");
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

                if(name.endsWith(".tgz") || name.endsWith(".tar.gz") || name.endsWith(".tar.bz2"))
                {
                    untar(info.filePath(), cacheDir.absolutePath(), true);
                    m_skins << info.canonicalFilePath();
                    m_previewHash.insert(info.canonicalFilePath(), QString());
                }
                else if(name.endsWith(".zip") || name.endsWith(".wsz"))
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
                qDebug("SkinReader: adding %s to cache", qPrintable(info.fileName()));
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
            qDebug("SkinReader: deleting %s from cache", qPrintable(i.fileName()));
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
    if (name.endsWith(".tgz") || name.endsWith(".tar.gz") || name.endsWith(".tar.bz2"))
        untar(path, unpackedSkinPath(), false);
    else if (name.endsWith(".zip") || name.endsWith(".wsz"))
        unzip(path, unpackedSkinPath(), false);
}

QString SkinReader::unpackedSkinPath()
{
    return Qmmp::cacheDir() + QStringLiteral("/skinned/skin");
}

void SkinReader::untar(const QString &from, const QString &to, bool preview)
{
    QProcess process;
    process.start("tar", { "tf", from }); //list archive
    process.waitForFinished();
    QByteArray array = process.readAllStandardOutput ();
    const QStringList outputList = QString(array).split("\n", Qt::SkipEmptyParts);

    for(QString str : qAsConst(outputList))
    {
        str = str.trimmed();

        if(str.endsWith("/"))
            continue;

        if (!preview || (str.contains("/main.", Qt::CaseInsensitive)
                         || str.startsWith("main.", Qt::CaseInsensitive)))
        {
            QStringList args = { "xvfk" , from , "-O" , str };
            process.start("tar", args);
            process.waitForStarted();
            process.waitForFinished();
            array = process.readAllStandardOutput();

            QString name;
            if (preview)
                name = from.section('/',-1) + (".") + str.section('.', -1);
            else
                name = str.contains('/') ? str.section('/',-1).toLower() : str.toLower();

            QFile file(to+"/"+name);
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
        QStringList args = { "-C", "-j", "-o", "-qq", "-d", to, from, "main.???", "*/main.???" };
        QProcess::execute("unzip", args);
        QDir dir(to);
        dir.setFilter(QDir::Files | QDir::Hidden);
        const QFileInfoList fileList = dir.entryInfoList();
        for(const QFileInfo &thumbInfo : qAsConst(fileList))
        {
            if (thumbInfo.fileName().startsWith("main.", Qt::CaseInsensitive))
            {
                dir.rename(thumbInfo.fileName(), from.section('/', -1) +
                           "." + thumbInfo.suffix ());
            }
        }
    }
    else
    {
        QStringList args = { "-j", "-o", "-qq", "-d", to, from };
        QProcess::execute("unzip", args);
    }
}
