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

#include "metadataformattermenu.h"
#include "ui_templateeditor.h"
#include "templateeditor.h"

TemplateEditor::TemplateEditor(QWidget *parent) : QDialog(parent), m_ui(new Ui::TemplateEditor)
{
    m_ui->setupUi(this);
    createMenu();
    connect(m_ui->buttonBox, &QDialogButtonBox::clicked, this, [this](QAbstractButton *button) {
        if(m_ui->buttonBox->standardButton(button) == QDialogButtonBox::RestoreDefaults)
            m_ui->textEdit->setPlainText(m_defaultTemplate);
    });
}

TemplateEditor::~TemplateEditor()
{
    delete m_ui;
}

QString TemplateEditor::currentTemplate() const
{
    return m_ui->textEdit->toPlainText ();
}

void TemplateEditor::setTemplate(const QString &text)
{
    m_ui->textEdit->setPlainText(text);
}

void TemplateEditor::setDefaultTemplate(const QString &text)
{
    m_defaultTemplate = text;
}

void TemplateEditor::createMenu()
{
    MetaDataFormatterMenu *menu = new MetaDataFormatterMenu(MetaDataFormatterMenu::TITLE_MENU, this);
    m_ui->insertButton->setMenu(menu);
    connect(menu, &MetaDataFormatterMenu::patternSelected, m_ui->textEdit, &QPlainTextEdit::insertPlainText);
}

QString TemplateEditor::getTemplate(QWidget *parent, const QString &title, const QString &text,
                                    const QString &default_template, bool *ok)
{
    TemplateEditor *editor = new TemplateEditor(parent);
    editor->setWindowTitle(title);
    editor->setTemplate(text);
    editor->setDefaultTemplate(default_template);
    if(editor->exec() == QDialog::Accepted)
    {
        if(ok)
            *ok = true;
        QString t = editor->currentTemplate();
        editor->deleteLater();
        return t;
    }

    if(ok)
        *ok = false;
    editor->deleteLater();
    return QString();
}
