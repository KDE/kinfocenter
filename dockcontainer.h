/*
 Copyright (c) 2000 Matthias Elter <elter@kde.org>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

 */

#ifndef __dockcontainer_h__
#define __dockcontainer_h__

#include <QStackedWidget>
#include <QLabel>
#include <QVBoxLayout>

#include <kvbox.h>
#include <ktitlewidget.h>

class AboutWidget;
class ConfigModule;
class ProxyWidget;

class ModuleTitle : public KTitleWidget {
public:
	ModuleTitle(QWidget *parent);
	~ModuleTitle();

	void showTitleFor(ConfigModule *module);
	
private:
	void setCommentText(const QString& docPath, const QString& text, const QString& quickHelp);
	void setCommentBaseText();

};

class DockContainer : public QWidget {
Q_OBJECT

public:
	DockContainer(AboutWidget* aboutWidget, QWidget *parent=NULL);
	virtual ~DockContainer();

	bool dockModule(ConfigModule *module);

public slots:
	void showAboutWidget();

signals:
	void newModule(const QString& name);

private:
	void showBusyWidget();
	void showConfigWidget(ConfigModule* module);

	ProxyWidget* initializeModule(ConfigModule* module);

	QLabel *_busyWidget;
	
	ModuleTitle* _moduleTitle;
	QStackedWidget* _moduleWidgets;
	
	AboutWidget* _generalWidget;
};

#endif
