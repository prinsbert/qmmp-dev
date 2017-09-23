/***************************************************************************
 *   Copyright (C) 2017 by Ilya Kotov                                      *
 *   forkotov02@hotmail.ru                                                 *
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

#ifndef AUDIOTHREAD_H
#define AUDIOTHREAD_H

extern "C"{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavcodec/version.h>
#include <libavutil/mathematics.h>
#include <libavutil/dict.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

#include <QThread>
#include <QMutex>

class Output;
class PacketBuffer;

class AudioThread : public QThread
{
    Q_OBJECT
public:
    explicit AudioThread(PacketBuffer *buf, QObject *parent = 0);

    bool initialize(AVCodecContext *ctx);

private:
    void run();

    QMutex m_mutex;
    AVCodecContext *m_context;
    Output *m_output;
    PacketBuffer *m_buffer;

};

#endif // AUDIOTHREAD_H
