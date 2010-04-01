
#ifndef __INFOKCMMODEL__
#define __INFOKCMMODEL__

//QT
#include <QAbstractItemModel>
#include <QObject>
#include <QHash>
#include <QString>

//KDE
#include <KService>
#include <KServiceTypeTrader>
#include <KDebug>

//Local
#include "kcmtreeitem.h"

class QAbstractItemModel;
class QObject;

class InfoKcmModel : public QAbstractItemModel 
{
  public:
    InfoKcmModel(QObject *);
    ~InfoKcmModel();
    
    QModelIndex index(int, int, const QModelIndex&) const;
    QModelIndex parent(const QModelIndex&) const;
    
    int rowCount(const QModelIndex&) const;
    int columnCount(const QModelIndex&) const;
    
    QVariant data(const QModelIndex&, int) const;
    QVariant headerData(int, Qt::Orientation, int) const;
    
    Qt::ItemFlags flags(const QModelIndex &) const;
    
  private:    
    void createTreeItems();
    
    QHash<QString, KcmTreeItem *> m_categories;
    KService::List m_moduleList;
    KcmTreeItem *m_root;
};

#endif // __INFOKCMMODEL__