/***************************************************************************
 *   Copyright (C) 2019 by Ilya Kotov                                      *
 *   forkotov02@ya.ru                                                      *
 *                                                                         *
 *   Based on Amarok 2 Ultimate Lyrics script                              *
 *   Copyright (C) 2009-2010 Vladimir Brkic <vladimir_brkic@yahoo.com>     *
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

#include <QtDebug>
#include <QTextCodec>
#include <QRegExp>
#include <qmmp/trackinfo.h>
#include "lyricsprovider.h"

LyricsProvider::LyricsProvider()
{
    m_charset = QLatin1String("utf-8");
    m_skipRules = false;
}

void LyricsProvider::setName(const QString &name)
{
    m_name = name;
}

void LyricsProvider::setTitle(const QString &title)
{
    m_title = title;
}

void LyricsProvider::setCharset(const QString &charset)
{
    m_charset = charset;
}

void LyricsProvider::setUrl(const QString &url)
{
    m_url = url;
}

void LyricsProvider::addUrlFormat(const QString &replace, const QString &with)
{
    UrlFormat f;
    f.replace = replace;
    f.with = with;
    m_urlFormats << f;
}

void LyricsProvider::addRule(const QList<QPair<QString, QString> > &args, bool exclude)
{
    Rule rule;
    foreach(Arg i, args)
    {
        Item item;
        if(!i.first.isEmpty() && !i.second.isEmpty())
        {
            item.begin = i.first;
            item.end = i.second;
        }
        else if(i.first.contains("://")) //url
        {
            item.url = i.first;
        }
        else
        {
            item.tag = i.first;
        }
        rule << item;
    }
    if(exclude)
        m_excludeRules << rule;
    else
        m_extractRules << rule;
}

void LyricsProvider::addInvalidIndicator(const QString &indicator)
{
    m_invalidIndicators << indicator;
}

QString LyricsProvider::getUrl(const TrackInfo &track) const
{
    QString url = m_url;
    const QMap<QString, QString> replaceMap = generateReplaceMap(track);
    QMap<QString, QString>::const_iterator it = replaceMap.constBegin();
    while(it != replaceMap.constEnd())
    {
        QString value = it.value();

        foreach(const UrlFormat &format, m_urlFormats)
            value.replace(QRegExp(QString("[%1]").arg(QRegExp::escape(format.replace))), format.with);

        url.replace(it.key(), it.value());

        it++;
    }

    return url;
}

QString LyricsProvider::format(const QByteArray &data, const TrackInfo &track) const
{
    QTextCodec *codec = QTextCodec::codecForName(m_charset.toLatin1().constData());
    if(!codec)
        codec = QTextCodec::codecForName("UTF-8");

    const QString content = codec->toUnicode(data);
    QString out;

    foreach(const QString &indicator, m_invalidIndicators)
    {
        if(content.contains(indicator))
            return QString();
    }

    if(m_skipRules)
        return content;

    const QMap<QString, QString> replaceMap = generateReplaceMap(track);

    foreach(const Rule &rule, m_extractRules)
    {
        Rule tmpRule = rule;
        QString tmpContent = content;

        for(int i = 0; i < tmpRule.size(); i++)
        {
            QMap<QString, QString>::const_iterator it = replaceMap.constBegin();
            while(it != replaceMap.constEnd())
            {
                tmpRule[i].begin.replace(it.key(), it.value());
                tmpRule[i].url.replace(it.key(), it.value());
                it++;
            }
        }

        tmpContent = extract(tmpContent, tmpRule);

        if(!tmpContent.isEmpty())
        {
            foreach(const Rule &excludeRule, m_excludeRules)
                tmpContent = exclude(tmpContent, excludeRule);
        }
        if(tmpContent.isEmpty())
        {
            tmpContent = content;
        }
        else
        {
            out = tmpContent;
            break;
        }
    }

    while (out.endsWith("<br />"))
    {
        out.chop(6);
        out = out.trimmed();
    }

    while (out.endsWith("<br>"))
    {
        out.chop(4);
        out = out.trimmed();
    }

    return out;
}

const QString &LyricsProvider::name() const
{
    return m_name;
}

void LyricsProvider::skipRules(bool skip)
{
    m_skipRules = skip;
}

QString LyricsProvider::fixCase(const QString &title) const
{
    QString out;
    QString::const_iterator it = title.constBegin();
    while (it != title.constEnd())
    {
        if(it == title.constBegin() || (it - 1)->isSpace())
            out.append(it->toUpper());
        else
            out.append(*it);

        it++;
    }

    return out;
}

QMap<QString, QString> LyricsProvider::generateReplaceMap(const TrackInfo &track) const
{
    QMap<QString, QString> replaceMap;
    replaceMap.insert("{artist}", track.value(Qmmp::ARTIST).toLower());
    replaceMap.insert("{artist2}", track.value(Qmmp::ARTIST).toLower().remove(' '));
    replaceMap.insert("{Artist}", track.value(Qmmp::ARTIST));
    replaceMap.insert("{ARTIST}", track.value(Qmmp::ARTIST).toUpper());
    replaceMap.insert("{a}", track.value(Qmmp::ARTIST).left(1).toLower());
    replaceMap.insert("{album}", track.value(Qmmp::ALBUM).toLower());
    replaceMap.insert("{album2}", track.value(Qmmp::ALBUM).toLower().remove(' '));
    replaceMap.insert("{Album}", track.value(Qmmp::ALBUM));
    replaceMap.insert("{title}",  track.value(Qmmp::TITLE).toLower());
    replaceMap.insert("{Title}", track.value(Qmmp::TITLE));
    replaceMap.insert("{Title2}", fixCase(track.value(Qmmp::TITLE)));
    replaceMap.insert("{track}", track.value(Qmmp::TRACK));
    replaceMap.insert("{year}",  track.value(Qmmp::YEAR));
    return replaceMap;
}

QString LyricsProvider::extract(const QString &content, const Rule &rule) const
{
    QString out = content;

    foreach(const Item &item, rule)
    {
        if(!item.url.isEmpty())
        {
            QString url = item.url;
            QString id = rule.count() >= 2 ? out.section(rule[1].begin, 1).section(rule[1].end, 0, 0) : QString();
            url.replace("{id}", id);
            return url;
        }
        else if(!item.tag.isEmpty())
        {
            const QRegExp re("<(\\w+).*>");
            re.indexIn(item.tag);
            out = out.section(item.tag, 1).section(QString("</%1>").arg(re.cap(1)), 0, 0);
        }
        else
        {
            out = out.section(item.begin, 1).section(item.end, 0, 0);
        }
    }
    return out.trimmed();
}

QString LyricsProvider::exclude(const QString &content, const LyricsProvider::Rule &rule) const
{
    QString out = content;

    foreach(const Item &item, rule)
    {
        if(!item.tag.isEmpty())
        {
            const QRegExp re("<(\\w+).*>");
            re.indexIn(item.tag);
            out = out.section(item.tag, 0, 0) + out.section(item.tag, 1).section(QString("</%1>").arg(re.cap(1)), 1);
        }
        else
        {
            out = out.section(item.begin, 0, 0) + out.section(item.begin, 1).section(item.end, 1);
        }
    }
    return out.trimmed();
}
