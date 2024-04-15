/***************************************************************************
 *   Copyright (C) 2006-2016 by Ilya Kotov                                 *
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

#include <QApplication>
#include <QLocale>
#include <QLibraryInfo>
#include <QIcon>
#include <QTranslator>
#include <stdio.h>
#include <stdlib.h>
#ifdef Q_OS_WIN
#include <QtGlobal>
#include <windows.h>
#include <winuser.h>
#endif
#include <qmmp/qmmp.h>
#include "qmmpstarter.h"

#ifdef Q_OS_UNIX
#include <QSettings>
#endif

int main(int argc, char *argv[])
{
#ifdef Q_OS_WIN
    //allows to activate main window from other instances
    AllowSetForegroundWindow(ASFW_ANY);
#endif

#ifdef Q_OS_UNIX
    //using XWayland for skinned user interface
    if(qEnvironmentVariable("XDG_SESSION_TYPE") == QLatin1String("wayland") && !qEnvironmentVariableIsSet("QT_QPA_PLATFORM"))
    {
        QSettings settings(QStringLiteral("qmmp"), QStringLiteral("qmmp"));
        if(settings.value(QStringLiteral("Ui/current_plugin")).toString() == QLatin1String("skinned"))
            qputenv("QT_QPA_PLATFORM", "xcb");
    }
#endif

    QApplication a(argc, argv);
    a.setApplicationName("qmmp");
    QIcon icon;
    icon.addFile(":/16x16/qmmp-1.png");
    icon.addFile(":/32x32/qmmp-1.png");
    icon.addFile(":/48x48/qmmp-1.png");
    icon.addFile(":/56x56/qmmp-1.png");
    icon.addFile(":/64x64/qmmp-1.png");
    icon.addFile(":/128x128/qmmp-1.png");
    icon.addFile(":/256x256/qmmp-1.png");
    icon.addFile(":/scalable/qmmp-1.svgz");
    a.setWindowIcon(icon);

    QTranslator translator;
    QString locale = Qmmp::systemLanguageID();
    if(translator.load(QString(":/qmmp_") + locale))
        a.installTranslator(&translator);

    QTranslator qt_translator;
    if(qt_translator.load(QLibraryInfo::location(QLibraryInfo::TranslationsPath) + "/qtbase_" + locale))
        a.installTranslator(&qt_translator);

    QMMPStarter starter;

    if(starter.isFinished())
        return starter.exitCode();

    a.setQuitOnLastWindowClosed(false);
    return a.exec();
}
