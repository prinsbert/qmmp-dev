/***************************************************************************
 *   Copyright (C) 2006 by Ilya Kotov                                      *
 *   forkotov02@hotmail.ru                                                 *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef MAINVISUAL_H
#define MAINVISUAL_H

#include <QWidget>
#include <QResizeEvent>
#include <visualization.h>
#include <constants.h>

#include "logscale.h"

class QSettings;
class QTimer;
class Buffer;


class VisualNode
{
public:
    VisualNode(short *l, short *r, unsigned long n, unsigned long o)
            : left(l), right(r), length(n), offset(o)
    {
        // left and right are allocated and then passed to this class
        // the code that allocated left and right should give up all ownership
    }

    ~VisualNode()
    {
        delete [] left;
        delete [] right;
    }

    short *left, *right;
    long length, offset;
};

class VisualBase
{
public:
    virtual ~VisualBase()
    {}
    virtual bool process(VisualNode *node) = 0;
    virtual void draw(QPainter *, const QColor &) = 0;
    virtual void resize(const QSize &size) = 0;
};

class MainVisual : public QWidget, public Visualization
{
    Q_OBJECT

public:
    MainVisual( QWidget *parent = 0);
    virtual ~MainVisual();

    static MainVisual *getPointer();

    VisualBase *visual() const
    {
        return vis;
    }
    void setVisual( VisualBase *newvis );

    void add(Buffer *, unsigned long, int, int);
    void prepare();

    void configChanged(QSettings &settings);

    void paintEvent( QPaintEvent * );
    void resizeEvent( QResizeEvent * );

    static QStringList visuals();

    void setFrameRate( int newfps );
    int frameRate() const
    {
        return fps;
    }

protected:
    void hideEvent ( QHideEvent *);
    void showEvent ( QShowEvent *);

public slots:
    void timeout();

private:
    static MainVisual *pointer;
    VisualBase *vis;
    QPixmap pixmap;
    QList <VisualNode*> nodes;
    QTimer *timer;
    bool playing;
    int fps;
};

class Skin;

class StereoAnalyzer : public VisualBase
{
public:
    StereoAnalyzer();
    virtual ~StereoAnalyzer();


    void resize( const QSize &size );
    bool process( VisualNode *node );
    void draw( QPainter *p, const QColor &back );

protected:
    QColor startColor, targetColor;
    QVector <QRect> rects;
    QVector <double> magnitudes;
    QSize size;
    LogScale scale;
    double scaleFactor, falloff;
    int analyzerBarWidth, fps;
    int intern_vis_data[19];

private:
    Skin *m_skin;
};

#endif
