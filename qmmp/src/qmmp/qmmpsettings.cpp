/***************************************************************************
 *   Copyright (C) 2010-2025 by Ilya Kotov                                 *
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

#include <QSettings>
#include <QApplication>
#include "metadatamanager.h"
#include "qmmp.h"
#include "qmmpsettings.h"

QmmpSettings *QmmpSettings::m_instance = nullptr;

QmmpSettings::QmmpSettings(QObject *parent) : QObject(parent)
{
    if(m_instance)
        qCFatal(core) << "only one instance is allowed";
    m_instance = this;
    QSettings settings;
    //replaygain settings
    settings.beginGroup(u"ReplayGain"_s);
    m_rg_mode = (ReplayGainMode) settings.value(u"mode"_s, REPLAYGAIN_DISABLED).toInt();
    m_rg_preamp = settings.value(u"preamp"_s, 0.0).toDouble();
    m_rg_defaut_gain = settings.value(u"default_gain"_s, 0.0).toDouble();
    m_rg_prevent_clipping = settings.value(u"prevent_clipping"_s, true).toBool();
    settings.endGroup();
    //audio settings
    m_aud_software_volume = settings.value(u"Output/software_volume"_s, false).toBool();
    m_aud_format = static_cast<Qmmp::AudioFormat>(settings.value(u"Output/format"_s, Qmmp::PCM_S16LE).toInt());
    m_aud_dithering = settings.value(u"Output/dithering"_s, true).toBool();
    m_volume_step = settings.value(u"Output/volume_step"_s, 10).toInt();
    m_average_bitrate = settings.value(u"Output/average_bitrate"_s, false).toBool();
    //cover settings
    settings.beginGroup(u"Cover"_s);
    m_cover_inc = settings.value(u"include"_s, QStringList{ u"*.jpg"_s , u"*.png"_s , u"*.webp"_s }).toStringList();
    m_cover_exclude = settings.value(u"exclude"_s, QStringList{ u"*back*"_s }).toStringList();
    m_cover_depth = settings.value(u"depth"_s, 0).toInt();
    m_cover_use_files = settings.value(u"use_files"_s, true).toBool();
    settings.endGroup();
    //network settings
    m_proxy_enabled = settings.value(u"Proxy/use_proxy"_s, false).toBool();
    m_proxy_auth = settings.value(u"Proxy/authentication"_s, false).toBool();
    m_proxy_type = static_cast<ProxyType>(settings.value(u"Proxy/proxy_type"_s, HTTP_PROXY).toInt());
    m_proxy_url = settings.value(u"Proxy/url"_s).toUrl();
    //buffer
    m_buffer_size = settings.value(u"Output/buffer_size"_s, 500).toInt();
    //file type determination
    m_determine_by_content = settings.value(u"Misc/determine_file_by_content"_s, false).toBool();
}

QmmpSettings::~QmmpSettings()
{
    sync();
    m_instance = nullptr;
}

QmmpSettings::ReplayGainMode QmmpSettings::replayGainMode() const
{
    return m_rg_mode;
}

double QmmpSettings::replayGainPreamp() const
{
    return m_rg_preamp;
}

double QmmpSettings::replayGainDefaultGain() const
{
    return m_rg_defaut_gain;
}

bool QmmpSettings::replayGainPreventClipping() const
{
    return m_rg_prevent_clipping;
}

void QmmpSettings::setReplayGainSettings(ReplayGainMode mode, double preamp, double def_gain, bool clip)
{
    m_rg_mode = mode;
    m_rg_preamp = preamp;
    m_rg_defaut_gain = def_gain;
    m_rg_prevent_clipping = clip;
    saveSettings();
    emit replayGainSettingsChanged();
}

bool QmmpSettings::useSoftVolume() const
{
    return m_aud_software_volume;
}

Qmmp::AudioFormat QmmpSettings::outputFormat() const
{
    return m_aud_format;
}

bool QmmpSettings::useDithering() const
{
    return m_aud_dithering;
}

void QmmpSettings::setAudioSettings(bool soft_volume, Qmmp::AudioFormat format, bool use_dithering)
{
    m_aud_software_volume = soft_volume;
    m_aud_format = format;
    m_aud_dithering = use_dithering;
    saveSettings();
    emit audioSettingsChanged();
}

const QStringList &QmmpSettings::coverNameFilters(bool include) const
{
    return include ? m_cover_inc : m_cover_exclude;
}

int QmmpSettings::coverSearchDepth() const
{
    return m_cover_depth;
}

bool QmmpSettings::useCoverFiles() const
{
    return m_cover_use_files;
}

void QmmpSettings::setCoverSettings(QStringList inc, QStringList exc, int depth, bool use_files)
{
    m_cover_inc = inc;
    m_cover_exclude = exc;
    m_cover_depth = depth;
    m_cover_use_files = use_files;
    MetaDataManager::instance()->clearCoverCache();
    saveSettings();
    emit coverSettingsChanged();
}

bool QmmpSettings::isProxyEnabled() const
{
    return m_proxy_enabled;
}

bool QmmpSettings::useProxyAuth() const
{
    return m_proxy_auth;
}

const QUrl &QmmpSettings::proxy() const
{
    return m_proxy_url;
}

QmmpSettings::ProxyType QmmpSettings::proxyType() const
{
    return m_proxy_type;
}

void QmmpSettings::setNetworkSettings(bool use_proxy, bool auth, ProxyType type, const QUrl &proxy)
{
    m_proxy_enabled = use_proxy;
    m_proxy_auth = auth;
    m_proxy_type = type;
    m_proxy_url = proxy;
    if(type == HTTP_PROXY)
        m_proxy_url.setScheme(u"http"_s);
    else if(type == SOCKS5_PROXY)
        m_proxy_url.setScheme(u"socks5"_s);
    saveSettings();
    emit networkSettingsChanged();
}

const EqSettings &QmmpSettings::eqSettings() const
{
    return m_eq_settings;
}

void QmmpSettings::setEqSettings(const EqSettings &settings)
{
    m_eq_settings = settings;
    saveSettings();
    emit eqSettingsChanged();
}

void QmmpSettings::readEqSettings(EqSettings::Bands bands)
{
    m_eq_settings = EqSettings(bands);
    QSettings settings;
    settings.beginGroup(QStringLiteral("Equalizer_%1").arg(bands));
    for (int i = 0; i < bands; ++i)
        m_eq_settings.setGain(i, settings.value(QStringLiteral("band_%1").arg(i), 0).toDouble());
    m_eq_settings.setPreamp(settings.value(u"preamp"_s, 0).toDouble());
    m_eq_settings.setEnabled(settings.value(u"enabled"_s, false).toBool());
    settings.endGroup();
    m_eq_settings.setTwoPasses(settings.value(u"Equalizer/two_passes"_s, true).toBool());
    emit eqSettingsChanged();
}

int QmmpSettings:: bufferSize() const
{
    return m_buffer_size;
}

void QmmpSettings::setBufferSize(int msec)
{
    m_buffer_size = msec;
}

void QmmpSettings::setVolumeStep(int step)
{
    m_volume_step = qBound(1, step, 20);
}

int QmmpSettings::volumeStep() const
{
    return m_volume_step;
}

void QmmpSettings::setAverageBitrate(bool enabled)
{
    m_average_bitrate = enabled;
    saveSettings();
    emit audioSettingsChanged();
}

bool QmmpSettings::averageBitrate() const
{
    return m_average_bitrate;
}

void QmmpSettings::sync()
{
    if(m_saveSettings)
    {
        qCDebug(core) << "saving settings...";
        QSettings settings;
        //replaygain settings
        settings.beginGroup(u"ReplayGain"_s);
        settings.setValue(u"mode"_s, m_rg_mode);
        settings.setValue(u"preamp"_s, m_rg_preamp);
        settings.setValue(u"default_gain"_s, m_rg_defaut_gain);
        settings.setValue(u"prevent_clipping"_s, m_rg_prevent_clipping);
        settings.endGroup();
        //audio settings
        settings.setValue(u"Output/software_volume"_s, m_aud_software_volume);
        settings.setValue(u"Output/format"_s, m_aud_format);
        settings.setValue(u"Output/dithering"_s, m_aud_dithering);
        settings.setValue(u"Output/volume_step"_s, m_volume_step);
        settings.setValue(u"Output/average_bitrate"_s, m_average_bitrate);
        //cover settings
        settings.beginGroup(u"Cover"_s);
        settings.setValue(u"include"_s, m_cover_inc);
        settings.setValue(u"exclude"_s, m_cover_exclude);
        settings.setValue(u"depth"_s, m_cover_depth);
        settings.setValue(u"use_files"_s, m_cover_use_files);
        settings.endGroup();
        //network settings
        settings.setValue(u"Proxy/use_proxy"_s, m_proxy_enabled);
        settings.setValue(u"Proxy/authentication"_s, m_proxy_auth);
        settings.setValue(u"Proxy/url"_s, m_proxy_url);
        settings.setValue(u"Proxy/proxy_type"_s, m_proxy_type);
        //equalizer settings
        settings.beginGroup(QStringLiteral("Equalizer_%1").arg(m_eq_settings.bands()));
        for (int i = 0; i < m_eq_settings.bands(); ++i)
            settings.setValue(QStringLiteral("band_%1").arg(i), m_eq_settings.gain(i));
        settings.setValue(u"preamp"_s, m_eq_settings.preamp());
        settings.setValue(u"enabled"_s, m_eq_settings.isEnabled());
        settings.endGroup();
        settings.setValue(u"Equalizer/two_passes"_s, m_eq_settings.twoPasses());
        //buffer size
        settings.setValue(u"Output/buffer_size"_s, m_buffer_size);
        //file type determination
        settings.setValue(u"Misc/determine_file_by_content"_s, m_determine_by_content);
        m_saveSettings = false;  //protect from multiple calls
    }
}

void QmmpSettings::saveSettings()
{
    m_saveSettings = true;
    QMetaObject::invokeMethod(this, &QmmpSettings::sync, Qt::QueuedConnection);
}

QmmpSettings* QmmpSettings::instance()
{
    if(!m_instance)
        return new QmmpSettings(qApp);
    return m_instance;
}

void QmmpSettings::setDetermineFileTypeByContent(bool enabled)
{
    m_determine_by_content = enabled;
}

bool QmmpSettings::determineFileTypeByContent() const
{
    return m_determine_by_content;
}
