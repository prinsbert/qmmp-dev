/***************************************************************************
 *   Copyright (C) 2009-2013 by Ilya Kotov                                 *
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

//#include "ytbstreamreader.h"
#include <QtDebug>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <qmmp/statehandler.h>
#include "ytbinputsource.h"

#define PREBUFFER_SIZE 128000

YtbInputSource::YtbInputSource(const QString &url, QObject *parent) : InputSource(url, parent)
{
    m_url = url;
    m_process = new QProcess(this);
    m_manager = new QNetworkAccessManager(this);

    connect(m_process, SIGNAL(errorOccurred(QProcess::ProcessError)), SLOT(onProcessErrorOccurred(QProcess::ProcessError)));
    connect(m_process, SIGNAL(finished(int,QProcess::ExitStatus)), SLOT(onProcessFinished(int,QProcess::ExitStatus)));
    connect(m_manager, SIGNAL(finished(QNetworkReply*)), SLOT(onFinished(QNetworkReply*)));
}

YtbInputSource::~YtbInputSource()
{
    if(m_getStreamReply)
    {
        if(m_getStreamReply->isFinished())
            m_getStreamReply->abort();
        m_getStreamReply->deleteLater();
        m_getStreamReply = nullptr;
    }
}

QIODevice *YtbInputSource::ioDevice()
{
    return m_getStreamReply;
}

bool YtbInputSource::initialize()
{
    QString id = m_url.section("://", -1);
    QString cmd = QString("youtube-dl --print-json -s https://www.youtube.com/watch?v=%1").arg(id);

    m_ready = false;
    m_process->start(cmd);
    qDebug("YtbInputSource: starting youtube-dl...");
    return true;
}

bool YtbInputSource::isReady()
{
    return m_ready;
}

bool YtbInputSource::isWaiting()
{
    return false;
}

QString YtbInputSource::contentType() const
{
    return QString();
    //return m_reader->contentType();
}

void YtbInputSource::onProcessErrorOccurred(QProcess::ProcessError)
{
    qWarning("YtbInputSource: unable to start process 'youtube-dl', error: %s", qPrintable(m_process->errorString()));
    emit error();
}

void YtbInputSource::onProcessFinished(int exitCode, QProcess::ExitStatus status)
{
    if(exitCode != EXIT_SUCCESS || status != QProcess::NormalExit)
        return;

    QJsonDocument json = QJsonDocument::fromJson(m_process->readAllStandardOutput());
    if(json.isEmpty())
    {
        qWarning("YtbInputSource: unable to parse youtube-dl output");
        emit error();
        return;
    }

    QMap<Qmmp::MetaData, QString> metaData = {
        { Qmmp::TITLE, json["fulltitle"].toString() }
    };
    addMetaData(metaData);

    QString url;
    for(const QJsonValue &value : json["formats"].toArray())
    {
        if(value["ext"].toString() == "m4a")
        {
            url = value["url"].toString();
            break;
        }
    }

    if(url.isEmpty())
    {
        qWarning("YtbInputSource: unable to find stream");
        emit error();
        return;
    }

    qDebug("YtbInputSource: downloading stream...");

    QUrl streamUrl(url);
    QNetworkRequest request(streamUrl);
    request.setRawHeader("Host", streamUrl.host().toLatin1());
    request.setRawHeader("Accept", "*/*");
    m_getStreamReply = m_manager->get(request);
    m_getStreamReply->setReadBufferSize(0);
    connect(m_getStreamReply, SIGNAL(downloadProgress(qint64, qint64)), SLOT(onDownloadProgress(qint64,qint64)));
}

void YtbInputSource::onFinished(QNetworkReply *reply)
{
    if(reply == m_getStreamReply)
    {
        if(reply->error() != QNetworkReply::NoError)
        {
            qWarning("YtbInputSource: downloading finished with error: %s", qPrintable(reply->errorString()));
            if(!m_ready)
            {
                m_getStreamReply = nullptr;
                reply->deleteLater();
                emit error();
            }
        }
        else
        {
            qDebug("YtbInputSource: downloading finished");
        }
    }
    else
    {
        reply->deleteLater();
    }
}

void YtbInputSource::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    Q_UNUSED(bytesTotal);

    if(!m_ready && bytesReceived > PREBUFFER_SIZE)
    {
        qDebug("YtbInputSource: ready");
        m_ready = true;
        emit ready();
    }
    else if(!m_ready)
    {
        StateHandler::instance()->dispatchBuffer(100 * bytesReceived / PREBUFFER_SIZE);
    }
}
