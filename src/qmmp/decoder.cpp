// Copyright (c) 2000-2001 Brad Hughes <bhughes@trolltech.com>
//
// Use, modification and distribution is allowed without limitation,
// warranty, or liability of any kind.
//
#include <QStringList>
#include <QSettings>
#include <QBuffer>
#include <QDir>
#include <math.h>
#include <algorithm>
#include "qmmpplugincache_p.h"
#include "output.h"
#include "decoderfactory.h"

extern "C"
{
#include "equ/iir.h"
}
#include "decoder.h"

Decoder::Decoder(QIODevice *input) : m_input(input)
{}

Decoder::~Decoder()
{}

void Decoder::setReplayGainInfo(const QMap<Qmmp::ReplayGainKey, double> &rg)
{
    m_rg = rg;
}

void Decoder::configure(quint32 srate, const ChannelMap &map, Qmmp::AudioFormat format)
{
    configure(AudioParameters(srate, map, format));
}

void Decoder::configure(quint32 srate, int channels, Qmmp::AudioFormat f)
{
    qCDebug(core) << "using internal channel order";
    configure(AudioParameters(srate, ChannelMap(channels), f));
}

void Decoder::configure(const AudioParameters &p)
{
    m_parameters = p;
    setProperty(Qmmp::SAMPLERATE, m_parameters.sampleRate());
    setProperty(Qmmp::CHANNELS, m_parameters.channels());
    setProperty(Qmmp::BITS_PER_SAMPLE, m_parameters.validBitsPerSample());
}

void Decoder::next()
{}

const QString Decoder::nextURL() const
{
    return QString();
}

const AudioParameters &Decoder::audioParameters() const
{
    return m_parameters;
}

const QMap<Qmmp::ReplayGainKey, double> &Decoder::replayGainInfo() const
{
    return m_rg;
}

void Decoder::addMetaData(const QMap<Qmmp::MetaData, QString> &metaData)
{
    m_metaData = metaData;
    m_hasMetaData = true;
}

QIODevice *Decoder::input()
{
    return m_input;
}

bool Decoder::hasMetaData() const
{
    return m_hasMetaData;
}

QMap<Qmmp::MetaData, QString> Decoder::takeMetaData()
{
    m_hasMetaData = false;
    return m_metaData;
}

void Decoder::setProperty(Qmmp::TrackProperty key, const QVariant &value)
{
    QString strValue = value.toString();
    if(strValue.isEmpty() || strValue == "0"_L1)
        m_properties.remove(key);
    else
        m_properties[key] = strValue;
}

void Decoder::setProperties(const QMap<Qmmp::TrackProperty, QString> &properties)
{
    for(auto it = properties.cbegin(); it != properties.cend(); ++it)
        setProperty(it.key(), it.value());
}

QMap<Qmmp::TrackProperty, QString> Decoder::properties() const
{
    return m_properties;
}

// static methods
QStringList Decoder::m_disabledNames;
QList<QmmpPluginCache*> *Decoder::m_cache = nullptr;

//sort cache items by priority
static bool _pluginCacheLessComparator(const QmmpPluginCache* f1, const QmmpPluginCache* f2)
{
    return f1->priority() < f2->priority();
}

void Decoder::loadPlugins()
{
    if (m_cache)
        return;

    m_cache = new QList<QmmpPluginCache*>;
    QSettings settings;
    for(const QString &filePath : Qmmp::findPlugins(u"Input"_s))
    {
        QmmpPluginCache *item = new QmmpPluginCache(filePath, &settings);
        if(item->hasError())
        {
            delete item;
            continue;
        }
        m_cache->append(item);
    }
    m_disabledNames = settings.value(u"Decoder/disabled_plugins"_s).toStringList();
    std::stable_sort(m_cache->begin(), m_cache->end(), _pluginCacheLessComparator);
    QmmpPluginCache::cleanup(&settings);

    qAddPostRoutine(Decoder::updateCache);
}

void Decoder::updateCache()
{
    if(m_cache)
    {
        QSettings settings;
        for(QmmpPluginCache *item : std::as_const(*m_cache))
            item->update(&settings);
    }
}

QString Decoder::file(const DecoderFactory *factory)
{
    loadPlugins();
    for(const QmmpPluginCache *item : std::as_const(*m_cache))
    {
        if(item->shortName() == factory->properties().shortName)
            return item->file();
    }
    return QString();
}

QStringList Decoder::protocols()
{
    loadPlugins();
    QStringList protocolList;

    for(QmmpPluginCache *item : std::as_const(*m_cache))
    {
        if(m_disabledNames.contains(item->shortName()))
            continue;

        protocolList << item->protocols();
    }
    protocolList.removeDuplicates();
    return protocolList;
}

DecoderFactory *Decoder::findByFilePath(const QString &path, bool useContent)
{
    loadPlugins();

    //get list of available/supported factories
    QList<DecoderFactory*> filtered = useContent ? enabledFactories() : findByFileExtension(path);

    if(filtered.isEmpty())
        return nullptr;

    //try to find by content
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly))
    {
        qCWarning(core, "file open error: %s", qPrintable(file.errorString()));
        return nullptr;
    }

    for(DecoderFactory *fact : std::as_const(filtered))
    {
        if(fact->canDecode(&file))
            return fact;
    }

    //fallback: try to find by extension
    if(useContent)
        filtered = findByFileExtension(path);

    for(DecoderFactory *fact : std::as_const(filtered))
    {
        if(fact->properties().noInput || fact->properties().protocols.contains(u"file"_s))
            return fact;
    }

    //fallback: try to find by content
    if(!useContent)
        return findByContent(&file);

    //fallback: use first available factory
    if(!filtered.isEmpty() && !useContent)
        return filtered.constFirst();

    return nullptr;
}

DecoderFactory *Decoder::findByMime(const QString& type)
{
    if(type.isEmpty())
        return nullptr;
    loadPlugins();
    for(QmmpPluginCache *item : std::as_const(*m_cache))
    {
        if(m_disabledNames.contains(item->shortName()))
            continue;
        DecoderFactory *fact = item->decoderFactory();
        if(fact && !fact->properties().noInput && fact->properties().contentTypes.contains(type))
            return fact;
    }
    return nullptr;
}

DecoderFactory *Decoder::findByContent(QIODevice *input)
{
    loadPlugins();
    for(QmmpPluginCache *item : std::as_const(*m_cache))
    {
        if(m_disabledNames.contains(item->shortName()))
            continue;
        DecoderFactory *fact = item->decoderFactory();
        if(fact && !fact->properties().noInput && fact->canDecode(input))
            return fact;
    }
    return nullptr;
}

DecoderFactory *Decoder::findByProtocol(const QString &p)
{
    loadPlugins();
    for(QmmpPluginCache *item : std::as_const(*m_cache))
    {
        if(m_disabledNames.contains(item->shortName()))
            continue;

        if (item->decoderFactory() && item->decoderFactory()->properties().protocols.contains(p))
            return item->decoderFactory();
    }
    return nullptr;
}

QList<DecoderFactory *> Decoder::findByFileExtension(const QString &path)
{
    QList<DecoderFactory*> filtered;
    DecoderFactory *fact = nullptr;
    for(QmmpPluginCache *item : std::as_const(*m_cache))
    {
        if(m_disabledNames.contains(item->shortName()))
            continue;

        if(!(fact = item->decoderFactory()))
            continue;

        if(QDir::match(fact->properties().filters, path.section(QLatin1Char('/'), -1)))
            filtered.append(fact);
    }

    return filtered;
}

void Decoder::setEnabled(DecoderFactory *factory, bool enable)
{
    loadPlugins();
    if (!factories().contains(factory))
        return;

    if(enable == isEnabled(factory))
        return;

    if(enable)
        m_disabledNames.removeAll(factory->properties().shortName);
    else
        m_disabledNames.append(factory->properties().shortName);

    m_disabledNames.removeDuplicates();
    QSettings settings;
    settings.setValue(u"Decoder/disabled_plugins"_s, m_disabledNames);
}

bool Decoder::isEnabled(const DecoderFactory *factory)
{
    loadPlugins();
    return !m_disabledNames.contains(factory->properties().shortName);
}

QList<DecoderFactory *> Decoder::factories()
{
    loadPlugins();
    QList<DecoderFactory *> list;
    for(QmmpPluginCache *item : std::as_const(*m_cache))
    {
        if(item->decoderFactory())
            list.append(item->decoderFactory());
    }
    return list;
}

QList<DecoderFactory *> Decoder::enabledFactories()
{
    loadPlugins();
    QList<DecoderFactory *> list;
    for(QmmpPluginCache *item : std::as_const(*m_cache))
    {
        if(m_disabledNames.contains(item->shortName()))
            continue;
        if(item->decoderFactory())
            list.append(item->decoderFactory());
    }
    return list;
}

QStringList Decoder::nameFilters()
{
    loadPlugins();
    QStringList filters;
    for(QmmpPluginCache *item : std::as_const(*m_cache))
    {
        if(m_disabledNames.contains(item->shortName()))
            continue;

        filters << item->filters();
    }
    return filters;
}

QStringList Decoder::contentTypes()
{
    loadPlugins();
    QStringList types;
    for(QmmpPluginCache *item : std::as_const(*m_cache))
    {
        if(m_disabledNames.contains(item->shortName()))
            continue;

        types << item->contentTypes();
    }
    return types;
}
