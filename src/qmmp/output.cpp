// Copyright (c) 2000-2001 Brad Hughes <bhughes@trolltech.com>
//
// Use, modification and distribution is allowed without limitation,
// warranty, or liability of any kind.
//
#include <QStringList>
#include <QDir>
#include "audioparameters.h"
#include "qmmp.h"
#include "qmmpplugincache_p.h"
#include "output.h"

class OutputPrivate
{
public:
    static void loadPlugins()
    {
        if (cache)
            return;

        cache = new QList<QmmpPluginCache *>;
        QSettings settings;
        for(const QString &filePath : Qmmp::findPlugins(u"Output"_s))
        {
            QmmpPluginCache *item = new QmmpPluginCache(filePath, &settings);
            if(item->hasError())
            {
                delete item;
                continue;
            }
            cache->append(item);
        }
        QmmpPluginCache::cleanup(&settings);
    }

    quint32 frequency = 0;
    ChannelMap channelMap;
    Qmmp::AudioFormat format = Qmmp::PCM_UNKNOWN;
    int sampleSize = 0;
    static QList<QmmpPluginCache*> *cache;
};

QList<QmmpPluginCache*> *OutputPrivate::cache = nullptr;

Output::Output() : d_ptr(new OutputPrivate)
{}

void Output::configure(quint32 freq, ChannelMap map, Qmmp::AudioFormat format)
{
    Q_D(Output);
    d->frequency = freq;
    d->channelMap = map;
    d->format = format;
    d->sampleSize = AudioParameters::sampleSize(format);
}

AudioParameters Output::audioParameters() const
{
    Q_D(const Output);
    return AudioParameters(d->frequency, d->channelMap, d->format);
}

quint32 Output::sampleRate() const
{
    return d_ptr->frequency;
}

int Output::channels() const
{
    return d_ptr->channelMap.count();
}

ChannelMap Output::channelMap() const
{
    return d_ptr->channelMap;
}

Qmmp::AudioFormat Output::format() const
{
    return d_ptr->format;
}

int Output::sampleSize() const
{
    return d_ptr->sampleSize;
}

void Output::suspend()
{}

void Output::resume()
{}

void Output::setTrackInfo(const TrackInfo &info)
{
    Q_UNUSED(info);
}

Output::~Output()
{
    delete d_ptr;
}

// static methods
Output *Output::create()
{
    OutputPrivate::loadPlugins();
    Output *output = nullptr;
    if (OutputPrivate::cache->isEmpty ())
    {
        qCDebug(core) << "unable to find output plugins";
        return output;
    }
    OutputFactory *fact = Output::currentFactory();
    if (fact)
        output = fact->create();
    return output;
}

QList<OutputFactory *> Output::factories()
{
    OutputPrivate::loadPlugins();
    QList<OutputFactory *> list;
    for(QmmpPluginCache *item : std::as_const(*OutputPrivate::cache))
    {
        if(item->outputFactory())
            list.append(item->outputFactory());
    }
    return list;
}

QString Output::file(const OutputFactory *factory)
{
    OutputPrivate::loadPlugins();
    for(const QmmpPluginCache *item : std::as_const(*OutputPrivate::cache))
    {
        if(item->shortName() == factory->properties().shortName)
            return item->file();
    }
    return QString();
}

void Output::setCurrentFactory(const OutputFactory *factory)
{
    OutputPrivate::loadPlugins();
    if (file(factory).isEmpty())
        return;
    QSettings settings;
    settings.setValue(u"Output/current_plugin"_s, factory->properties().shortName);
}

OutputFactory *Output::currentFactory()
{
    OutputPrivate::loadPlugins();

    QSettings settings;
#ifdef QMMP_DEFAULT_OUTPUT
    QString name = settings.value(u"Output/current_plugin"_s, QMMP_DEFAULT_OUTPUT).toString();
#else
#ifdef Q_OS_LINUX
    QString name = settings.value(u"Output/current_plugin"_s, u"alsa"_s).toString();
#elif defined Q_OS_WIN
    QString name = settings.value(u"Output/current_plugin"_s, u"wasapi"_s).toString();
#elif defined Q_OS_MAC
    QString name = settings.value(u"Output/current_plugin"_s, u"qtmultimedia"_s).toString();
#else
    QString name = settings.value(u"Output/current_plugin"_s, u"oss4"_s).toString();
#endif
#endif //QMMP_DEFAULT_OUTPUT
    for(QmmpPluginCache *item : std::as_const(*OutputPrivate::cache))
    {
        if (item->shortName() == name && item->outputFactory())
            return item->outputFactory();
    }
    if(!OutputPrivate::cache->isEmpty())
        return OutputPrivate::cache->at(0)->outputFactory();
    return nullptr;
}
