/***************************************************************************
 *   Copyright (C) 2019-2025 by Ilya Kotov                                 *
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
#include <unistd.h>
#include <stdlib.h>
#include <qmmp/statehandler.h>
#include <qmmp/qmmpsettings.h>
#include "bufferdevice.h"
#include "ytbinputsource.h"

YtbInputSource::YtbInputSource(const QString &url, QObject *parent) : InputSource(url, parent), m_url(url)
{
    m_buffer = new BufferDevice(this);
    m_process = new QProcess(this);
    m_manager = new QNetworkAccessManager(this);
    m_manager->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);

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

    connect(m_process, &QProcess::errorOccurred, this, &YtbInputSource::onProcessErrorOccurred);
    connect(m_process, qOverload<int, QProcess::ExitStatus>(&QProcess::finished), this, &YtbInputSource::onProcessFinished);
    connect(m_manager, &QNetworkAccessManager::finished, this, &YtbInputSource::onFinished);
    connect(m_buffer, &BufferDevice::seekRequest, this, &YtbInputSource::onSeekRequest);
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

QString YtbInputSource::findBackend(QString *version)
{
    static const QStringList backends = { u"yt-dlp"_s, u"youtube-dl"_s };

    for(const QString &backend : std::as_const(backends))
    {
        QProcess p;
        p.start(backend, { u"--version"_s });
        p.waitForFinished();
        if(p.exitCode() == EXIT_SUCCESS)
        {
            if(version)
                *version = QString::fromLatin1(p.readAll()).trimmed();
            return backend;
        }
    }

    return QString();
}

QIODevice *YtbInputSource::ioDevice() const
{
    return m_buffer;
}

bool YtbInputSource::initialize()
{
    m_backend = findBackend();
    if(m_backend.isEmpty())
    {
        qCWarning(plugin) << "unable to find backend";
        return false;
    }

    qCWarning(plugin) << "using" << m_backend;

    QString id;
    if(m_url.startsWith(u"ytb://"_s))
        id = m_url.section(u"://"_s, -1);
    else if(m_url.startsWith(u"https://www.youtube.com/"_s))
        id = QUrlQuery(QUrl(m_url)).queryItemValue(u"v"_s);
    else if(m_url.startsWith(u"https://youtu.be/"_s))
        id = QUrl(m_url).path().remove(QLatin1Char('/'));

    QStringList args = { u"-j"_s, QStringLiteral("https://www.youtube.com/watch?v=%1").arg(id) };

    if(QmmpSettings::instance()->isProxyEnabled())
        args << QStringLiteral("--proxy") << QmmpSettings::instance()->proxy().toString();

    m_ready = false;
    m_buffer->open(QIODevice::ReadOnly);
    m_process->start(m_backend, args);
    qCDebug(plugin) << "starting" << m_backend << "...";
    return true;
}

bool YtbInputSource::isReady() const
{
    return m_ready;
}

bool YtbInputSource::isWaiting() const
{
    return m_getStreamReply && !m_buffer->hasEnougthData();
}

QString YtbInputSource::contentType() const
{
    return QString();
    //return m_reader->contentType();
}

void YtbInputSource::stop()
{
    m_buffer->stop();
}

void YtbInputSource::onProcessErrorOccurred(QProcess::ProcessError)
{
    qCWarning(plugin, "unable to start process '%s', error: %s", qPrintable(m_backend), qPrintable(m_process->errorString()));
    emit error();
}

void YtbInputSource::onProcessFinished(int exitCode, QProcess::ExitStatus status)
{
    if(exitCode != EXIT_SUCCESS || status != QProcess::NormalExit)
    {
        qCWarning(plugin, "%s finished with error:\n%s", qPrintable(m_backend), m_process->readAllStandardError().constData());
        emit error();
        return;
    }

    QJsonDocument document = QJsonDocument::fromJson(m_process->readAllStandardOutput());
    if(document.isEmpty())
    {
        qCWarning(plugin, "unable to parse %s output", qPrintable(m_backend));
        emit error();
        return;
    }

    QJsonObject json = document.object();

    //qDebug("%s", document.toJson(QJsonDocument::Indented).constData());

    QMap<Qmmp::MetaData, QString> metaData = {
        { Qmmp::TITLE, json[u"fulltitle"_s].toString() }
    };
    addMetaData(metaData);

    QHash<QString, QString> streamInfo = {
        { tr("Uploader"), json[u"uploader"_s].toString() },
        { tr("Upload date"), json[u"upload_date"_s].toString() },
        { tr("Duration"), QStringLiteral("%1:%2").arg(json[u"duration"_s].toInt() / 60)
          .arg(json[u"duration"_s].toInt() % 60, 2, 10, QLatin1Char('0')) },
    };
    addStreamInfo(streamInfo);

    double bitrate = 0;
    QString url, codec;
    QJsonObject headers;
    for(const QJsonValue &value : json[u"formats"_s].toArray())
    {
        QJsonObject obj = value.toObject();

        qCDebug(plugin) << obj[u"acodec"_s].toString() << obj[u"vcodec"_s].toString() << obj[u"abr"_s].toDouble();

        if(obj[u"abr"_s].toDouble() > bitrate && obj[u"acodec"_s].toString() == "opus"_L1 &&
                obj[u"vcodec"_s].toString() == "none"_L1)
        {
            url = obj.contains(u"fragment_base_url"_s) ? obj[u"fragment_base_url"_s].toString() : obj[u"url"_s].toString();
            bitrate = obj[u"abr"_s].toDouble();
            headers = obj[u"http_headers"_s].toObject();
            codec = obj[u"acodec"_s].toString();
            m_fileSize = obj[u"filesize"_s].toInt();
        }
    }

    //fallback
    if(url.isEmpty())
    {
        for(const QJsonValue &value : json[u"formats"_s].toArray())
        {
            QJsonObject obj = value.toObject();

            if(obj[u"abr"_s].toDouble() > bitrate && obj[u"vcodec"_s].toString() == "none"_L1)
            {
                url = obj.contains(u"fragment_base_url"_s) ? obj[u"fragment_base_url"_s].toString() : obj[u"url"_s].toString();
                bitrate = obj[u"abr"_s].toDouble();
                headers = obj[u"http_headers"_s].toObject();
                codec = obj[u"acodec"_s].toString();
                m_fileSize = obj[u"filesize"_s].toInt();
            }
        }
    }

    if(url.isEmpty())
    {
        qCWarning(plugin) << "unable to find stream";
        emit error();
        return;
    }

    setProperty(Qmmp::BITRATE, int(bitrate));
    setProperty(Qmmp::FILE_SIZE, m_fileSize);
    setProperty(Qmmp::FORMAT_NAME, codec);

    qCDebug(plugin) << "selected stream:" << codec << bitrate << "kb/s";
    qCDebug(plugin) << "downloading stream...";

    QUrl streamUrl(url);
    m_request.setUrl(streamUrl);
    QJsonObject::const_iterator it = headers.begin();
    while (it != headers.end())
    {
        m_request.setRawHeader(it.key().toLatin1(), it.value().toString().toLatin1());
        ++it;
    }

    if(m_offset > 0)
    {
        m_request.setRawHeader("Range", QStringLiteral("bytes=%1-").arg(m_offset).toLatin1());
        m_request.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
        m_buffer->setOffset(m_offset);
    }

    m_buffer->setSize(m_fileSize);

    m_getStreamReply = m_manager->get(m_request);
    m_getStreamReply->setReadBufferSize(0);
    connect(m_getStreamReply, &QNetworkReply::downloadProgress, this, &YtbInputSource::onDownloadProgress);
}

void YtbInputSource::onFinished(QNetworkReply *reply)
{
    if(reply == m_getStreamReply)
    {
        if(reply->error() != QNetworkReply::NoError)
        {
            qCWarning(plugin, "downloading finished with error: %s", qPrintable(reply->errorString()));
            if(!m_ready)
            {
                emit error();
            }
            m_buffer->stop();
        }
        else
        {
            m_buffer->addData(m_getStreamReply->readAll());
            qCDebug(plugin) << "downloading finished";
        }

        m_getStreamReply = nullptr;
    }
    else
    {
        if(reply->error() == QNetworkReply::OperationCanceledError && m_buffer->seekRequestPos() > 0)
        {
            qCDebug(plugin) << "processing seek request...";
            m_buffer->clearRequestPos();
            m_request.setRawHeader("Range", QStringLiteral("bytes=%1-").arg(m_offset).toLatin1());
            m_request.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
            m_buffer->setOffset(m_offset);
            m_getStreamReply = m_manager->get(m_request);
            m_getStreamReply->setReadBufferSize(0);
            connect(m_getStreamReply, &QNetworkReply::downloadProgress, this, &YtbInputSource::onDownloadProgress);
        }
    }

    reply->deleteLater();
}

void YtbInputSource::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    Q_UNUSED(bytesTotal);

    if(!m_ready && bytesReceived > PREBUFFER_SIZE)
    {
        qCDebug(plugin) << "ready";
        m_ready = true;
        m_buffer->open(QIODevice::ReadOnly);
        emit ready();
    }
    else if(!m_ready)
    {
        StateHandler::instance()->dispatchBuffer(100 * bytesReceived / PREBUFFER_SIZE);
    }

    if(m_getStreamReply)
    {
        m_buffer->addData(m_getStreamReply->readAll());
    }
}

void YtbInputSource::onSeekRequest()
{
    m_offset = m_buffer->seekRequestPos();
    qCDebug(plugin) << "seek request position:" << m_offset;
    if(m_getStreamReply)
    {
        QNetworkReply *prevReply = m_getStreamReply;
        m_getStreamReply = nullptr;
        prevReply->abort();
    }
    else
    {
        m_buffer->clearRequestPos();
        m_request.setRawHeader("Range", QStringLiteral("bytes=%1-").arg(m_offset).toLatin1());
        m_request.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
        m_buffer->setOffset(m_offset);
        m_getStreamReply = m_manager->get(m_request);
        m_getStreamReply->setReadBufferSize(0);
        connect(m_getStreamReply, &QNetworkReply::downloadProgress, this, &YtbInputSource::onDownloadProgress);
    }
}
