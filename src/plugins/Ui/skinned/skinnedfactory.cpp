/***************************************************************************
 *   Copyright (C) 2011-2023 by Ilya Kotov                                 *
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
#include <QMessageBox>
#include <QProcess>
#include <QApplication>
#include <QFile>
#include <QFileInfo>
#include <qmmp/qmmpsettings.h>
#include "skinnedmainwindow.h"
#include "skinnedfactory.h"

UiProperties SkinnedFactory::properties() const
{
    UiProperties props;
    props.hasAbout = true;
    props.name = tr("Skinned User Interface");
    props.shortName = "skinned"_L1;
    return props;
}

QObject *SkinnedFactory::SkinnedFactory::create()
{
#ifdef QMMP_WS_X11
    if(qgetenv("XDG_CURRENT_DESKTOP") == "KDE")
    {
        QString kwinScript = Qmmp::dataPath() + u"/scripts/kwin.sh"_s;
        if(!QFile::exists(kwinScript))
            kwinScript = qApp->applicationDirPath() + u"/../src/plugins/Ui/skinned/kwin.sh"_s;
        if(QFile::exists(kwinScript))
        {
            qDebug("SkinnedFactory: adding kwin rules...");
            QProcess::execute(QStringLiteral("sh"), QStringList() << QFileInfo(kwinScript).canonicalFilePath());
        }
    }
#endif
    QmmpSettings::instance()->readEqSettings(EqSettings::EQ_BANDS_10);
    return new SkinnedMainWindow();
}

void SkinnedFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About Qmmp Skinned User Interface"),
                        tr("Qmmp Skinned User Interface")+QChar::LineFeed+
                        tr("Simple user interface with Winamp-2.x/XMMS skins support") + QChar::LineFeed +
                        tr("Written by:")+QChar::LineFeed+
                        tr("Vladimir Kuznetsov <vovanec@gmail.com>")+QChar::LineFeed+
                        tr("Ilya Kotov <forkotov02@ya.ru>")+QChar::LineFeed+
                        tr("Artwork:")+QChar::LineFeed+
                        tr("Andrey Adreev <andreev00@gmail.com>")+QChar::LineFeed+
                        tr("sixsixfive <http://sixsixfive.deviantart.com/>"));

}

QString SkinnedFactory::translation() const
{
    return QLatin1String(":/skinned_plugin_");
}
