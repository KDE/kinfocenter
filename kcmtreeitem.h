
#ifndef __KCMTREEITEM__
#define __KCMTREEITEM__

//QT
#include <QList>
#include <QString>

//KDE
#include <KCModuleInfo>

class KcmTreeItem
{
  public:
    KcmTreeItem(const KService::Ptr, KcmTreeItem *parent=0);
    KcmTreeItem(QString, KcmTreeItem *parent=0);
    ~KcmTreeItem();
    
    void addChild(KcmTreeItem *);
    KcmTreeItem *child(int row);
    int childCount();
    
    KcmTreeItem *parent();
    int indexOf(KcmTreeItem *);
    
    int columnCount();
    int row();
    
    QString data();
    bool isValid();
    bool containsCategory(QString); 
    
  private:
     QList<KcmTreeItem *> m_children;
     KcmTreeItem *m_parent;
     const KService::Ptr m_module;
     
     const QString m_category;
     const KCModuleInfo *m_moduleInfo;
     bool m_isValid;
};

#endif // __KCMTREEITEM__