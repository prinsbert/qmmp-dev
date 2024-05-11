/***************************************************************************
 *   Copyright (C) 2024 by Ilya Kotov                                      *
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

#include <QMessageBox>
#include <qmmp/qmmp.h>
#include "xdgglobalshortcuts.h"
#include "xdghotkeyfactory.h"

GeneralProperties XdgHotkeyFactory::properties() const
{
    GeneralProperties properties;
    properties.name = tr("XDG Shortcuts Plugin");
    properties.shortName = "xdghotkey"_L1;
    properties.hasAbout = true;
    properties.hasSettings = false;
    properties.visibilityControl = false;
    return properties;
}

QObject *XdgHotkeyFactory::create(QObject *parent)
{
    return new XdgGlobalShortcuts(parent);
}

QDialog *XdgHotkeyFactory::createSettings(QWidget *parent)
{
    Q_UNUSED(parent);
    return nullptr;
}

void XdgHotkeyFactory::showAbout(QWidget *parent)
{
    QMessageBox::about(parent, tr("About XDG Global Shortcuts Plugin"),
                       tr("XDG Global Shortcuts Plugin for Qmmp") + QChar::LineFeed +
                           tr("This plugin adds global shortcuts support via XDG portal") + QChar::LineFeed +
                           tr("Written by: Ilya Kotov <forkotov02@ya.ru>"));
}

QString XdgHotkeyFactory::translation() const
{
    return QLatin1String(":/xdghotkey_plugin_");
}
