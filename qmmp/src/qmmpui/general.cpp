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

#include <QList>
#include <QDir>
#include <QDialog>
#include <qmmp/qmmp.h>
#include "uihelper.h"
#include "qmmpuiplugincache_p.h"
#include "general.h"

class GeneralPrivate
{
public:
    static void loadPlugins()
    {
        if(cache)
            return;

        cache = new QList<QmmpUiPluginCache*>;
        QSettings settings;
        for(const QString &filePath : Qmmp::findPlugins(u"General"_s))
        {
            QmmpUiPluginCache *item = new QmmpUiPluginCache(filePath, &settings);
            if(item->hasError())
            {
                delete item;
                continue;
            }
            cache->append(item);
        }
        enabledNames = settings.value(u"General/enabled_plugins"_s).toStringList();
        QmmpUiPluginCache::cleanup(&settings);
    }
    static QHash<GeneralFactory*, QObject*> *generals;
    static QObject *parent;
    static QList<QmmpUiPluginCache*> *cache;
    static QStringList enabledNames;
};

QList<QmmpUiPluginCache*> *GeneralPrivate::cache = nullptr;
QStringList GeneralPrivate::enabledNames;
QHash<GeneralFactory*, QObject*> *GeneralPrivate::generals = nullptr;
QObject *GeneralPrivate::parent = nullptr;

void General::create(QObject *parent)
{
    if(GeneralPrivate::generals)
        return;
    GeneralPrivate::generals = new QHash<GeneralFactory*, QObject*>();
    GeneralPrivate::parent = parent;
    GeneralPrivate::loadPlugins();
    for(QmmpUiPluginCache* item : std::as_const(*GeneralPrivate::cache))
    {
        if(!GeneralPrivate::enabledNames.contains(item->shortName()))
            continue;
        GeneralFactory *factory = item->generalFactory();
        if(factory)
        {
            QObject *general = factory->create(parent);
            if(general)
                GeneralPrivate::generals->insert(factory, general);
        }
    }
}

QList<GeneralFactory *> General::factories()
{
    GeneralPrivate::loadPlugins();
    QList<GeneralFactory *> list;
    for(QmmpUiPluginCache *item : std::as_const(*GeneralPrivate::cache))
    {
        if(item->generalFactory())
            list.append(item->generalFactory());
    }
    return list;
}

QList<GeneralFactory *> General::enabledFactories()
{
    GeneralPrivate::loadPlugins();
    QList<GeneralFactory *> list;
    for(QmmpUiPluginCache *item : std::as_const(*GeneralPrivate::cache))
    {
        if(!GeneralPrivate::enabledNames.contains(item->shortName()))
            continue;
        if(item->generalFactory())
            list.append(item->generalFactory());
    }
    return list;
}

QStringList General::enabledWidgets()
{
    QStringList out;
    for(const GeneralFactory *f : General::enabledFactories())
    {
        for(const WidgetDescription &desc : f->properties().widgets)
            out << QStringLiteral("%1_%2").arg(f->properties().shortName).arg(desc.id);
    }

    return out;
}

WidgetDescription General::widgetDescription(const QString &id)
{
    for(const GeneralFactory *f : General::enabledFactories())
    {
        for(const WidgetDescription &desc : f->properties().widgets)
        {
            if(id == QLatin1String("%1_%2").arg(f->properties().shortName).arg(desc.id))
              return desc;
        }
    }

    return { -1, QString(), QString(), Qt::LeftDockWidgetArea, Qt::NoDockWidgetArea };
}

QWidget *General::createWidget(const QString &id, QWidget *parent)
{
    for(GeneralFactory *f : General::enabledFactories())
    {
        for(const WidgetDescription &desc : f->properties().widgets)
        {
            if(id == QLatin1String("%1_%2").arg(f->properties().shortName).arg(desc.id))
              return f->createWidget(desc.id, parent);
        }
    }

    return nullptr;
}

QString General::file(const GeneralFactory *factory)
{
    GeneralPrivate::loadPlugins();
    auto it = std::find_if(GeneralPrivate::cache->cbegin(), GeneralPrivate::cache->cend(),
                           [factory] (QmmpUiPluginCache *item){ return item->shortName() == factory->properties().shortName; } );
    return it == GeneralPrivate::cache->cend() ? QString() : (*it)->file();
}

void General::setEnabled(GeneralFactory *factory, bool enable)
{
    GeneralPrivate::loadPlugins();
    if(!factories().contains(factory))
        return;

    if(enable == isEnabled(factory))
        return;

    QSettings settings;

    if(enable)
        GeneralPrivate::enabledNames << factory->properties().shortName;
    else
        GeneralPrivate::enabledNames.removeAll(factory->properties().shortName);
    GeneralPrivate::enabledNames.removeDuplicates();
    settings.setValue(u"General/enabled_plugins"_s, GeneralPrivate::enabledNames);

    if(!GeneralPrivate::generals)
        return;

    if(enable == GeneralPrivate::generals->contains(factory))
        return;

    if(enable)
    {
        QObject *general = factory->create(GeneralPrivate::parent);
        if(general)
            GeneralPrivate::generals->insert(factory, general);

        for(const WidgetDescription &d : factory->properties().widgets)
            emit UiHelper::instance()->widgetAdded(QStringLiteral("%1_%2").arg(factory->properties().shortName).arg(d.id));
    }
    else
    {
        for(const WidgetDescription &d : factory->properties().widgets)
            emit UiHelper::instance()->widgetRemoved(QStringLiteral("%1_%2").arg(factory->properties().shortName).arg(d.id));

        if(GeneralPrivate::generals->value(factory))
            delete GeneralPrivate::generals->take(factory);
    }
}

void General::showSettings(GeneralFactory *factory, QWidget *parentWidget)
{
    QDialog *dialog = factory->createSettings(parentWidget);
    if(!dialog)
        return;

    if(GeneralPrivate::generals && dialog->exec() == QDialog::Accepted && GeneralPrivate::generals->contains(factory))
    {
        delete GeneralPrivate::generals->take(factory);

        QObject *general = factory->create(GeneralPrivate::parent);
        if(general)
            GeneralPrivate::generals->insert(factory, general);

        for(const WidgetDescription &d : factory->properties().widgets)
            emit UiHelper::instance()->widgetUpdated(QStringLiteral("%1_%2").arg(factory->properties().shortName).arg(d.id));
    }
    dialog->deleteLater();
}

bool General::isEnabled(const GeneralFactory *factory)
{
    GeneralPrivate::loadPlugins();
    return GeneralPrivate::enabledNames.contains(factory->properties().shortName);
}
