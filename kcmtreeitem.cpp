
#include "kcmtreeitem.h"

KcmTreeItem::KcmTreeItem(const KService::Ptr module, KcmTreeItem *parent) : m_parent(parent), m_module(module), 
  m_moduleInfo(new KCModuleInfo(m_module)), m_isValid(true)
{
}

KcmTreeItem::KcmTreeItem(QString category, KcmTreeItem *parent) : m_parent(parent), m_category(category), 
  m_isValid(false)
{
}

KcmTreeItem::~KcmTreeItem()
{
  qDeleteAll(m_children);
  delete m_parent;
}

void KcmTreeItem::addChild(KcmTreeItem *child) 
{
  m_children.append(child);
}

KcmTreeItem *KcmTreeItem::child(int row)
{
  if(childCount() > row) return m_children.value(row);
  return NULL;
}

int KcmTreeItem::childCount() 
{
    return m_children.count();
}

int KcmTreeItem::columnCount()
{
  // Hard coded, menu should never have more than one column
  
  return 1;
}

KcmTreeItem *KcmTreeItem::parent()
{
  return m_parent;
}

int KcmTreeItem::row()
{
  if (m_parent)
  {
    return indexOf(const_cast<KcmTreeItem*>(this));
  }
  
  return 0;
}

int KcmTreeItem::indexOf(KcmTreeItem *item)
{
  if (m_parent)
  {
    return m_parent->m_children.indexOf(item);
  }
  
  return 0;
}

QString KcmTreeItem::data()
{
  if(isValid() == true)
  {
    return m_moduleInfo->moduleName();
  }
  else 
  {
    return m_category;
  }
}

bool KcmTreeItem::isValid()
{
  return m_isValid;
}

bool KcmTreeItem::containsCategory(QString category) 
{
  //TODO
  return false;
}
