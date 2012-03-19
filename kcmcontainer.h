
/*
 *  kcmcontainer.h
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

#ifndef KCMCONTAINER
#define KCMCONTAINER

//KDE
#include <KCModuleInfo>
#include <KAboutData>
#include <KCModule>
#include <KCModuleProxy>

//QT
#include <QLabel>
#include <QWidget>
#include <QString>
#include <QScrollArea>

class KcmContainer : public QScrollArea
{
	public:

		/**
		* Widget to display and receive information
		* about displayed KCM.
		*
		* @param parent objects parent
		*/
		KcmContainer(QWidget *parent);

		/**
		* Destory KCMContainer
		*/
		~KcmContainer();

		/**
		* Set and show KCM in KCMContainer window
		*/
		void setKcm(const KCModuleInfo &);

		/**
		* Move KCMContainer top edge to a certain position
		*
		* @param y y coordinates of top edge
		*/
		void setKcmTopEdge(int y);

		/**
		* Get currently selected KCMs document path
		* for .desktop
		* X-KDE-DocPath
		*/
		QString helpPath() const;

		/**
		* Get currently selected KCMs
		* information on which buttons need to be shown
		*/
		KCModule::Buttons buttons() const;

		/**
		* Get about data for currently selected KCM
		*/
		const KAboutData *kcmAboutData() const;

		/**
		* Get currently selected KCMs export information
		*/
		QString exportText() const;

		/**
		* Get currently selected KCMs name
		*/
		QString name() const;

		/**
		* Get currently selected KCMs .desktop file name
		*/
		QString fileName() const;

	private:

		/**
		* Init and set KCMContainer Layout
		*/
		void setContainerLayout();

		/**
		* Set top title in KCMContainer
		*/
		void setKcmTitle(const KCModuleInfo &);

		QLabel *m_titleLabel;
		QWidget *m_centerWidget;
		KCModuleProxy *m_mod;
		KCModuleInfo m_modInfo;
		int m_kcmTopEdge;
};

#endif //KCMCONTAINER
