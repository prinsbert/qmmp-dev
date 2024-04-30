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
#ifndef QSUILISTWIDGET_H
#define QSUILISTWIDGET_H

#include <QWidget>
#include <QDir>
#include <QContextMenuEvent>
#include <QPen>
#include "qsuilistwidgetdrawer.h"

class QFont;
class QFontMetrics;
class QMenu;
class QAction;
class QTimer;
class QScrollBar;
class PlayListModel;
class PlayListItem;
class PlayListTrack;
class QmmpUiSettings;
class QSUiPlayListHeader;
class QSUiPopupWidget;


/**
   @author Ilya Kotov <forkotov02@ya.ru>
*/
class QSUiListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QSUiListWidget(PlayListModel *model, QWidget *parent = nullptr);

    ~QSUiListWidget();

    /*!
     * Returns count of currently visible rows.
     */
    int visibleRows() const;
    /*!
     * Returns index of the first visible line.
     */
    int firstVisibleLine() const;

    int anchorIndex() const;
    void setAnchorIndex(int index);
    QMenu *menu();
    void setMenu(QMenu *menu);
    PlayListModel *model();
    bool filterMode() const;

public slots:
    void setModel(PlayListModel *selected, PlayListModel *previous);
    void readSettings();
    void updateList(int flags);
    void setViewPosition(int sc);
    void setFilterString(const QString &str = QString());
    void clear();
    void removeSelected();
    void removeUnselected();

signals:
    void positionChanged(int, int); //current position, maximum value
    void doubleClicked();

protected:
    void paintEvent(QPaintEvent *) override;
    void mouseDoubleClickEvent(QMouseEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void resizeEvent(QResizeEvent *) override;
    void wheelEvent(QWheelEvent *) override;
    void showEvent(QShowEvent *) override;
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
        NONE = 0,TOP,DOWN
    };

    void recenterTo(int index);
    /*!
     * Returns string with queue number or(and) repeate flag for the \b item.
     */
    const QString getExtraString(PlayListItem *item);
    bool updateRowCount();
    void restoreFirstVisible();
    void updateScrollBars();

    bool m_update = false;
    int m_pressedLine = -1, m_drop_index = -1, m_anchorLine = -1;
    QMenu *m_menu = nullptr;
    PlayListModel *m_model;
    int m_row_count = 0, m_firstLine = 0, m_lineCount = 0; //visible rows, first visible index, total item count
    PlayListItem *m_firstItem = nullptr; //first visible item
    /*!
     * Scroll direction that is performing in current moment.
     */
    ScrollDirection m_scroll_direction = NONE;
    int m_prev_y = 0;
    bool m_select_on_release = false;
    bool m_show_protocol;
    QList<QSUiListWidgetRow *> m_rows;
    QmmpUiSettings *m_ui_settings;
    QSUiPopupWidget *m_popupWidget = nullptr;
    QTimer *m_timer;
    QScrollBar *m_scrollBar;
    QSUiListWidgetDrawer m_drawer;
    QSUiPlayListHeader *m_header;
    QScrollBar *m_hslider;

    QString m_filterString;
    bool m_filterMode = false;
    QList<PlayListItem *> m_filteredItems;
};

#endif
