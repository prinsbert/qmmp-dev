/***************************************************************************
 *   Copyright (C) 2008-2024 by Ilya Kotov                                 *
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
#ifndef MODPLUGSETTINGSDIALOG_H
#define MODPLUGSETTINGSDIALOG_H

#include <QDialog>

class QAbstractButton;

namespace Ui {
class ModPlugSettingsDialog;
}

/**
	@author Ilya Kotov <forkotov02@ya.ru>
*/
class ModPlugSettingsDialog : public QDialog
{
Q_OBJECT
public:
    explicit ModPlugSettingsDialog(QWidget *parent = nullptr);

    ~ModPlugSettingsDialog();

private slots:
    void writeSettings();
    void setPreamp(int);
    void on_buttonBox_clicked(QAbstractButton *);

private: 
     Ui::ModPlugSettingsDialog *m_ui;

};

#endif
