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
#include <stdlib.h>
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

bool BufferDevice::addData(const QByteArray &data)
{
    QMutexLocker locker(&m_mutex);
    if(m_writeAt + data.size() > m_bufferSize && m_readAt > 0)
    {
        m_writeAt -= m_readAt;
        memmove(m_buffer, m_buffer + m_readAt, m_writeAt);
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
    return true;
}

bool BufferDevice::isSequential() const
{
    return true;
}

qint64 BufferDevice::bytesAvailable() const
{
    QMutexLocker locker(&m_mutex);
    return m_writeAt - m_readAt + QIODevice::bytesAvailable();
}

qint64 BufferDevice::readData(char *data, qint64 maxSize)
{
    QMutexLocker locker(&m_mutex);
    if(!m_buffer)
        return -1;
    qint64 size = qMin(maxSize, m_writeAt - m_readAt);
    memcpy(data, m_buffer + m_readAt, size);
    m_readAt += size;
    return size;
}

qint64 BufferDevice::writeData(const char *data, qint64 maxSize)
{
    Q_UNUSED(data);
    Q_UNUSED(maxSize);
    return -1;
}
