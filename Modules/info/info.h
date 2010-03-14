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
  KInfoListWidget(const KComponentData &inst,const QString &_title, QWidget *parent, bool _getlistbox (QTreeWidget*) = NULL);

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
