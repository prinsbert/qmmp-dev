/***************************************************************************
 *   Copyright (C) 2010-2025 by Ilya Kotov                                 *
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

#include <QKeyEvent>
#include <QPushButton>
#include "ui_shortcutdialog.h"
#include "shortcutdialog.h"

ShortcutDialog::ShortcutDialog(const QKeySequence &key, QWidget *parent)
        : QDialog(parent), m_ui(new Ui::ShortcutDialog), m_key(key)
{
    m_ui->setupUi(this);
    m_ui->keyLineEdit->setText(key.toString(QKeySequence::NativeText));
    QPushButton *button = m_ui->buttonBox->addButton(tr("Clear"), QDialogButtonBox::ResetRole);
    connect(button, &QPushButton::clicked, m_ui->keyLineEdit, &QLineEdit::clear);
    connect(button, &QPushButton::clicked, this, [this] { m_key = QKeySequence(); });
    connect(this, &QDialog::accepted, this, [this] { releaseKeyboard(); });
    connect(this, &QDialog::rejected, this, [this] { releaseKeyboard(); });
}

ShortcutDialog::~ShortcutDialog()
{
    delete m_ui;
}

void ShortcutDialog::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    switch(key)
    {
    case Qt::Key_Shift:
    case Qt::Key_Control:
    case Qt::Key_Meta:
    case Qt::Key_Alt:
    case Qt::Key_AltGr:
    case Qt::Key_Super_L:
    case Qt::Key_Super_R:
    case Qt::Key_Menu:
    case 0:
    case Qt::Key_unknown:
        m_ui->keyLineEdit->clear();
        m_key = QKeySequence();
        QWidget::keyPressEvent(event);
        return;
    }
    m_key = QKeySequence(event->keyCombination());
    m_ui->keyLineEdit->setText(m_key.toString(QKeySequence::NativeText));
    QWidget::keyPressEvent(event);
}

void ShortcutDialog::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    grabKeyboard();
}

QKeySequence ShortcutDialog::key() const
{
    return m_key;
}
