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
#include <QPixmap>
#include <QResizeEvent>
#include <QPainter>
#include <QFont>
#include <QFontMetrics>
#include <QSettings>
#include <QMenu>
#include <QUrl>
#include <QApplication>
#include <QHelpEvent>
#include <QTimer>
#include <QScrollBar>
#include <QMimeData>
#include <qmmpui/playlistitem.h>
#include <qmmpui/playlistmodel.h>
#include <qmmpui/qmmpuisettings.h>
#include <qmmpui/mediaplayer.h>
#include "listwidget.h"
#include "playlistheader.h"
#include "actionmanager.h"
#include "popupwidget.h"

ListWidget::ListWidget(PlayListModel *model, QWidget *parent) : QWidget(parent),
    m_model(model)
{
    setFocusPolicy(Qt::StrongFocus);
    m_ui_settings = QmmpUiSettings::instance();
    m_timer = new QTimer(this);
    m_timer->setInterval(50);
    m_header = new PlayListHeader(this);
    m_scrollBar = new QScrollBar(Qt::Vertical, this);
    m_hslider = new QScrollBar(Qt::Horizontal, this);
    m_hslider->setPageStep(50);

    setAcceptDrops(true);
    setMouseTracking(true);

    readSettings();
    connect(m_ui_settings, SIGNAL(repeatableTrackChanged(bool)), SLOT(updateRepeatIndicator()));
    connect(m_timer, SIGNAL(timeout()), SLOT(autoscroll()));
    connect(m_scrollBar, SIGNAL(valueChanged(int)), SLOT(setViewPosition(int)));
    connect(m_hslider, SIGNAL(valueChanged(int)), m_header, SLOT(scroll(int)));
    connect(m_hslider, SIGNAL(valueChanged(int)), this, SLOT(update()));
    connect(m_model, SIGNAL(scrollToRequest(int)), SLOT(scrollTo(int)));
    connect(m_model, SIGNAL(listChanged(int)), SLOT(updateList(int)));
    connect(m_model, SIGNAL(sortingByColumnFinished(int,bool)), m_header, SLOT(showSortIndicator(int,bool)));
    SET_ACTION(ActionManager::PL_SHOW_HEADER, this, SLOT(readSettings()));
}

ListWidget::~ListWidget()
{
    qDeleteAll(m_rows);
    m_rows.clear();
}

void ListWidget::readSettings()
{
    QSettings settings;
    settings.beginGroup("Simple");
    m_show_protocol = settings.value ("pl_show_protocol", false).toBool();
    bool show_popup = settings.value("pl_show_popup", false).toBool();

    m_header->readSettings();
    m_header->setVisible(ACTION(ActionManager::PL_SHOW_HEADER)->isChecked());
    m_header->setGeometry(0,0,width(), m_header->requiredHeight());

    if (m_update)
    {
        m_drawer.readSettings();
        updateList(PlayListModel::STRUCTURE);
        if(m_popupWidget)
        {
            m_popupWidget->deleteLater();
            m_popupWidget = nullptr;
        }
    }
    else
    {
        m_update = true;
    }

    if(show_popup)
        m_popupWidget = new PlayListPopup::PopupWidget(this);
}

int ListWidget::visibleRows() const
{
    return m_row_count;
}

int ListWidget::firstVisibleIndex() const
{
    return m_firstLine;
}

int ListWidget::anchorIndex() const
{
    return m_anchorLine;
}

void ListWidget::setAnchorIndex(int index)
{
    m_anchorLine = index;
    updateList(PlayListModel::SELECTION);
}

QMenu *ListWidget::menu()
{
    return m_menu;
}

void ListWidget::setMenu(QMenu *menu)
{
    m_menu = menu;
}

PlayListModel *ListWidget::model()
{
    Q_ASSERT(m_model);
    return m_model;
}

bool ListWidget::filterMode() const
{
    return m_filterMode;
}

void ListWidget::setModel(PlayListModel *selected, PlayListModel *previous)
{
    if(m_filterMode)
    {
        m_filterMode = false;
        m_firstLine = 0;
        m_filteredItems.clear();
    }

    if(previous)
    {
        previous->setProperty("first_visible", m_firstLine);
        disconnect(previous, nullptr, this, nullptr); //disconnect previous model
        disconnect(previous, nullptr, m_header, nullptr);
    }
    qApp->processEvents();
    m_model = selected;
    m_lineCount = m_model->lineCount();
    m_firstItem = nullptr;

    if(m_model->property("first_visible").isValid())
    {
        m_firstLine = m_model->property("first_visible").toInt();
        updateList(PlayListModel::STRUCTURE);
    }
    else
    {
        m_firstLine = 0;
        updateList(PlayListModel::STRUCTURE | PlayListModel::CURRENT);
    }
    connect(m_model, SIGNAL(scrollToRequest(int)), SLOT(scrollTo(int)));
    connect(m_model, SIGNAL(listChanged(int)), SLOT(updateList(int)));
    connect(m_model, SIGNAL(sortingByColumnFinished(int,bool)), m_header, SLOT(showSortIndicator(int,bool)));
}

void ListWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    m_drawer.fillBackground(&painter, width(), height());
    painter.setLayoutDirection(Qt::LayoutDirectionAuto);
    bool rtl = (layoutDirection() == Qt::RightToLeft);
    int scroll_bar_width = m_scrollBar->isVisibleTo(this) ? m_scrollBar->sizeHint().width() : 0;

    painter.setClipRect(5, 0, width() - scroll_bar_width - 9, height());
    painter.translate(rtl ? m_header->offset() : -m_header->offset(), 0);

    for(int i = 0; i < m_rows.size(); ++i)
    {
        if(m_rows[i]->flags & ListWidgetRow::GROUP)
        {
            if(m_model->linesPerGroup() == 1)
            {
                m_drawer.drawBackground(&painter, m_rows[i]);
                m_drawer.drawSeparator(&painter, m_rows[i], rtl);
            }
            else if(m_rows[i]->subIndex == 0 || (i == 0 && m_rows[i]->subIndex > 0))
            {
                m_drawer.drawBackground(&painter, m_rows[i]);
                m_drawer.drawMultiLineSeparator(&painter, m_rows[i], rtl);
            }
        }
        else
        {
            m_drawer.drawBackground(&painter, m_rows[i]);
            m_drawer.drawTrack(&painter, m_rows[i], rtl);
        }
    }
    //draw drop line
    if(m_drop_index >= 0)
    {
        m_drawer.drawDropLine(&painter, m_drop_index - m_firstLine, width(),
                              m_header->isVisible() ? m_header->height() : 0);
    }
}

void ListWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
    int y = e->position().y();
    int lineIndex = lineAt(y);

    if(lineIndex >= 0)
    {
        if(m_filterMode)
        {
            m_filterMode = false;
            m_filteredItems.clear();
            scrollTo(lineIndex);
        }

        PlayListItem *item = m_model->itemAtLine(lineIndex);
        if(!item)
            return;

        if(item->isGroup())
        {
            PlayListGroup *group = static_cast<PlayListGroup *>(item);
            m_model->setCurrent(group->tracks().constFirst());
        }
        else
        {
            PlayListTrack *track = static_cast<PlayListTrack *>(item);
            m_model->setCurrent(track);
        }

        MediaPlayer *player = MediaPlayer::instance();
        player->playListManager()->selectPlayList(m_model);
        player->playListManager()->activatePlayList(m_model);
        player->stop();
        player->play();
        emit doubleClicked();
        update();
    }
}

void ListWidget::mousePressEvent(QMouseEvent *e)
{
    if(m_popupWidget)
        m_popupWidget->hide();

    int pressedLine = lineAt(e->position().y());

    if(pressedLine >= 0 && pressedLine < m_model->lineCount())
    {
        m_pressedLine = pressedLine;
        PlayListItem *item = m_model->itemAtLine(pressedLine);

        if(e->button() == Qt::RightButton)
        {
            if(!item->isSelected())
            {
                m_anchorLine = m_pressedLine;
                m_model->clearSelection();
                m_model->setSelected(item);
            }
            if(item->isGroup() && m_model->selectedTracks().isEmpty())
            {
                m_anchorLine = m_pressedLine;
                PlayListGroup *group = static_cast<PlayListGroup *>(item);
                m_model->setSelected(group->tracks());
            }
            QWidget::mousePressEvent(e);
            return;
        }

        if(item->isSelected() && (e->modifiers() == Qt::NoModifier))
        {
            m_select_on_release = true;
            QWidget::mousePressEvent(e);
            return;
        }

        if((Qt::ShiftModifier & e->modifiers()))
        {
            int prevAnchorLine = m_anchorLine;
            m_anchorLine = m_pressedLine;
            m_model->setSelectedLines(m_pressedLine, prevAnchorLine, true);
        }
        else //ShiftModifier released
        {
            m_anchorLine = m_pressedLine;
            if ((Qt::ControlModifier & e->modifiers()))
            {
                m_model->setSelected(item, !item->isSelected());
            }
            else //ControlModifier released
            {
                m_model->clearSelection();
                m_model->setSelected(item);
            }
        }

        update();
    }
    QWidget::mousePressEvent(e);
}

void ListWidget::resizeEvent(QResizeEvent *e)
{
    m_header->setGeometry(0,0,width(), m_header->requiredHeight());
    if(e->oldSize().height() < 10)
        updateList(PlayListModel::STRUCTURE | PlayListModel::CURRENT); //recenter to current on first resize
    else
        updateList(PlayListModel::STRUCTURE);
    QWidget::resizeEvent(e);
}

void ListWidget::wheelEvent(QWheelEvent *e)
{
    if(m_hslider->underMouse())
        return;

    if (m_model->lineCount() <= m_row_count)
        return;
    if ((m_firstLine == 0 && e->angleDelta().y() > 0) ||
            ((m_firstLine == m_model->lineCount() - m_row_count) && e->angleDelta().y() < 0))
        return;
    m_firstLine -= e->angleDelta().y() / 40;  //40*3 TODO: add step to config
    if (m_firstLine < 0)
        m_firstLine = 0;

    if (m_firstLine > m_model->lineCount() - m_row_count)
        m_firstLine = m_model->lineCount() - m_row_count;

    updateList(PlayListModel::STRUCTURE);
}

void ListWidget::showEvent(QShowEvent *)
{
    if(!m_rows.isEmpty())
        updateList(PlayListModel::METADATA);
}

bool ListWidget::event(QEvent *e)
{
    if(m_popupWidget)
    {
        if(e->type() == QEvent::ToolTip)
        {
            QHelpEvent *helpEvent = (QHelpEvent *) e;
            PlayListTrack *track = trackAt(helpEvent->y());
            if(!track)
            {
                m_popupWidget->deactivate();
                return QWidget::event(e);
            }
            e->accept();
            m_popupWidget->prepare(track, helpEvent->globalPos());
            return true;
        }

        if(e->type() == QEvent::Leave)
            m_popupWidget->deactivate();
    }
    if(e->type() == QEvent::StyleChange)
        readSettings();

    return QWidget::event(e);
}

void ListWidget::updateList(int flags)
{
    m_hslider->setRange(0, m_header->maxScrollValue());
    m_hslider->setValue(m_header->offset());
    m_hslider->setVisible(m_header->maxScrollValue() > 0);

    if(updateRowCount())
        flags |= PlayListModel::STRUCTURE;

    if(flags & PlayListModel::STRUCTURE && m_filterMode)
    {
        m_filteredItems = m_model->findTracks(m_filterString);
    }

    if(flags & PlayListModel::CURRENT)
        recenterTo(m_model->currentIndex());

    QList<PlayListItem *> items;
    int count = m_filterMode ? m_filteredItems.count() : m_model->lineCount();

    if(flags & PlayListModel::STRUCTURE || flags & PlayListModel::CURRENT)
    {
        m_scrollBar->blockSignals(true);
        if(m_row_count >= count)
        {
            m_firstLine = 0;
            m_scrollBar->setMaximum(0);
            m_scrollBar->setValue(0);
            emit positionChanged(0,0);
        }
        else if(m_firstLine + m_row_count >= count)
        {
            //try to restore first visible first
            if(!m_filterMode && (m_lineCount > 0) &&
                    (m_lineCount != m_model->lineCount()) && m_firstItem)
            {
                restoreFirstVisible();
            }
            if(m_firstLine + m_row_count >= count)
                m_firstLine = qMax(0, count - m_row_count);
            m_scrollBar->setMaximum(count - m_row_count);
            m_scrollBar->setValue(m_firstLine);
            emit positionChanged(m_firstLine, m_firstLine);
        }
        else if(!m_filterMode && (m_lineCount > 0) && (m_lineCount != m_model->lineCount()) &&
                m_firstItem && m_model->itemAtLine(m_firstLine) != m_firstItem)
        {
            restoreFirstVisible();
            m_scrollBar->setMaximum(count - m_row_count);
            m_scrollBar->setValue(m_firstLine);
            emit positionChanged(m_firstLine, m_model->lineCount() - m_row_count);
        }
        else
        {
            m_scrollBar->setMaximum(count - m_row_count);
            m_scrollBar->setValue(m_firstLine);
            emit positionChanged(m_firstLine, count - m_row_count);
        }
        m_scrollBar->blockSignals(false);

        if(m_filterMode)
        {
            items = m_filteredItems.mid(m_firstLine, m_row_count);
        }
        else
        {
            m_firstItem = m_model->isEmpty() ? nullptr : m_model->itemAtLine(m_firstLine);
            m_lineCount = m_model->lineCount();
            items = m_model->itemsAtLines(m_firstLine, m_row_count);
        }

        while(m_rows.count() < qMin(m_row_count, items.count()))
            m_rows << new ListWidgetRow;
        while(m_rows.count() > qMin(m_row_count, items.count()))
            delete m_rows.takeFirst();

        m_scrollBar->setVisible(count > m_row_count);
    }
    else
    {
        items = m_filterMode ? m_filteredItems.mid(m_firstLine, m_row_count) : m_model->itemsAtLines(m_firstLine, m_row_count);
    }

    if(flags & PlayListModel::STRUCTURE)
        m_header->hideSortIndicator();

    if(flags & PlayListModel::STRUCTURE || flags & PlayListModel::METADATA)
    {
        //song numbers width
        m_drawer.calculateNumberWidth(m_model->trackCount());
        m_drawer.setSingleColumnMode(m_model->columnCount() == 1);
        m_header->setNumberWidth(m_drawer.numberWidth());
    }

    int scroll_bar_width = m_scrollBar->isVisibleTo(this) ? m_scrollBar->sizeHint().width() : 0;
    int trackStateColumn = m_header->trackStateColumn();
    bool rtl = layoutDirection() == Qt::RightToLeft;
    m_header->setScrollBarWidth(scroll_bar_width);

    updateScrollBars();

    for(int i = 0; i < items.count(); ++i)
    {
        ListWidgetRow *row = m_rows[i];
        row->autoResize = m_header->hasAutoResizeColumn();
        row->trackStateColumn = trackStateColumn;
        row->subIndex = m_model->subIndexOfLine(m_firstLine + i);
        row->alternateColor = m_model->alternateColor(m_firstLine + i);

        if(items[i]->isSelected())
            row->flags |= ListWidgetRow::SELECTED;
        else
            row->flags &= ~ListWidgetRow::SELECTED;

        if(i == (m_anchorLine - m_firstLine))
            row->flags |= ListWidgetRow::ANCHOR;
        else
            row->flags &= ~ListWidgetRow::ANCHOR;

        if(flags == PlayListModel::SELECTION)
            continue;

        row->titles = items[i]->formattedTitles();
        row->length = items.at(i)->formattedDuration();
        row->sizes = m_header->sizes();
        row->alignment = m_header->alignment();

        if(items[i] == m_model->currentTrack())
            row->flags |= ListWidgetRow::CURRENT;
        else
            row->flags &= ~ListWidgetRow::CURRENT;

        if(items[i]->isGroup())
        {
            row->flags |= ListWidgetRow::GROUP;
            row->number = -1;
            row->cover = items.at(i)->cover();
            row->length.clear();
        }
        else
        {
            row->flags &= ~ListWidgetRow::GROUP;
            row->number = items.at(i)->trackIndex() + 1;
            row->extraString = getExtraString(items.at(i));
        }

        int rect_w = width() + m_header->maxScrollValue() - 10 - scroll_bar_width;
        int rect_h = m_drawer.rowHeight() - 1;
        int rect_x = rtl ? (width() - rect_w - 5) : 5;
        int rect_y = (m_header->isVisibleTo(this) ? m_header->height() : 0) + i * m_drawer.rowHeight();

        if((row->flags & ListWidgetRow::GROUP) && m_model->linesPerGroup() > 1)
        {
            rect_h += (m_model->linesPerGroup() - 1) * m_drawer.rowHeight();
            rect_y -= row->subIndex * m_drawer.rowHeight();
        }

        row->rect = QRect(rect_x, rect_y, rect_w, rect_h);
        m_drawer.prepareRow(row);  //elide titles
    }
    update();
}

void ListWidget::autoscroll()
{
    if(m_filterMode)
        return;

    /*
    SimpleSelection sel = m_model->getSelection(m_pressedLine);
    if ((sel.m_top == 0 && m_scroll_direction == TOP && sel.count() > 1) ||
        (sel.m_bottom == m_model->lineCount() - 1 && m_scroll_direction == DOWN && sel.count() > 1))
        return;

    if(m_scroll_direction == DOWN)
    {
        int row = m_firstLine + m_row_count;
        (m_firstLine + m_row_count < m_model->lineCount()) ? m_firstLine ++ : m_firstLine;
        //m_model->moveItems(m_pressed_index,row);
        m_pressedLine = row;
    }
    else if(m_scroll_direction == TOP && m_firstLine > 0)
    {
        m_firstLine--;
        //m_model->moveItems(m_pressed_index, m_first);
        m_pressedLine = m_firstLine;
    }
    */
}

void ListWidget::updateRepeatIndicator()
{
    updateList(PlayListModel::CURRENT | PlayListModel::STRUCTURE);
}

void ListWidget::scrollTo(int index)
{
    if (m_row_count && !m_filterMode)
    {
        recenterTo(index);
        updateList(PlayListModel::STRUCTURE);
    }
}

void ListWidget::setViewPosition(int sc)
{
    if (m_model->lineCount() <= m_row_count)
        return;
    m_firstLine = sc;
    updateList(PlayListModel::STRUCTURE);
}

void ListWidget::setFilterString(const QString &str)
{
    m_filterString = str;
    if(str.isEmpty())
    {
        m_filteredItems.clear();
        m_filterString.clear();
        m_filterMode = false;
    }
    else
    {
        m_filterMode = true;
    }
    m_firstLine = 0;
    updateList(PlayListModel::STRUCTURE);
}

void ListWidget::clear()
{
    if(m_filterMode)
    {
        m_model->removeTracks(m_filteredItems);
        m_filteredItems.clear();
    }
    else
    {
        m_model->clear();
    }
}

void ListWidget::removeSelected()
{
    if(m_filterMode)
    {
        QList<PlayListItem *> items;
        for(PlayListItem *item : qAsConst(m_filteredItems))
        {
            if(item->isSelected())
                items << item;
        }
        m_model->removeTracks(items);
    }
    else
    {
        m_model->removeSelected();
    }
}

void ListWidget::removeUnselected()
{
    if(m_filterMode)
    {
        QList<PlayListItem *> items;
        for(PlayListItem *item : m_filteredItems)
        {
            if(!item->isSelected())
                items << item;
        }
        m_model->removeTracks(items);
    }
    else
    {
        m_model->removeUnselected();
    }
}

void ListWidget::updateSkin()
{
    m_drawer.loadSystemColors();
    update();
}

void ListWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasFormat("text/uri-list") || event->mimeData()->hasFormat("application/json"))
        event->acceptProposedAction();
}

void ListWidget::dropEvent(QDropEvent *event)
{
    if(!m_filterMode && (event->mimeData()->hasUrls() || event->mimeData()->hasFormat("application/json")))
    {
        event->acceptProposedAction();
        QApplication::restoreOverrideCursor();

        int index = lineAt(event->position().y());
        if(index < 0)
            index = qMin(m_firstLine + m_row_count, m_model->lineCount());

        if(event->mimeData()->hasUrls())
        {
            QList<QUrl> list_urls = event->mimeData()->urls();
            m_model->insert(index, list_urls);
        }
        else if(event->mimeData()->hasFormat("application/json"))
        {
            QByteArray json = event->mimeData()->data("application/json");
            m_model->insert(index, json);
        }
    }
    m_drop_index = -1;
}

void ListWidget::dragLeaveEvent(QDragLeaveEvent *)
{
    m_drop_index = -1;
    update();
}

void ListWidget::dragMoveEvent(QDragMoveEvent *event)
{
    int index = lineAt(event->position().y());
    if(index < 0)
        index = qMin(m_firstLine + m_row_count, m_model->lineCount());
    if(index != m_drop_index)
    {
        m_drop_index = index;
        update();
    }
    if (event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();
}

const QString ListWidget::getExtraString(PlayListItem *item)
{
    if(item->isGroup())
        return QString();

    QString extra_string;
    PlayListTrack *track = static_cast<PlayListTrack *>(item);

    if (m_show_protocol && track->path().contains("://"))
        extra_string = "[" + track->path().split("://").at(0) + "]";

    if (track->isQueued())
        extra_string += "|"+QString::number(track->queuedIndex() + 1)+"|";

    if(m_model->currentTrack() == track && m_ui_settings->isRepeatableTrack())
        extra_string += "|R|";
    else if(m_model->isStopAfter(track))
        extra_string += "|S|";

    return extra_string.trimmed(); //remove white space
}

bool ListWidget::updateRowCount()
{
    int h = height();
    if(m_header->isVisibleTo(this))
        h -= m_header->requiredHeight();
    if(m_hslider->isVisibleTo(this))
        h -= m_hslider->height();
    int row_count = qMax(0, h / m_drawer.rowHeight());
    if(m_row_count != row_count)
    {
        m_row_count = row_count;
        return true;
    }
    return false;
}

void ListWidget::restoreFirstVisible()
{
    if(m_firstLine < m_model->lineCount() && m_firstItem == m_model->itemAtLine(m_firstLine))
        return;

    int delta = m_model->lineCount() - m_lineCount;

    //try to find and restore first visible line index
    if(delta > 0)
    {
        int from = qMin(m_model->lineCount() - 1, m_firstLine + 1);
        for(int i = from; i <= qMin(m_model->lineCount() - 1, m_firstLine + delta); ++i)
        {
            if(m_model->itemAtLine(i) == m_firstItem)
            {
                m_firstLine = i;
                break;
            }
        }
    }
    else
    {
        int from = qMin(m_model->lineCount() - 1, m_firstLine - 1);
        for(int i = from; i >= qMax(0, m_firstLine + delta); --i)
        {
            if(m_model->itemAtLine(i) == m_firstItem)
            {
                m_firstLine = i;
                break;
            }
        }
    }
}

void ListWidget::updateScrollBars()
{
    bool rtl = layoutDirection() == Qt::RightToLeft;

    int vslider_width = m_scrollBar->isVisibleTo(this) ? m_scrollBar->sizeHint().width() : 0;
    int hslider_height = m_hslider->isVisibleTo(this) ? m_hslider->sizeHint().height() : 0;

    if(rtl)
    {
        m_scrollBar->setGeometry(0, 0, m_scrollBar->sizeHint().width(), height() - hslider_height);
        m_hslider->setGeometry(vslider_width, height() - m_hslider->sizeHint().height(),
                               width() - vslider_width, m_hslider->sizeHint().height());
    }
    else
    {
        m_scrollBar->setGeometry(width() - m_scrollBar->sizeHint().width(), 0,
                                 m_scrollBar->sizeHint().width(), height() - hslider_height);
        m_hslider->setGeometry(0, height() - m_hslider->sizeHint().height(), width() - vslider_width,
                               m_hslider->sizeHint().height());
    }
}

void ListWidget::mouseMoveEvent(QMouseEvent *e)
{
    if(m_filterMode)
        return;

    if(e->buttons() == Qt::LeftButton)
    {
        if (m_prev_y > e->position().y())
            m_scroll_direction = TOP;
        else if (m_prev_y < e->position().y())
            m_scroll_direction = DOWN;
        else
            m_scroll_direction = NONE;

        if(e->position().y() < 0 || e->position().y() > height())
        {
            if(!m_timer->isActive())
                m_timer->start();
            return;
        }
        m_timer->stop();

        int index = lineAt(e->position().y());

        if(index >= 0)
        {
            m_anchorLine = index;
            SimpleSelection sel = m_model->getSelection(m_model->trackIndexAtLine(m_pressedLine));
            if(sel.count() > 1 && m_scroll_direction == TOP)
            {
                if(sel.m_top == 0 || sel.m_top == m_model->trackIndexAtLine(m_firstLine))
                    return;
            }
            else if(sel.count() > 1 && m_scroll_direction == DOWN)
            {
                if(sel.m_bottom == m_model->trackIndexAtLine(m_model->lineCount() - 1) ||
                        sel.m_bottom == m_model->trackIndexAtLine(m_firstLine + m_row_count))
                    return;
            }
            m_model->moveItems(m_model->trackIndexAtLine(m_pressedLine),
                               m_model->trackIndexAtLine(index));

            m_prev_y = e->position().y();
            m_pressedLine = index;
        }
    }
    else if(m_popupWidget)
    {
        PlayListTrack *track = trackAt(e->position().y());
        if(!track || m_popupWidget->url() != track->path())
            m_popupWidget->deactivate();
    }
}

void ListWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if(m_select_on_release && m_pressedLine >= 0)
    {
        m_model->clearSelection();
        PlayListItem *item = m_model->itemAtLine(m_pressedLine);
        m_model->setSelected(item, true);
        m_anchorLine = m_pressedLine;
        m_select_on_release = false;
    }
    m_pressedLine = -1;
    m_scroll_direction = NONE;
    m_timer->stop();
    QWidget::mouseReleaseEvent(e);
}

int ListWidget::lineAt(int y) const
{
    y -= m_header->isVisible() ? m_header->height() : 0;

    if(m_filterMode)
    {
        for(int i = 0; i < qMin(m_row_count, m_filteredItems.count() - m_firstLine); ++i)
        {
            if ((y >= i * m_drawer.rowHeight()) && (y <= (i+1) * m_drawer.rowHeight()))
                return m_model->indexOf(m_filteredItems.at(m_firstLine + i));
        }
    }
    else
    {
        for(int i = 0; i < qMin(m_row_count, m_model->lineCount() - m_firstLine); ++i)
        {
            if ((y >= i * m_drawer.rowHeight()) && (y <= (i+1) * m_drawer.rowHeight()))
                return m_firstLine + i;
        }
    }
    return -1;
}

PlayListTrack *ListWidget::trackAt(int y) const
{
    int line = lineAt(y);
    return line >= 0 ? m_model->trackAtLine(line) : nullptr;
}

void ListWidget::contextMenuEvent(QContextMenuEvent * event)
{
    if (menu())
        menu()->exec(event->globalPos());
}

void ListWidget::recenterTo(int index)
{
    if (m_row_count && index >= 0 && !m_filterMode)
    {
        int line = m_model->findLine(index);
        if (line < 0)
            return;

        if (m_firstLine + m_row_count < line + 1)
            m_firstLine = qMin(m_model->lineCount() - m_row_count, line - m_row_count / 2);
        else if (m_firstLine > line)
            m_firstLine = qMax(line - m_row_count / 2, 0);
    }
}
