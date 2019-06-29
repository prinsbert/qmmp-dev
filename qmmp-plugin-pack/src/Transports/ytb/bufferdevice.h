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

#ifndef BUFFERDEVICE_H
#define BUFFERDEVICE_H

#include <QObject>
#include <QIODevice>
#include <QByteArray>
#include <QMutex>

class BufferDevice : public QIODevice
{
    Q_OBJECT
public:
    BufferDevice(QObject *parent);
    ~BufferDevice();

    bool addData(const QByteArray &data);
    bool isSequential() const override;
    qint64 bytesAvailable() const override;

private:
    qint64 readData(char *data, qint64 maxSize) override;
    qint64 writeData(const char *data, qint64 maxSize) override;
    char *m_buffer;
    qint64 m_readAt = 0;
    qint64 m_writeAt = 0;
    qint64 m_bufferSize = 0;
    mutable QMutex m_mutex;

};

#endif // BUFFERDEVICE_H
