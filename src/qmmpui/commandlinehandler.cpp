/***************************************************************************
 *   Copyright (C) 2008-2025 by Ilya Kotov                                 *
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

#include <qmmp/qmmp.h>
#include "commandlinehandler.h"

QStringList CommandLineHandler::helpString() const
{
    QStringList out;
    for(const CommandLineOption &opt : std::as_const(m_options))
    {
        if(opt.flags & HiddenFromHelp)
            continue;

        if(opt.values.isEmpty())
            out << QStringLiteral("%1||%2").arg(opt.names.join(u", "_s), opt.helpString);
        else
            out << QStringLiteral("%1 <%2>||%3").arg(opt.names.join(u", "_s), opt.values.join(u"> <"_s), opt.helpString);
    }
    return out;
}

QString CommandLineHandler::helpString(int id) const
{
    if(m_options[id].values.isEmpty())
        return  QStringLiteral("%1||%2").arg(m_options[id].names.join(u", "_s), m_options[id].helpString);

    return QStringLiteral("%1 <%2>||%3").arg(m_options[id].names.join(u", "_s), m_options[id].values.join(u"> <"_s), m_options[id].helpString);
}

int CommandLineHandler::identify(const QString &name) const
{
    for(const CommandLineOption &opt : std::as_const(m_options))
    {
        if(opt.names.contains(name))
            return m_options.key(opt);
    }
    return -1;
}

CommandLineHandler::OptionFlags CommandLineHandler::flags(int id) const
{
    return m_options.value(id).flags;
}

void CommandLineHandler::registerOption(int id, const QString &name, const QString &helpString, const QStringList &values)
{
    registerOption(id, QStringList{ name }, helpString, values);
}

void CommandLineHandler::registerOption(int id, const QStringList &names, const QString &helpString, const QStringList &values)
{
    CommandLineOption opt;
    opt.names = names;
    opt.values = values;
    opt.helpString = helpString;
    m_options.insert(id, opt);
}

void CommandLineHandler::setOptionFlags(int id, OptionFlags flags)
{
    m_options[id].flags = flags;
}
