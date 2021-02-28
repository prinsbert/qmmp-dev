/***************************************************************************
 *   Copyright (C) 2019-2021 by Ilya Kotov                                 *
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

#include <QtDebug>
#include <QNetworkReply>
#include <QNetworkProxy>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QUrlQuery>
#include <qmmp/statehandler.h>
#include <qmmp/qmmpsettings.h>
#include "bufferdevice.h"
#include "ytbinputsource.h"

#define PREBUFFER_SIZE 128000

YtbInputSource::YtbInputSource(const QString &url, QObject *parent) : InputSource(url, parent), m_url(url)
{
    m_buffer = new BufferDevice(this);
    m_process = new QProcess(this);
    m_manager = new QNetworkAccessManager(this);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 9, 0))
    m_manager->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
#endif

    QmmpSettings *gs = QmmpSettings::instance();
    if (gs->isProxyEnabled())
    {
        QNetworkProxy proxy(QNetworkProxy::HttpProxy, gs->proxy().host(),  gs->proxy().port());
        if(gs->proxyType() == QmmpSettings::SOCKS5_PROXY)
            proxy.setType(QNetworkProxy::Socks5Proxy);
        if(gs->useProxyAuth())
        {
            proxy.setUser(gs->proxy().userName());
            proxy.setPassword(gs->proxy().password());
        }
        m_manager->setProxy(proxy);
    }
    else
        m_manager->setProxy(QNetworkProxy::NoProxy);

    connect(m_process, SIGNAL(errorOccurred(QProcess::ProcessError)), SLOT(onProcessErrorOccurred(QProcess::ProcessError)));
    connect(m_process, SIGNAL(finished(int,QProcess::ExitStatus)), SLOT(onProcessFinished(int,QProcess::ExitStatus)));
    connect(m_manager, SIGNAL(finished(QNetworkReply*)), SLOT(onFinished(QNetworkReply*)));
    connect(m_buffer, SIGNAL(seekRequest()), SLOT(onSeekRequest()));
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

QIODevice *YtbInputSource::ioDevice() const
{
    return m_buffer;
}

bool YtbInputSource::initialize()
{
    QString id;
    if(m_url.startsWith("ytb://"))
        id = m_url.section("://", -1);
    else if(m_url.startsWith("https://www.youtube.com/"))
        id = QUrlQuery(QUrl(m_url)).queryItemValue("v");
    else if(m_url.startsWith("https://youtu.be/"))
        id = QUrl(m_url).path().remove("/");

    QStringList args = { "--print-json", "-s", QString("https://www.youtube.com/watch?v=%1").arg(id) };

    m_ready = false;
    m_buffer->open(QIODevice::ReadOnly);
    m_process->start("youtube-dl", args);
    qDebug("YtbInputSource: starting youtube-dl...");
    return true;
}

bool YtbInputSource::isReady() const
{
    return m_ready;
}

bool YtbInputSource::isWaiting() const
{
    return m_buffer->isWaiting();
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
    {
        qWarning("YtbInputSource: youtube-dl finished with error:\n%s", m_process->readAllStandardError().constData());
        emit error();
        return;
    }

    QJsonDocument document = QJsonDocument::fromJson(m_process->readAllStandardOutput());
    if(document.isEmpty())
    {
        qWarning("YtbInputSource: unable to parse youtube-dl output");
        emit error();
        return;
    }

    QJsonObject json = document.object();

    //qDebug("%s", document.toJson(QJsonDocument::Indented).constData());

    QMap<Qmmp::MetaData, QString> metaData = {
        { Qmmp::TITLE, json["fulltitle"].toString() }
    };
    addMetaData(metaData);

    QHash<QString, QString> streamInfo = {
        { tr("Uploader"), json["uploader"].toString() },
        { tr("Upload date"), json["upload_date"].toString() },
        { tr("Duration"), QString("%1:%2").arg(json["duration"].toInt() / 60)
          .arg(json["duration"].toInt() % 60, 2, 10, QChar('0')) },
    };
    addStreamInfo(streamInfo);

    int bitrate = json["abr"].toInt();
    QString acodec = json["acodec"].toString();


    QString url;
    QJsonObject headers;
    for(const QJsonValue &value : json["formats"].toArray())
    {
        QJsonObject obj = value.toObject();
        qDebug() << obj["acodec"].toString() << obj["vcodec"].toString() << obj["abr"].toInt();

        if(obj["abr"].toInt() == bitrate &&
                obj["acodec"].toString() == acodec &&
                obj["vcodec"].toString() == "none")
        {
            url = obj["protocol"].toString() == "http_dash_segments" ?
                        obj["fragment_base_url"].toString() : obj["url"].toString();

            headers = obj["http_headers"].toObject();
            setProperty(Qmmp::BITRATE, bitrate);
            m_fileSize = obj["filesize"].toInt();
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
    QJsonObject::iterator it = headers.begin();
    while (it != headers.end())
    {
        request.setRawHeader(it.key().toLatin1(), it.value().toString().toLatin1());
        ++it;
    }

    if(m_offset > 0)
    {
        request.setRawHeader("Range", QString("bytes=%1-").arg(m_offset).toLatin1());
        request.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
        m_buffer->setOffset(m_offset);
    }

    m_buffer->setSize(m_fileSize);

    m_getStreamReply = m_manager->get(request);
    m_getStreamReply->setReadBufferSize(0);
    connect(m_getStreamReply, SIGNAL(downloadProgress(qint64, qint64)), SLOT(onDownloadProgress(qint64,qint64)));
}

void YtbInputSource::onFinished(QNetworkReply *reply)
{
    qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if(reply == m_getStreamReply)
    {
        if(reply->error() != QNetworkReply::NoError)
        {
            qWarning("YtbInputSource: downloading finished with error: %s", qPrintable(reply->errorString()));
            if(!m_ready)
            {
                m_getStreamReply = nullptr;
                reply->deleteLater();
                //emit error();
            }
        }
        else
        {
            m_buffer->addData(m_getStreamReply->readAll());
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
        m_buffer->open(QIODevice::ReadOnly);
        emit ready();
    }
    else if(!m_ready)
    {
        StateHandler::instance()->dispatchBuffer(100 * bytesReceived / PREBUFFER_SIZE);
    }

    qDebug("received %lld", m_getStreamReply->bytesAvailable());
    m_buffer->addData(m_getStreamReply->readAll());
}

void YtbInputSource::onSeekRequest()
{
    qDebug() << Q_FUNC_INFO <<  m_buffer->seekRequestPos();
    m_offset = m_buffer->seekRequestPos();
    m_buffer->clearRequestPos();

    QNetworkReply *tmp = m_getStreamReply;
    m_getStreamReply = nullptr;
    disconnect(tmp, nullptr, nullptr, nullptr);
    tmp->abort();


    QNetworkRequest request = tmp->request();
    tmp->deleteLater();
    request.setRawHeader("Range", QString("bytes=%1-").arg(m_offset).toLatin1());
    request.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
    m_buffer->setOffset(m_offset);
    m_getStreamReply = m_manager->get(request);
    m_getStreamReply->setReadBufferSize(0);
    connect(m_getStreamReply, SIGNAL(downloadProgress(qint64, qint64)), SLOT(onDownloadProgress(qint64,qint64)));
    //m_ready = false;
    //m_buffer->close();
    qDebug("seeking");
}
