/***************************************************************************
 *   Copyright (C) 2006-2023 by Ilya Kotov                                 *
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
#include <QMimeData>
#include <qmmpui/playlistitem.h>
#include <qmmpui/playlistmodel.h>
#include <qmmpui/qmmpuisettings.h>
#include <qmmpui/playlistmanager.h>
#include "listwidget.h"
#include "playlistheader.h"
#include "actionmanager.h"
#include "skin.h"
#include "popupwidget.h"
#include "horizontalslider.h"
#include "playlist.h"

ListWidget::ListWidget(QWidget *parent) : QWidget(parent)
{
    m_popupWidget = nullptr;

    m_skin = Skin::instance();
    m_ui_settings = QmmpUiSettings::instance();
    m_menu = new QMenu(this);
    m_timer = new QTimer(this);
    m_timer->setInterval(50);

    m_header = new PlayListHeader(this);
    m_hslider = new HorizontalSlider(this);

    setAcceptDrops(true);
    setMouseTracking(true);

    readSettings();
    connect(m_skin, &Skin::skinChanged, this, &ListWidget::updateSkin);
    connect(m_ui_settings, SIGNAL(repeatableTrackChanged(bool)), SLOT(updateRepeatIndicator()));
    connect(m_timer, SIGNAL(timeout()), SLOT(autoscroll()));
    connect(m_hslider, SIGNAL(sliderMoved(int)), m_header, SLOT(scroll(int)));
    connect(m_hslider, SIGNAL(sliderMoved(int)), this, SLOT(update()));
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
    settings.beginGroup("Skinned");
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

int ListWidget::firstVisibleLine() const
{
    return m_firstLine;
}

int ListWidget::anchorLine() const
{
    return m_anchorLine;
}

void ListWidget::setAnchorLine(int line)
{
    m_anchorLine = line;
    updateList(PlayListModel::SELECTION);
}

QMenu *ListWidget::menu()
{
    return m_menu;
}

PlayListModel *ListWidget::model()
{
    Q_ASSERT(m_model);
    return m_model;
}

void ListWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    m_drawer.fillBackground(&painter, width(), height());
    painter.setLayoutDirection(Qt::LayoutDirectionAuto);
    bool rtl = (layoutDirection() == Qt::RightToLeft);
    const int linesPerGroup = m_model->linesPerGroup();

    painter.setClipRect(5, 0, width() - 9, height());
    painter.translate(rtl ? m_header->offset() : -m_header->offset(), 0);

    for(int i = 0; i < m_rows.size(); ++i)
    {
        if(m_rows[i]->flags & ListWidgetRow::GROUP)
        {
            if(linesPerGroup == 1)
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
    if(m_dropLine >= 0)
    {
        m_drawer.drawDropLine(&painter, m_dropLine - m_firstLine, width(),
                              m_header->isVisible() ? m_header->height() : 0);
    }
}

void ListWidget::mouseDoubleClickEvent (QMouseEvent *e)
{
    int y = e->position().y();
    int lineIndex = lineAt(y);

    if(lineIndex >= 0)
    {
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

        emit doubleClicked();
        update();
    }
}

void ListWidget::mousePressEvent(QMouseEvent *e)
{
    if(m_popupWidget)
        m_popupWidget->hide();

    const int pressedLine = lineAt(e->position().y());

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
    m_hslider->setGeometry(5,height() - 7, width() - 10, 7);
    updateList(PlayListModel::STRUCTURE);
    QWidget::resizeEvent(e);
}

void ListWidget::wheelEvent(QWheelEvent *e)
{
    if(m_model->lineCount() <= m_row_count)
        return;

    if((m_firstLine == 0 && e->angleDelta().y() > 0) ||
            ((m_firstLine == m_model->lineCount() - m_row_count) && e->angleDelta().y() < 0))
        return;

    m_firstLine -= e->angleDelta().y() / 40;  //40*3 TODO: add step to config
    if(m_firstLine < 0)
        m_firstLine = 0;

    if(m_firstLine > m_model->lineCount() - m_row_count)
        m_firstLine = m_model->lineCount() - m_row_count;

    updateList(PlayListModel::STRUCTURE);
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

    return QWidget::event(e);
}

void ListWidget::updateList(int flags)
{
    m_hslider->setVisible(m_header->maxScrollValue() > 0);
    m_hslider->setPos(m_header->offset(), m_header->maxScrollValue());

    if(updateRowCount())
        flags |= PlayListModel::STRUCTURE;

    if(flags & PlayListModel::CURRENT)
        recenterTo(m_model->currentIndex());

    QList<PlayListItem *> items;

    if(flags & PlayListModel::STRUCTURE || flags & PlayListModel::CURRENT)
    {
        if(m_row_count >= m_model->lineCount())
        {
            m_firstLine = 0;
            emit positionChanged(0,0);
        }
        else if(m_firstLine + m_row_count >= m_model->lineCount())
        {
            //try to restore first visible first
            if((m_lineCount > 0) && (m_lineCount != m_model->lineCount()) && m_firstItem)
            {
                restoreFirstVisible();
            }
            if(m_firstLine + m_row_count >= m_model->lineCount())
                m_firstLine = qMax(0, m_model->lineCount() - m_row_count);
            emit positionChanged(m_firstLine, m_firstLine);
        }
        else if((m_lineCount > 0) && (m_lineCount != m_model->lineCount()) &&
                m_firstItem && m_model->itemAtLine(m_firstLine) != m_firstItem)
        {
            restoreFirstVisible();
            emit positionChanged(m_firstLine, m_model->lineCount() - m_row_count);
        }
        else
        {
            emit positionChanged(m_firstLine, m_model->lineCount() - m_row_count);
        }

        m_firstItem = m_model->isEmpty() ? nullptr : m_model->itemAtLine(m_firstLine);
        m_lineCount = m_model->lineCount();
        items = m_model->itemsAtLines(m_firstLine, m_row_count);

        while(m_rows.count() < qMin(m_row_count, items.count()))
            m_rows << new ListWidgetRow;
        while(m_rows.count() > qMin(m_row_count, items.count()))
            delete m_rows.takeFirst();
    }
    else
    {
        items = m_model->itemsAtLines(m_firstLine, m_row_count);
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

    int trackStateColumn = m_header->trackStateColumn();
    bool rtl = (layoutDirection() == Qt::RightToLeft);

    const int linesPerGroup = m_model->linesPerGroup();

    for(int i = 0; i < items.count(); ++i)
    {
        ListWidgetRow *row = m_rows[i];
        row->autoResize = m_header->hasAutoResizeColumn();
        row->trackStateColumn = trackStateColumn;
        row->subIndex = m_model->subIndexOfLine(m_firstLine + i);

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

        int rect_w = width() + m_header->maxScrollValue() - 10;
        int rect_h = m_drawer.rowHeight() - 1;
        int rect_x = rtl ? (width() - rect_w - 5) : 5;
        int rect_y = (m_header->isVisibleTo(this) ? m_header->height() : 0) + i * m_drawer.rowHeight();


        if((row->flags & ListWidgetRow::GROUP) && linesPerGroup > 1)
        {
            rect_h += (linesPerGroup - 1) * m_drawer.rowHeight();
            rect_y -= row->subIndex * m_drawer.rowHeight();
        }

        row->rect = QRect(rect_x, rect_y, rect_w, rect_h);
        m_drawer.prepareRow(row);  //elide titles
    }
    update();
}

void ListWidget::autoscroll()
{
    SimpleSelection sel = m_model->getSelection(m_model->trackIndexAtLine(m_pressedLine));
    if((sel.top == 0 && m_scroll_direction == TOP && sel.count() > 1) ||
            (sel.bottom == m_model->trackCount() - 1 && m_scroll_direction == DOWN && sel.count() > 1))
        return;

    if(m_scroll_direction == DOWN)
    {
        int line = m_firstLine + m_row_count;
        if(line < m_model->lineCount())
            m_firstLine++;
        m_model->moveItems(m_model->trackIndexAtLine(m_pressedLine), m_model->trackIndexAtLine(line));
        m_pressedLine = line;
    }
    else if(m_scroll_direction == TOP && m_firstLine > 0)
    {
        m_firstLine--;
        m_model->moveItems(m_model->trackIndexAtLine(m_pressedLine), m_model->trackIndexAtLine(m_firstLine));
        m_pressedLine = m_firstLine;
    }

    updateList(PlayListModel::STRUCTURE);
}

void ListWidget::updateRepeatIndicator()
{
    updateList(PlayListModel::CURRENT | PlayListModel::STRUCTURE);
}

void ListWidget::scrollTo(int index)
{
    if(m_row_count)
    {
        recenterTo(index);
        updateList(PlayListModel::STRUCTURE);
    }
}

void ListWidget::setModel(PlayListModel *selected, PlayListModel *previous)
{
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

void ListWidget::setViewPosition(int sc)
{
    if(m_model->lineCount() <= m_row_count)
           return;
    m_firstLine = sc;
    updateList(PlayListModel::STRUCTURE);
}

void ListWidget::updateSkin()
{
    m_drawer.readSettings();
    m_header->readSettings();
    update();
}

void ListWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasFormat("text/uri-list") || event->mimeData()->hasFormat("application/json"))
        event->acceptProposedAction();
}

void ListWidget::dropEvent(QDropEvent *event)
{
    if(event->mimeData()->hasUrls() || event->mimeData()->hasFormat(u"application/json"_s))
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
        else if(event->mimeData()->hasFormat(u"application/json"_s))
        {
            QByteArray json = event->mimeData()->data(u"application/json"_s);
            m_model->insert(index, json);
        }
    }
    m_dropLine = -1;
}

void ListWidget::dragLeaveEvent(QDragLeaveEvent *)
{
    m_dropLine = -1;
    update();
}

void ListWidget::dragMoveEvent(QDragMoveEvent *event)
{
    int index = lineAt(event->position().y());
    if(index < 0)
        index = qMin(m_firstLine + m_row_count, m_model->lineCount());
    if(index != m_dropLine)
    {
        m_dropLine = index;
        update();
    }
    if(event->mimeData()->hasFormat(u"text/uri-list"_s))
        event->acceptProposedAction();
}

const QString ListWidget::getExtraString(PlayListItem *item)
{
    if(item->isGroup())
        return QString();

    QString extra_string;
    PlayListTrack *track = static_cast<PlayListTrack *>(item);

    if (m_show_protocol && track->path().contains(u"://"_s))
        extra_string = QLatin1Char('[') + track->path().split(u"://"_s).constFirst() + QLatin1Char(']');

    if (track->isQueued())
        extra_string += QLatin1Char('|') + QString::number(track->queuedIndex() + 1) + QLatin1Char('|');

    if(m_model->currentTrack() == track && m_ui_settings->isRepeatableTrack())
        extra_string += u"|R|"_s;
    else if(m_model->isStopAfter(track))
        extra_string += u"|S|"_s;

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

void ListWidget::mouseMoveEvent(QMouseEvent *e)
{    
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
                if(sel.top == 0 || sel.top == m_model->trackIndexAtLine(m_firstLine))
                    return;
            }
            else if(sel.count() > 1 && m_scroll_direction == DOWN)
            {
                if(sel.bottom == m_model->trackIndexAtLine(m_model->lineCount() - 1) ||
                        sel.bottom == m_model->trackIndexAtLine(m_firstLine + m_row_count))
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
    for(int i = 0; i < qMin(m_row_count, m_model->lineCount() - m_firstLine); ++i)
    {
        if ((y >= i * m_drawer.rowHeight()) && (y <= (i+1) * m_drawer.rowHeight()))
            return m_firstLine + i;
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
    if(m_row_count && index >= 0)
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
