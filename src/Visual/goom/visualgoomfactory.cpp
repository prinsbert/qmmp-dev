/***************************************************************************
 *   Copyright (C) 2016 by Ilya Kotov                                      *
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
#include <QTranslator>
#include <QtPlugin>
#include <qmmp/qmmp.h>
#include "visualgoomfactory.h"
#include "goomwidget.h"

const VisualProperties VisualGoomFactory::properties() const
{
    VisualProperties properties;
    properties.name = tr("Goom");
    properties.shortName = "goom";
    properties.hasSettings = false;
    properties.hasAbout = true;
    return properties;
}

Visual *VisualGoomFactory::create(QWidget *parent)
{
    return new GoomWidget(parent);
}

QDialog *VisualGoomFactory::createConfigDialog(QWidget *)
{
    return 0;
}

void VisualGoomFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About Goom Visual Plugin"),
                        tr("Qmmp Goom Visual Plugin")+"\n"+
                        tr("Written by: Ilya Kotov <forkotov02@ya.ru>")+"\n"+
                        tr("Based on the source code from the Goom project")+"\n"+
                        tr("Goom project developers:")+"\n"+
                        tr("Jean-Christophe Hoelt <jeko@ios-software.com>") + "\n"+
                        tr("Guillaume Borios <gyom@ios-software.com>"));
}

QTranslator *VisualGoomFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = Qmmp::systemLanguageID();
    translator->load(QString(":/goom_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN2(goom,VisualGoomFactory)
