
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
#include <KIcon>
#include <KDebug>

//QT
#include <QVBoxLayout>
#include <QApplication>
#include <QStyle>

KcmContainer::KcmContainer(QWidget *parent)
	: QScrollArea(parent),
	m_titleLabel(NULL),
	m_centerWidget(NULL),
	m_mod(NULL),
	m_kcmTopEdge(-1)
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
	if (!m_mod && m_centerWidget)
	{
		// we have no module, but a widget set up for one,
		// so just return for now
		return;
	}

	m_mod = NULL; // will be deleted on the line below
	delete m_centerWidget;
	m_centerWidget = new QWidget(this);
	m_centerWidget->setContentsMargins(0,0,0,0);

	QVBoxLayout *centerWidgetLayout = new QVBoxLayout(m_centerWidget);
	centerWidgetLayout->setContentsMargins(0, 0, 0, 0);

	QFont bFont;
	bFont.setBold(true);

	m_titleLabel = new QLabel(m_centerWidget);
	m_titleLabel->setFont(bFont);
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

	setKcmTitle(info);

	m_mod->setWhatsThis(m_mod->quickHelp());
	m_centerWidget->layout()->addWidget(m_mod);
}

void KcmContainer::setKcmTopEdge(int y)
{
	m_kcmTopEdge = y;
	if (m_kcmTopEdge < 0)
	{
		return;
	}

	if (m_titleLabel)
	{
		int spacing = style()->layoutSpacing(QSizePolicy::DefaultType, QSizePolicy::DefaultType, Qt::Vertical);
		m_titleLabel->setMinimumHeight(m_kcmTopEdge - spacing);
	}
}

void KcmContainer::setKcmTitle(const KCModuleInfo &info)
{
	const QString kcmTitle = info.moduleName();
	const QString kcmComment = info.comment();

	m_titleLabel->setText(i18n("%1  ( %2 )",kcmTitle,kcmComment));
}

QString KcmContainer::helpPath() const
{
	if(m_mod == NULL) return QString();
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

QString KcmContainer::exportText() const
{
	return m_mod->realModule()->exportText();
}

QString KcmContainer::name() const
{
	return m_modInfo.moduleName();
}

QString KcmContainer::fileName() const
{
	return m_modInfo.fileName();
}
