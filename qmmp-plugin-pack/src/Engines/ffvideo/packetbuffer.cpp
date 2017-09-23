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

#include "packetbuffer.h"

PacketBuffer::PacketBuffer(int size)
{
    m_size = size;
    m_add_index = 0;
    m_done_index = 0;
    m_current_count = 0;
    m_packets = new AVPacket*[size];
    for(unsigned int i = 0; i < m_size; ++i)
    {
        m_packets[i] = av_packet_alloc();
        av_init_packet(m_packets[i]);
    }
}

PacketBuffer::~PacketBuffer()
{
    for(unsigned int i = 0; i < m_size; ++i)
    {
        av_packet_unref(m_packets[i]);
        av_packet_free(&m_packets[i]);
    }
    delete [] m_packets;
    m_packets = 0;
}

bool PacketBuffer::full() const
{
    return m_current_count == m_size;
}

bool PacketBuffer::empty() const
{
    return m_current_count == 0;
}

unsigned  int PacketBuffer::available() const
{
    return m_size - m_current_count;
}

unsigned int PacketBuffer::used() const
{
    return m_current_count;
}

AVPacket *PacketBuffer::next() const
{
    if(m_current_count)
    {
        //m_blocked = m_buffers[m_done_index];
        return m_packets[m_done_index] ;
    }
    return 0;
}

AVPacket *PacketBuffer::get() const
{
    if (full())
        return 0;
    return m_packets[m_add_index];
}

void PacketBuffer::add()
{
    m_add_index = (m_add_index + 1) % m_size;
    m_current_count++;
}

void PacketBuffer::done()
{
    //m_blocked = 0;
    if (m_current_count)
    {
        av_packet_unref(m_packets[m_done_index]);
        av_init_packet(m_packets[m_done_index]);
        m_current_count--;
        m_done_index = (m_done_index + 1) % m_size;
    }
}

void PacketBuffer::clear()
{
    m_current_count = 0;
    m_add_index = 0;
    m_done_index = 0;
    for(unsigned int i = 0; i < m_size; ++i)
    {
        av_packet_unref(m_packets[m_done_index]);
        av_init_packet(m_packets[i]);
    }
}

QMutex *PacketBuffer::mutex()
{
    return &m_mutex;
}

QWaitCondition *PacketBuffer::cond()
{
    return &m_condition;
}
