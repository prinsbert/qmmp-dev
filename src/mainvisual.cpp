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
#include <QTimer>
#include <QSettings>
#include <QPainter>
#include <buffer.h>
#include <constants.h>
#include <output.h>
#include <math.h>

#include "skin.h"
#include "fft.h"
#include "inlines.h"
#include "mainvisual.h"


MainVisual *MainVisual::pointer = 0;

MainVisual *MainVisual::getPointer()
{
    if ( !pointer )
        qFatal ( "MainVisual: this object not created!" );
    return pointer;
}

MainVisual::MainVisual ( QWidget *parent)
        : QWidget ( parent ), vis ( 0 ), playing ( FALSE ), fps ( 20 )
{
    //setVisual(new MonoScope);
    setVisual ( new StereoAnalyzer );
    timer = new QTimer ( this );
    connect ( timer, SIGNAL ( timeout() ), this, SLOT ( timeout() ) );
    timer->setInterval ( 1000 / fps );
    timer->start();
    nodes.clear();
    pointer = this;

}

MainVisual::~MainVisual()
{
    delete vis;
    vis = 0;
}

void MainVisual::setVisual ( VisualBase *newvis )
{
    vis = newvis;
    if ( vis )
        vis->resize ( size() );
}

/*void MainVisual::configChanged ( QSettings &settings )
{
    fps = 20;
    timer->stop();
    timer->start ( 1000 / fps );
    if ( vis )
        vis->configChanged ( settings );
}*/

void MainVisual::prepare()
{
    //nodes.setAutoDelete(TRUE);
    nodes.clear();  //TODO memory leak??
    //nodes.setAutoDelete(FALSE);
}

void MainVisual::add ( Buffer *b, unsigned long w, int c, int p )
{
    if(!timer->isActive ())
        return; 
    long len = b->nbytes, cnt;
    short *l = 0, *r = 0;

    len /= c;
    len /= ( p / 8 );
    if ( len > 512 )
        len = 512;
    //len = 512;
    cnt = len;

    if ( c == 2 )
    {
        l = new short[len];
        r = new short[len];

        if ( p == 8 )
            stereo16_from_stereopcm8 ( l, r, b->data, cnt );
        else if ( p == 16 )
            stereo16_from_stereopcm16 ( l, r, ( short * ) b->data, cnt );
    }
    else if ( c == 1 )
    {
        l = new short[len];

        if ( p == 8 )
            mono16_from_monopcm8 ( l, b->data, cnt );
        else if ( p == 16 )
            mono16_from_monopcm16 ( l, ( short * ) b->data, cnt );
    }
    else
        len = 0;

    nodes.append ( new VisualNode ( l, r, len, w ) );
}

void MainVisual::timeout()
{
    VisualNode *node = 0;

    if ( /*playing &&*/ output() )
    {
        //output()->mutex()->lock ();
        //long olat = output()->latency();
        //long owrt = output()->written();
        //output()->mutex()->unlock();

        //long synctime = owrt < olat ? 0 : owrt - olat;

        mutex()->lock ();
        VisualNode *prev = 0;
        while ( ( !nodes.isEmpty() ) )
        {
            node = nodes.first();
            /*if ( node->offset > synctime )
               break;*/

            if ( prev )
                delete prev;
            nodes.removeFirst();

            prev = node;

        }
        mutex()->unlock();
        node = prev;
    }

    bool stop = TRUE;
    if ( vis )
        stop = vis->process ( node );
    delete node;

    if ( vis )
    {
        pixmap.fill ( Qt::transparent );
        QPainter p ( &pixmap );
        vis->draw ( &p, "Green" );
    }
    else
        pixmap.fill ( "Red" );
    update();
}

void MainVisual::paintEvent ( QPaintEvent * )
{
    QPainter painter ( this );
    painter.drawPixmap ( 0,0,pixmap );
}

void MainVisual::resizeEvent ( QResizeEvent *event )
{
    pixmap = QPixmap ( event->size() );
    if ( vis )
        vis->resize ( size() );
}

void MainVisual::hideEvent ( QHideEvent *)
{
    timer->stop();
}

void MainVisual::showEvent ( QShowEvent *)
{
    timer->start();
}

StereoAnalyzer::StereoAnalyzer()
        : scaleFactor ( 1.0 ), falloff ( 1.0 ), analyzerBarWidth ( 4 ), fps ( 20 )
{
    startColor = Qt::green;
    targetColor = Qt::red;
    falloff = .5;
    for ( int i = 0; i< 19; ++i )
        intern_vis_data[i] = 0;
    m_skin = Skin::getPointer();
}

StereoAnalyzer::~StereoAnalyzer()
{}

void StereoAnalyzer::resize ( const QSize &newsize )
{
    size = newsize;

    scale.setMax ( 192, size.width() / analyzerBarWidth );

    rects.resize ( scale.range() );
    int i = 0, w = 0;
    for ( i = 0; i < rects.count(); i++, w += analyzerBarWidth )
        rects[i].setRect ( w, size.height() / 2, analyzerBarWidth - 1, 1 );

    int os = magnitudes.size();
    magnitudes.resize ( scale.range() * 2 );
    for ( os = 0; os < magnitudes.size(); os++ )
        magnitudes[os] = 0.0;

}

bool StereoAnalyzer::process ( VisualNode *node )
{
    uint i;
    static fft_state *state = 0;
    if ( !state )
        state = fft_init();
    short dest[256];

    const int xscale[] =
        {
            0, 1, 2, 3, 4, 5, 6, 7, 8, 11, 15, 20, 27,
            36, 47, 62, 82, 107, 141, 184, 255
        };

    if ( node )
    {
        i = node->length;
        calc_freq ( dest, node->left );
    }
    else
        return FALSE;
    const double y_scale = 3.60673760222;   /* 20.0 / log(256) */
    int max = 19, y, j;

    for ( int i = 0; i < max; i++ )
    {
        for ( j = xscale[i], y = 0; j < xscale[i + 1]; j++ )
        {
            if ( dest[j] > y )
                y = dest[j];
        }
        y >>= 7;
        if ( y != 0 )
        {
            intern_vis_data[i] = int(log (y) * y_scale);
            //qDebug("%d",y);
            if ( intern_vis_data[i] > 15 )
                intern_vis_data[i] = 15;
            if ( intern_vis_data[i] < 0 )
                intern_vis_data[i] = 0;
        }
    }
    return TRUE;
}

void StereoAnalyzer::draw ( QPainter *p, const QColor &)
{
    p->setPen ( "Cyan" );
    for ( int j= 0; j<19; ++j )
    {
        for ( int i = 0; i<=intern_vis_data[j]; ++i )
        {
            p->setPen ( m_skin->getVisBarColor ( i ) );
            p->drawLine ( j*4,size.height()-i, ( j+1 ) *4-2,size.height()-i );
        }
    }
    for ( int i = 0; i< 19; ++i )
        intern_vis_data[i] = 0;
}
