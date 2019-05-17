/***************************************************************************
 *   Copyright (C) 2009-2012 by Ilya Kotov                                 *
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

#ifndef YTBINPUTSOURCE_H
#define YTBINPUTSOURCE_H

#include <QNetworkAccessManager>
#include <qmmp/inputsource.h>

class QNetworkReply;

/**
    @author Ilya Kotov <forkotov02@ya.ru>
*/
class YtbInputSource : public InputSource
{
Q_OBJECT
public:
    YtbInputSource(const QString &path, QObject *parent = nullptr);

    QIODevice *ioDevice() override;
    bool initialize() override;
    bool isReady() override;
    bool isWaiting() override;
    QString contentType() const override;

private slots:
    void onFinished(QNetworkReply *reply);
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

private:
    QString m_url;
    bool m_ready = false;
    QNetworkAccessManager *m_manager;
    QNetworkReply *m_getVideoReply = nullptr;
    QNetworkReply *m_getStreamReply = nullptr;

};

#endif // YTBINPUTSOURCE_H
