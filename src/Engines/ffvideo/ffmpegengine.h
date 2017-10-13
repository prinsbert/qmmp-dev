/***************************************************************************
 *   Copyright (C) 2017 by Ilya Kotov                                      *
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

#ifndef FFMPEGENGINE_H
#define FFMPEGENGINE_H

extern "C"{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavcodec/version.h>
#include <libavutil/mathematics.h>
#include <libavutil/dict.h>
#include <libswscale/swscale.h>
}

#include <QQueue>
#include <QString>
#include <QPointer>
#include <QSharedPointer>
#include <qmmp/statehandler.h>
#include <qmmp/abstractengine.h>
#include "ffvideodecoder.h"

class Output;
class QIDevice;
class QMenu;
class QProcess;
class FileInfo;
class InputSource;
class PacketBuffer;
class AudioThread;
class VideoThread;
class VideoWindow;

class FFmpegEngine : public AbstractEngine
{
    Q_OBJECT
public:
    FFmpegEngine(EngineFactory *factory, QObject *parent);
    virtual ~FFmpegEngine();

    // Engine API
    bool play();
    bool enqueue(InputSource *source);
    void seek(qint64);
    void stop();
    void pause();
    void setMuted(bool muted);

private slots:
    void onStopRequest();

private:
    void run();
    void sendMetaData();
    void clearDecoders();
    void reset();

    EngineFactory *m_factory;
    PacketBuffer *m_audioBuffer;
    PacketBuffer *m_videoBuffer;
    AudioThread *m_audioThread;
    VideoThread *m_videoThread;
    QQueue <FFVideoDecoder*> m_decoders;
    QHash <FFVideoDecoder*, InputSource*> m_inputs;
    QPointer<VideoWindow> m_videoWindow;
    FFVideoDecoder *m_decoder;
    bool m_done, m_finish, m_user_stop;
    qint64 m_seekTime;
    QSharedPointer<QMap<Qmmp::MetaData, QString> > m_metaData;
};


#endif // FFMPEGENGINE_H
