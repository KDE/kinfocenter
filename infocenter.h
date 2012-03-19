
/*
 *  infocenter.h
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

#ifndef INFOCENTER
#define INFOCENTER

//KDE
#include <KXmlGuiWindow>
#include <KLineEdit>
#include <KAction>
#include <KCModule>
#include <KActionMenu>

//QT
#include <QWidget>
#include <QObject>
#include <QEvent>
#include <QSplitter>
#include <QPushButton>

//Local
#include "sidepanel.h"
#include "kcmcontainer.h"
#include "infokcmmodel.h"
#include "kcmtreeitem.h"
#include "tooltipmanager.h"

class KcmContainer;
class SizePanel;
class ToolTipManager;
class KcmTreeItem;

class KInfoCenter : public KXmlGuiWindow
{
	Q_OBJECT

	public:

		/**
		* Create KInfoCenter main window
		*/
		KInfoCenter();

		/**
		* Destory KInfoCenter object
		*/
		~KInfoCenter();

		/**
		* Event filter to move KCMContainer to be level with
		* the side menu.
		*
		* @param watched object being watched
		* @param event event object containing event information
		*/
		bool eventFilter(QObject *watched, QEvent *event);

		public Q_SLOTS:

		/**
		* Triggered when mouse is clicked on a treeview item
		*
		* @param item item selected via mouse
		*/
		void itemClickedSlot(const KcmTreeItem *item);

		/**
		* Triggered when help toolbar action is used.
		*/
		void helpClickedSlot();

		/**
		* Triggered when export toolbar action is used.
		*/
		void exportClickedSlot();

		/**
		* Triggered when the about KCM action is used
		* in the help menu.
		*/
		void aboutKcmSlot();

		/**
		* Show a error message box
		*
		* @param parent parent object
		* @param errorMessage error message text
		*/
		static void showError(QWidget *parent, const QString& errorMessage);

	private:

		/**
		* Create main window and splitter
		*/
		void createMainFrame();

		/**
		* Create top toolbar on main window and setup toolbar
		* actions.
		*/
		void createToolBar();

		/**
		* Create side menu frame including search bar
		*/
		void createMenuFrame();

		/**
		* Detect if export and help buttons should be enabled
		*
		* @param buttons buttons to check
		*/
		void setButtons(const KCModule::Buttons buttons);

		/**
		* Load KCM into KCMContainer
		*
		* @param kcmItem KCM to be loaded
		*/
		void setKcm(const KcmTreeItem *kcmItem);

		/**
		* Set enable state on KCM About menu action,
		* export toolbar action and help toolbar action.
		*/
		void resetCondition();

		KcmContainer *m_contain;
		QSplitter *m_splitter;

		KAction *m_aboutKcm;
		KAction *m_searchAction;
		KAction *m_exportAction;
		KAction *m_moduleHelpAction;
		KActionMenu *m_helpAction;

		QWidget *m_cWidget;
		KLineEdit *m_searchText;

		SidePanel *m_sideMenu;
		ToolTipManager *m_toolTips;
};

#endif //INFOCENTER
