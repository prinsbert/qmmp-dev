/***************************************************************************
 *   Copyright (C) 2014-2019 by Ilya Kotov                                 *
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

#include <QtPlugin>
#include <QLocale>
#include <QApplication>
#include <qmmpui/winfileassoc.h>
#include <qmmp/metadatamanager.h>
#include "uninstalloption.h"

void UninstallOption::registerOprions()
{
    registerOption(0, "--uninstall", tr("Restore the old file associations and clean up the registry"));
    setOptionFlags(0, HIDDEN_FROM_HELP | NO_START);
}

QString UninstallOption::shortName() const
{
     return QLatin1String("UninstallOption");
}

QString UninstallOption::translation() const
{
    return QLatin1String(":/uninstall_plugin_");
}

QString UninstallOption::executeCommand(int id, const QStringList &args)
{
    Q_UNUSED(args);
    if(id == 0)
    {
        WinFileAssoc assoc;
        QStringList regExts, extsToCheck;
        foreach (QString ext, MetaDataManager::instance()->nameFilters())
        {
            ext.remove("*.");
            extsToCheck.append(ext);
        }
        assoc.GetRegisteredExtensions(extsToCheck, regExts);
        assoc.RestoreFileAssociations(regExts);
        assoc.RemoveClassId();
    }
    return QString();
}

Q_EXPORT_PLUGIN2(uninstalloption, UninstallOption)
