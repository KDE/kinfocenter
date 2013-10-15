
/*
Copyright 1998  Helge Deller deller@gmx.de

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License or (at your option) version 3 or any later version
accepted by the membership of KDE e.V. (or its successor approved
by the membership of KDE e.V.), which shall act as a proxy 
defined in Section 14 of version 3 of the license.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _INFO_H_
#define _INFO_H_

#include <QWidget>
#include <QStackedWidget>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QTreeWidget>
#include <QFile>
#include <QEvent>

#include <kcmodule.h>
#include <kaboutdata.h>

#include "os_current.h"

class KInfoListWidget : public KCModule
{
public:
  KInfoListWidget(const QString &_title, QWidget *parent, bool _getlistbox (QTreeWidget*) = NULL);

  virtual void load();
  virtual QString quickHelp() const;

private:
  QTreeWidget* tree;
  bool 		(*getlistbox) (QTreeWidget*);
  QString title;

  QLabel	*noInfoText;
  QString	errorString;
  QStackedWidget  *widgetStack;
};

#endif
