/*
  Copyright (c) 1999 Matthias Hoelzer-Kluepfel <hoelzer@kde.org>
  Copyright (c) 2000 Matthias Elter <elter@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef __TOPLEVEL_H__
#define __TOPLEVEL_H__

#include <kxmlguiwindow.h>

class QAction;
class QSplitter;

class KAboutData;

class DockContainer;
class IndexWidget;
//class HelpWidget;
class ConfigModule;
class ConfigModuleList;

class TopLevel : public KXmlGuiWindow
{
  Q_OBJECT

public:
  TopLevel();
  ~TopLevel();

protected:
  void setupActions();

protected slots:
  void activateModule(ConfigModule *);
  void activateGeneral();

  void reportBug();
  void aboutModule();

  void deleteDummyAbout();


private:

  QString handleAmpersand( const QString &modName ) const;

  QSplitter      *_splitter;
  DockContainer  *_dock;

  QAction 		*report_bug, *about_module;

  IndexWidget  *_indextab;
  /*
  HelpWidget   *_helptab;
  */

  ConfigModule     *_active;
  ConfigModuleList *_modules;

  /**
   * if someone wants to report a bug
   * against a module with no about data
   * we construct one for him
   **/
  KAboutData *dummyAbout;
};

#endif
