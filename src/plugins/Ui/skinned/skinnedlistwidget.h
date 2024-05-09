/***************************************************************************
 *   Copyright (C) 2006-2024 by Ilya Kotov                                 *
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
#ifndef SKINNEDLISTWIDGET_H
#define SKINNEDLISTWIDGET_H

#include <QWidget>
#include <QDir>
#include <QContextMenuEvent>
#include <QPen>
#include "skinnedlistwidgetdrawer.h"

class QFont;
class QFontMetrics;
class QMenu;
class QAction;
class QTimer;
class PlayListModel;
class Skin;
class PlayListItem;
class PlayListTrack;
class QmmpUiSettings;
class SkinnedPlayListHeader;
class SkinnedHorizontalSlider;
class SkinnedPopupWidget;


/**
   @author Ilya Kotov <forkotov02@ya.ru>
*/
class SkinnedListWidget : public QWidget
{
    Q_OBJECT
public:
    SkinnedListWidget(QWidget *parent = nullptr);

    ~SkinnedListWidget();

    /*!
     * Returns count of currently visible rows.
     */
    int visibleRows() const;
    /*!
     * Returns index of the first visible item.
     */
    int firstVisibleLine() const;

    int anchorLine() const;
    void setAnchorLine(int line);
    QMenu *menu();
    PlayListModel *model();

public slots:
    void readSettings();
    void updateList(int flags);
    void setViewPosition(int sc);
    void setModel(PlayListModel *selected, PlayListModel *previous = nullptr);

signals:
    void doubleClicked();
    void positionChanged(int, int); //current position, maximum value

protected:
    void paintEvent(QPaintEvent *) override;
    void mouseDoubleClickEvent(QMouseEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void resizeEvent(QResizeEvent *) override;
    void wheelEvent(QWheelEvent *) override;
    int lineAt(int y) const;
    PlayListTrack *trackAt(int y) const;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void contextMenuEvent (QContextMenuEvent * event) override;
    bool event(QEvent *e) override;

private slots:
    void updateSkin();
    void autoscroll();
    void updateRepeatIndicator();
    void scrollTo(int index);

private:
    enum ScrollDirection
    {
        NONE = 0, TOP, DOWN
    };

    void recenterTo(int index);
    /*!
     * Returns string with queue number or(and) repeate flag for the item number \b i.
     */
    const QString getExtraString(PlayListItem *item);
    bool updateRowCount();
    void restoreFirstVisible();

    bool m_update = false;
    int m_pressedLine = -1, m_dropLine = -1, m_anchorLine = -1;
    QMenu *m_menu = nullptr;
    PlayListModel *m_model;
    int m_row_count = 0, m_firstLine = 0, m_lineCount = 0; //visible rows, first visible index, total item count
    PlayListItem *m_firstItem = nullptr; //first visible item
    Skin *m_skin;
    /*!
     * Scroll direction that is performing in current moment.
     */
    ScrollDirection m_scroll_direction = NONE;
    int m_prev_y = 0;
    bool m_select_on_release = false;
    bool m_show_protocol = false;
    QList<ListWidgetRow *> m_rows;
    QmmpUiSettings *m_ui_settings;
    SkinnedPopupWidget *m_popupWidget;
    QTimer *m_timer;
    SkinnedListWidgetDrawer m_drawer;
    SkinnedPlayListHeader *m_header;
    SkinnedHorizontalSlider *m_hslider;
};

#endif
