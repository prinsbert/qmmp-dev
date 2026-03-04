/***************************************************************************
 *   Copyright (C) 2009-2026 by Ilya Kotov                                 *
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
#ifndef MPLAYERSETTINGSDIALOG_H
#define MPLAYERSETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class MplayerSettingsDialog;
}

/**
    @author Ilya Kotov <forkotov02@ya.ru>
*/
class MplayerSettingsDialog : public QDialog
{
Q_OBJECT
public:
    explicit MplayerSettingsDialog(QWidget *parent = nullptr);

    ~MplayerSettingsDialog();


public slots:
    virtual void accept() override;

private:
    Ui::MplayerSettingsDialog *m_ui;
};

#endif
