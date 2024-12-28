/***************************************************************************
 *   Copyright (C) 2009-2025 by Ilya Kotov                                 *
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

#include <QRegularExpression>
#include <QFileInfo>
#include <QStringList>
#include <QProcess>
#include <QHash>
#include "mplayermetadatamodel.h"

MplayerMetaDataModel::MplayerMetaDataModel(const QString &path)
    : MetaDataModel(true, MetaDataModel::CompletePropertyList),
      m_path(path)
{}

MplayerMetaDataModel::~MplayerMetaDataModel()
{}

QList<MetaDataItem> MplayerMetaDataModel::extraProperties() const
{
    //prepare and start mplayer process
    QStringList args = { u"-slave"_s, u"-identify"_s, u"-frames"_s, u"0"_s, u"-vo"_s, u"null"_s, u"-ao"_s, u"null"_s, m_path };
    QProcess mplayer_process;
    mplayer_process.start(u"mplayer"_s, args);
    mplayer_process.waitForFinished();
    QString str = QString::fromLocal8Bit(mplayer_process.readAll()).trimmed();
    const QStringList lines = str.split(QChar::LineFeed);
    //mplayer std output parsing
    static const QRegularExpression rx_id(u"^(ID_.*)=(.*)"_s);
    QHash<QString, QString> params;
    for(const QString &line : std::as_const(lines))
    {
        QRegularExpressionMatch match = rx_id.match(line.trimmed());
        if(match.hasMatch())
            params.insert(match.captured(1), match.captured(2));
    }

    QList<MetaDataItem> ep = {
        //general info
        MetaDataItem(tr("Size"), QFileInfo(m_path).size () / 1024, tr("KiB")),
        MetaDataItem(tr("Demuxer"), params[u"ID_DEMUXER"_s]),
        //video info
        MetaDataItem(tr("Video format"), params[u"ID_VIDEO_FORMAT"_s]),
        MetaDataItem(tr("FPS"), params[u"ID_VIDEO_FPS"_s]),
        MetaDataItem(tr("Video codec"), params[u"ID_VIDEO_CODEC"_s]),
        MetaDataItem(tr("Aspect ratio"), params[u"ID_VIDEO_ASPECT"_s]),
        MetaDataItem(tr("Video bitrate"), params[u"ID_VIDEO_BITRATE"_s].toInt() / 1000, tr("kbps")),
        MetaDataItem(tr("Resolution"), QStringLiteral("%1x%2").arg(params[u"ID_VIDEO_WIDTH"_s], params[u"ID_VIDEO_HEIGHT"_s])),
        MetaDataItem(tr("Audio codec"), params[u"ID_AUDIO_CODEC"_s]),
        MetaDataItem(tr("Sample rate"), params[u"ID_AUDIO_RATE"_s], tr("Hz")),
        MetaDataItem(tr("Audio bitrate"), params[u"ID_AUDIO_BITRATE"_s].toInt() / 1000, tr("kbps")),
        MetaDataItem(tr("Channels"), params[u"ID_AUDIO_NCH"_s])
    };
    return ep;
}
