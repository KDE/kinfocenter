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

#include <QRect>
#include <QLabel>
#include <QTimer>
#include <QScrollBar>
#include <QGridLayout>
#include <QApplication>
#include <QAbstractItemView>

#include <QIcon>
#include <KColorScheme>
#include <KToolTipWidget>

class ToolTipManager::Private
{
public:
    Private() :
        view(0),
        timer(0),
        delay(50)
        { }

    KToolTipWidget *tooltip;
    QAbstractItemView* view;
    QTimer* timer;
    QModelIndex item;
    QRect itemRect;
    int delay;
};

ToolTipManager::ToolTipManager(QAbstractItemView* parent)
    : QObject(parent)
    , d(new ToolTipManager::Private)
{
    d->view = parent;
    d->tooltip = new KToolTipWidget(d->view);
    d->tooltip->setHideDelay(0);

    connect(parent, &QAbstractItemView::viewportEntered, this, &ToolTipManager::hideToolTip);
    connect(parent, &QAbstractItemView::entered, this, &ToolTipManager::requestToolTip);
            
    d->timer = new QTimer(this);
    d->timer->setSingleShot(true);
    connect(d->timer, &QTimer::timeout, this, &ToolTipManager::prepareToolTip);

    // When the mousewheel is used, the items don't get a hovered indication
    // (Qt-issue #200665). To assure that the tooltip still gets hidden,
    // the scrollbars are observed.
    connect(parent->horizontalScrollBar(), &QAbstractSlider::valueChanged, this, &ToolTipManager::hideToolTip);
    connect(parent->verticalScrollBar(), &QAbstractSlider::valueChanged, this, &ToolTipManager::hideToolTip);

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
        d->tooltip->hide();
        
        d->itemRect = d->view->visualRect(index);
        const QPoint pos = d->view->viewport()->mapToGlobal(d->itemRect.topLeft());
        d->itemRect.moveTo(pos);
        d->item = index;
        d->timer->start(d->delay);
    } else {
        hideToolTip();
    }
}

void ToolTipManager::hideToolTip()
{
    d->timer->stop();
    d->tooltip->hideLater();
}

void ToolTipManager::prepareToolTip()
{
    showToolTip( d->item );
}

void ToolTipManager::showToolTip( const QModelIndex& menuItem )
{
    if (QApplication::mouseButtons() & Qt::LeftButton) {
        return;
    }
    
    QWidget * tip = createTipContent( menuItem );
    connect(d->tooltip, &KToolTipWidget::hidden, tip, &QObject::deleteLater);

    d->tooltip->showBelow(d->itemRect, tip, d->view->nativeParentWidget()->windowHandle());
}

QWidget * ToolTipManager::createTipContent( const QModelIndex& item )
{
    QWidget * tipContent = new QWidget();
    QGridLayout* tipLayout = new QGridLayout();

    QLayout * primaryLine = generateToolTipLine( item, tipContent, QSize(32,32), true );
    tipLayout->addLayout( primaryLine, 0, 0 );

    for ( int done = 0; d->view->model()->rowCount( item ) > done; done = 1 + done ) {
        QModelIndex childItem = d->view->model()->index( done, 0, item );
        QLayout * subLine = generateToolTipLine( childItem, tipContent, QSize(24,24), false );
        tipLayout->addLayout( subLine, done + 2, 0 );
    }

    tipLayout->setVerticalSpacing( 0 );
    tipContent->setLayout( tipLayout );

    if( d->view->model()->rowCount( item ) > 0 ) {
        QFrame * separatorLine = new QFrame( tipContent );
        separatorLine->setFrameStyle( QFrame::HLine );
        tipLayout->addWidget( separatorLine, 1, 0 );
    }

    return tipContent;
}

QLayout * ToolTipManager::generateToolTipLine( const QModelIndex & item, QWidget * toolTip, const QSize& iconSize, bool comment )
{
    SidePanel *sidePanel = static_cast<SidePanel*>(d->view);
    KcmTreeItem *menuItem = static_cast<KcmTreeItem*>( sidePanel->mapToProxySource(item).internalPointer() );
    
    QString text = menuItem->data(); 
    if ( comment ) {
        text = QStringLiteral( "<b>%1</b>" ).arg( menuItem->data() );
    }

    QLabel * textLabel = new QLabel( toolTip );
    textLabel->setForegroundRole(QPalette::ToolTipText);
    textLabel->setText( text );
    
    // Get icon
    QIcon icon( menuItem->icon() );
    QLabel * iconLabel = new QLabel( toolTip );
    iconLabel->setPixmap( icon.pixmap(iconSize) );
    iconLabel->setMaximumSize( iconSize );
    
    // Generate layout
    QHBoxLayout * layout = new QHBoxLayout();
    layout->addWidget( iconLabel );
    layout->addWidget( textLabel );
    
    return layout;
}

