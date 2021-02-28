/***************************************************************************
 *   Copyright (C) 2019 by Ilya Kotov                                      *
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

#include <QMutexLocker>
#include <QtDebug>
#include <stdlib.h>
#include <unistd.h>
#include "bufferdevice.h"

#define INITIAL_BUFFER_SIZE 30000000

BufferDevice::BufferDevice(QObject *parent) : QIODevice(parent)
{
    m_buffer = (char *)malloc(INITIAL_BUFFER_SIZE);
    m_bufferSize = INITIAL_BUFFER_SIZE;
}

BufferDevice::~BufferDevice()
{
    if(m_buffer)
    {
        free(m_buffer);
        m_buffer = nullptr;
    }
}

void BufferDevice::setOffset(int offset)
{
    m_offset = offset;
}

void BufferDevice::setSize(int size)
{
    m_size = size;
}

bool BufferDevice::addData(const QByteArray &data)
{
    qDebug() << Q_FUNC_INFO;
    QMutexLocker locker(&m_mutex);
    if(m_writeAt + data.size() > m_bufferSize && m_readAt > 0)
    {
        m_writeAt -= m_readAt;
        memmove(m_buffer, m_buffer + m_readAt, m_writeAt);
        m_offset += m_readAt;
        m_readAt = 0;
    }

    if(m_writeAt + data.size() > m_bufferSize)
    {
        m_bufferSize = m_writeAt + data.size() + INITIAL_BUFFER_SIZE / 10;
        char *tmp = (char *)realloc(m_buffer, m_bufferSize);
        if(tmp)
            m_buffer = tmp;
        else
            return false;
    }

    memcpy(m_buffer + m_writeAt, data.data(), data.size());
    m_writeAt += data.size();
    if(m_writeAt > 250000)
        m_waiting = false;
    qDebug("++");
    return true;
}

qint64 BufferDevice::seekRequestPos() const
{
    //QMutexLocker locker(&m_mutex);
    return m_seekRequestPos;
}

bool BufferDevice::isWaiting() const
{
    return m_waiting;
}

void BufferDevice::clearRequestPos()
{
    QMutexLocker locker(&m_mutex);
    m_seekRequestPos = -1;
}

bool BufferDevice::isSequential() const
{
    return false;
}

/*qint64 BufferDevice::bytesAvailable() const
{
    QMutexLocker locker(&m_mutex);
    return m_writeAt - m_readAt + QIODevice::bytesAvailable();
}*/

qint64 BufferDevice::size() const
{
    return m_size;
}

bool BufferDevice::seek(qint64 pos)
{
    qDebug() << Q_FUNC_INFO << pos;

    if(pos >= m_offset && pos < m_writeAt + m_offset)
        m_readAt = pos - m_offset;
    else
    {
        m_seekRequestPos = pos;
        m_offset = pos;
        m_writeAt = 0;
        m_readAt = 0;
        m_waiting = true;
        emit seekRequest();
        while (m_writeAt < 250000) {
            qDebug("waiting");
            sleep(1);
        }
    }

    return QIODevice::seek(pos);
}

qint64 BufferDevice::readData(char *data, qint64 maxSize)
{
    qDebug() << Q_FUNC_INFO << maxSize;
    QMutexLocker locker(&m_mutex);
    if(!m_buffer)
        return -1;
    qint64 size = qMin(maxSize, m_writeAt - m_readAt);
    memcpy(data, m_buffer + m_readAt, size);
    m_readAt += size;
    qDebug("done");
    return size;
}

qint64 BufferDevice::writeData(const char *data, qint64 maxSize)
{
    Q_UNUSED(data);
    Q_UNUSED(maxSize);
    return -1;
}
