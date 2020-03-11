
/*
 *  kcmcontainer.cpp
 *
 *  Copyright (C) 2010 David Hubner <hubnerd@ntlworld.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "kcmcontainer.h"

//KDE
#include <KLocalizedString>

//QT
#include <QApplication>
#include <QVBoxLayout>
#include <QStyle>

KcmContainer::KcmContainer(QWidget *parent) : QScrollArea(parent), m_titleLabel(nullptr),
    m_centerWidget(nullptr), m_mod(nullptr), m_kcmTopEdge(-1)
{
    setWidgetResizable( true );
    setFrameStyle( QFrame::NoFrame );

    setContainerLayout();
}

KcmContainer::~KcmContainer()
{
    delete m_mod;
}

void KcmContainer::setContainerLayout()
{
    if (!m_mod && m_centerWidget) {
        // we have no module, but a widget set up for one,
        // so just return for now
        return;
    }

    m_mod = nullptr; // will be deleted on the line below
    delete m_centerWidget;
    m_centerWidget = new QWidget(this);
    m_centerWidget->setContentsMargins(0,0,0,0);

    QVBoxLayout *centerWidgetLayout = new QVBoxLayout(m_centerWidget);
    centerWidgetLayout->setContentsMargins(0, 0, 0, 0);

    QFont font;
    // Adapted from KTitleWidget to match KCM qml title size
    font.setPointSize(qRound(QApplication::font().pointSize() * 1.6));

    m_titleLabel = new QLabel(m_centerWidget);
    m_titleLabel->setFont(font);
    m_titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_titleLabel->setContentsMargins(style()->pixelMetric(QStyle::PM_DefaultFrameWidth), 0, 0, 0);
    m_titleLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    centerWidgetLayout->addWidget(m_titleLabel);
    setWidget(m_centerWidget);
    m_centerWidget->setAutoFillBackground(false);
    setKcmTopEdge(m_kcmTopEdge);
}

void KcmContainer::setKcm(const KCModuleInfo &info)
{
    setContainerLayout();

    m_mod = new KCModuleProxy(info);
    m_modInfo = info;

    // Adpated from systemsettings ModuleView::updatePageIconHeader similar hack
    if (!m_mod->realModule()->inherits("KCModuleQml") ) {
        m_centerWidget->setContentsMargins(5,5,5,5);
    }

    setKcmTitle(info);

    m_mod->setWhatsThis(m_mod->quickHelp());
    m_centerWidget->layout()->addWidget(m_mod);
}

void KcmContainer::setKcmTopEdge(int y)
{
    m_kcmTopEdge = y;
    if (m_kcmTopEdge < 0) {
        return;
    }

    if (m_titleLabel) {
        int spacing = style()->layoutSpacing(QSizePolicy::DefaultType, QSizePolicy::DefaultType, Qt::Vertical);
        m_titleLabel->setMinimumHeight(m_kcmTopEdge - spacing);
    }
}

void KcmContainer::setKcmTitle(const KCModuleInfo &info)
{
    //HACK: not much other ways to detect is a qml kcm
    // Adpated from systemsettings ModuleView::updatePageIconHeader similar hack
    if (m_mod->realModule()->inherits("KCModuleQml") ) {
        m_titleLabel->setVisible(false);
    }
    m_titleLabel->setText(info.comment());
}

QString KcmContainer::helpPath() const
{
    if(m_mod == nullptr) {
        return QString();
    }
    return m_modInfo.docPath();
}

KCModule::Buttons KcmContainer::buttons() const
{
    return m_mod->buttons();
}

const KAboutData *KcmContainer::kcmAboutData() const
{
    return m_mod->aboutData();
}

QString KcmContainer::name() const
{
    return m_modInfo.moduleName();
}

QString KcmContainer::fileName() const
{
    return m_modInfo.fileName();
}