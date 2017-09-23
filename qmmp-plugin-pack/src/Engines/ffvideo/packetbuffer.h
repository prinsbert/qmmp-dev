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

#ifndef PACKETBUFFER_H
#define PACKETBUFFER_H


#include <QMutex>
#include <QWaitCondition>

extern "C"{
#include <libavcodec/avcodec.h>
}

class PacketBuffer
{
public:
    explicit PacketBuffer(int size);

    ~PacketBuffer();

    bool full() const;
    bool empty() const;
    unsigned int available() const;
    unsigned int used() const;
    AVPacket *next() const;
    AVPacket *get() const;
    void add();
    void done();
    void clear();

    QMutex *mutex();
    QWaitCondition *cond();

private:
    unsigned int m_size, m_add_index, m_done_index, m_current_count;
    AVPacket **m_packets;
    QMutex m_mutex;
    QWaitCondition m_condition;
};

#endif // PACKETBUFFER_H
