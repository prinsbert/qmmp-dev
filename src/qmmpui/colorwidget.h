/***************************************************************************
 *   Copyright (C) 2005-2025 by Ilya Kotov                                 *
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
#ifndef COLORWIDGET_H
#define COLORWIDGET_H

#include <QFrame>
#include <QPaintEvent>
#include <QColorDialog>
#include "qmmpui_export.h"

/*! @brief The ColorWidget class is used to display and select color.
 * @author Ilya Kotov <forkotov02@ya.ru>
 */
class QMMPUI_EXPORT ColorWidget : public QFrame
{
    Q_OBJECT
    /*!
     * Color selection dialog options.
     */
    Q_PROPERTY(QColorDialog::ColorDialogOptions options READ options WRITE setOptions NOTIFY optionsChanged)
public:
    /*!
     * Constructor.
     * @param parent Parent object.
     */
    ColorWidget(QWidget *parent = nullptr);
    /*!
     * Destructor.
     */
    ~ColorWidget() = default;
    /*!
     * Returns color dialog options.
     */
    QColorDialog::ColorDialogOptions options() const;
    /*!
     * Sets color dialog options.
     */
    void setOptions(QColorDialog::ColorDialogOptions options);
    /*!
     * Returns color name.
     */
    QString colorName() const;

signals:
    /*!
     * Emitted when the dialog options is changed.
     */
    void optionsChanged();

public slots:
    /*!
     * Sets color name.
     */
    void setColor(const QString &name);

private:
    void mousePressEvent(QMouseEvent *) override final;

    QString m_colorName;
    QColorDialog::ColorDialogOptions m_options;
};

#endif
