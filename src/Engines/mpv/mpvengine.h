/***************************************************************************
 *   Copyright (C) 2008-2026 by Ilya Kotov                                 *
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

#ifndef MPVENGINE_H
#define MPVENGINE_H

#include <QQueue>
#include <QString>
#include <QProcess>
#include <mpv/client.h>
#include <qmmp/statehandler.h>
#include <qmmp/abstractengine.h>

class Output;
class QIDevice;
class QMenu;
class QProcess;
class TrackInfo;
class InputSource;

class MpvEngine : public AbstractEngine
{
    Q_OBJECT
public:
    explicit MpvEngine(EngineFactory *factory, QObject *parent);
    virtual ~MpvEngine();

    // Engine API
    bool play() override;
    bool enqueue(InputSource *source) override;
    bool initialize();
    void seek(qint64) override;
    void stop() override;
    void pause() override;

private slots:
    void setMuted(bool muted);
    void onError(QProcess::ProcessError error);

private:
    void sendMetaData();
    void processEvents();
    Qmmp::AudioFormat findFormat(const char *name) const;
    static void wakeup(void *data);


    mpv_handle *m_ctx = nullptr;

    QProcess *m_process = nullptr;
    int m_bitrate = 0;
    int m_samplerate = 0;
    int m_channels = 0;
    int m_bitsPerSample = 0;
    bool m_user_stop = false;
    qint64 m_currentTime = 0;
    qint64 m_duration = 0;
    QQueue <InputSource*> m_sources;
    InputSource *m_source = nullptr;
    EngineFactory *m_factory;
};


#endif // MPVENGINE_H
