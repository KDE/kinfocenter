/*******************************************************************************
 *   Copyright (C) 2008 by Konstantin Heil <konst.heil@stud.uni-heidelberg.de> *
 *                                                                             *
 *   This program is free software; you can redistribute it and/or modify      *
 *   it under the terms of the GNU General Public License as published by      *
 *   the Free Software Foundation; either version 2 of the License, or         *
 *   (at your option) any later version.                                       *
 *                                                                             *
 *   This program is distributed in the hope that it will be useful,           *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 *   GNU General Public License for more details.                              *
 *                                                                             *
 *   You should have received a copy of the GNU General Public License         *
 *   along with this program; if not, write to the                             *
 *   Free Software Foundation, Inc.,                                           *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA                *
 *******************************************************************************/

#include "tooltipmanager.h"

#include "kcmtreeitem.h"
#include "sidepanel.h"

#include "ktooltip.h"

#include <QRect>
#include <QLabel>
#include <QTimer>
#include <QPainter>
#include <QHelpEvent>
#include <QScrollBar>
#include <QGridLayout>
#include <QApplication>
#include <QDesktopWidget>
#include <QAbstractItemView>

#ifdef Q_WS_X11
#include <QX11Info>
#endif

#include <KIcon>
#include <KColorScheme>

class ToolTipManager::Private
{
public:
    Private() :
        view(0),
        timer(0)
        { }

    QAbstractItemView* view;
    QTimer* timer;
    QModelIndex item;
    QRect itemRect;
};

ToolTipManager::ToolTipManager(QAbstractItemView* parent)
    : QObject(parent)
    , d(new ToolTipManager::Private)
{
    d->view = parent;

    connect(parent, SIGNAL(viewportEntered()), this, SLOT(hideToolTip()));
    connect(parent, SIGNAL(entered(const QModelIndex&)), this, SLOT(requestToolTip(const QModelIndex&)));
            
    d->timer = new QTimer(this);
    d->timer->setSingleShot(true);
    connect(d->timer, SIGNAL(timeout()), this, SLOT(prepareToolTip()));

    // When the mousewheel is used, the items don't get a hovered indication
    // (Qt-issue #200665). To assure that the tooltip still gets hidden,
    // the scrollbars are observed.
    connect(parent->horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(hideToolTip()));
    connect(parent->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(hideToolTip()));

    d->view->viewport()->installEventFilter(this);
}

ToolTipManager::~ToolTipManager()
{
    delete d;
}

bool ToolTipManager::eventFilter(QObject* watched, QEvent* event)
{
    if ( watched == d->view->viewport() ) {
        switch ( event->type() ) {
            case QEvent::Leave:
            case QEvent::MouseButtonPress:
                hideToolTip();
                break;
            case QEvent::ToolTip:
                return true;
            default:
                break;
        }
    }
    
    return QObject::eventFilter(watched, event);
}

void ToolTipManager::requestToolTip(const QModelIndex& index)
{
    // only request a tooltip for the name column and when no selection or
    // drag & drop operation is done (indicated by the left mouse button)
    if ( !(QApplication::mouseButtons() & Qt::LeftButton) ) {
        KToolTip::hideTip();
        
        d->itemRect = d->view->visualRect(index);
        const QPoint pos = d->view->viewport()->mapToGlobal(d->itemRect.topLeft());
        d->itemRect.moveTo(pos);
        d->item = index;
        d->timer->start(50);
    } else {
        hideToolTip();
    }
}

void ToolTipManager::hideToolTip()
{
    d->timer->stop();
    KToolTip::hideTip();
}

void ToolTipManager::prepareToolTip()
{
    showToolTip( d->item );
}

void ToolTipManager::showToolTip( QModelIndex menuItem )
{
    if (QApplication::mouseButtons() & Qt::LeftButton) {
        return;
    }
    
    QWidget * tip = createTipContent( menuItem );
    
    // calculate the x- and y-position of the tooltip
    const QSize size = tip->sizeHint();
    const QRect desktop = QApplication::desktop()->screenGeometry( d->itemRect.bottomRight() );
    
    // d->itemRect defines the area of the item, where the tooltip should be
    // shown. Per default the tooltip is shown in the bottom right corner.
    // If the tooltip content exceeds the desktop borders, it must be assured that:
    // - the content is fully visible
    // - the content is not drawn inside d->itemRect
    const bool hasRoomToLeft  = (d->itemRect.left()   - size.width()  >= desktop.left());
    const bool hasRoomToRight = (d->itemRect.right()  + size.width()  <= desktop.right());
    const bool hasRoomAbove   = (d->itemRect.top()    - size.height() >= desktop.top());
    const bool hasRoomBelow   = (d->itemRect.bottom() + size.height() <= desktop.bottom());
    if (!hasRoomAbove && !hasRoomBelow && !hasRoomToLeft && !hasRoomToRight) {
        delete tip;
        tip = 0;
        return;
    }
    
    int x = 0;
    int y = 0;
    if (hasRoomBelow || hasRoomAbove) {
        x = QCursor::pos().x() + 16; // TODO: use mouse pointer width instead of the magic value of 16
        if (x + size.width() >= desktop.right()) {
            x = desktop.right() - size.width();
        }
        y = hasRoomBelow ? d->itemRect.bottom() : d->itemRect.top() - size.height();
    } else {
        Q_ASSERT(hasRoomToLeft || hasRoomToRight);
        x = hasRoomToRight ? d->itemRect.right() : d->itemRect.left() - size.width();
        
        // Put the tooltip at the bottom of the screen. The x-coordinate has already
        // been adjusted, so that no overlapping with d->itemRect occurs.
        y = desktop.bottom() - size.height();
    }
    
    // the ownership of tip is transferred to KToolTip
    KToolTip::showTip(QPoint(x, y), tip);
}

QWidget * ToolTipManager::createTipContent( QModelIndex item )
{
    QWidget * tipContent = new QWidget();
    QGridLayout* tipLayout = new QGridLayout();

    QLayout * primaryLine = generateToolTipLine( &item, tipContent, QSize(32,32), true );
    tipLayout->addLayout( primaryLine, 0, 0 );

    for ( int done = 0; d->view->model()->rowCount( item ) > done; done = 1 + done ) {
        QModelIndex childItem = d->view->model()->index( done, 0, item );
        QLayout * subLine = generateToolTipLine( &childItem, tipContent, QSize(24,24), false );
        tipLayout->addLayout( subLine, done + 2, 0 );
    }

    tipLayout->setVerticalSpacing( 0 );
    tipContent->setLayout( tipLayout );

    if( d->view->model()->rowCount( item ) > 0 ) {
        QLabel * seperatorLine = new QLabel( tipContent );
        tipLayout->addWidget( seperatorLine, 1, 0 );

        int width = tipContent->sizeHint().width();
        QPixmap seperatorPixmap( width, 3 );
        seperatorPixmap.fill( Qt::transparent );

        QPainter painter( &seperatorPixmap );
        KColorScheme paintColors( QPalette::Normal, KColorScheme::Tooltip );
        painter.setBrush( paintColors.foreground() );

        #ifdef Q_WS_X11
        if( QX11Info::isCompositingManagerRunning() ) {
            painter.setRenderHint(QPainter::Antialiasing);
        }
        #endif

        painter.drawLine( 0, 1, width, 1 );
        seperatorLine->setPixmap( seperatorPixmap );
    }

    return tipContent;
}

QLayout * ToolTipManager::generateToolTipLine( QModelIndex * item, QWidget * toolTip, QSize iconSize, bool comment )
{
    SidePanel *sidePanel = static_cast<SidePanel*>(d->view);
    KcmTreeItem *menuItem = static_cast<KcmTreeItem*>( sidePanel->mapProxySource(*item).internalPointer() );
    
    QString text = menuItem->data(); 
    if ( comment ) {
        text = QString( "<b>%1</b>" ).arg( menuItem->data() );
    }

    QLabel * textLabel = new QLabel( toolTip );
    textLabel->setForegroundRole(QPalette::ToolTipText);
    textLabel->setText( text );
    
    // Get icon
    KIcon icon( menuItem->icon() );
    QLabel * iconLabel = new QLabel( toolTip );
    iconLabel->setPixmap( icon.pixmap(iconSize) );
    iconLabel->setMaximumSize( iconSize );
    
    // Generate layout
    QHBoxLayout * layout = new QHBoxLayout();
    layout->addWidget( iconLabel );
    layout->addWidget( textLabel );
    
    return layout;
}

#include "tooltipmanager.moc"
