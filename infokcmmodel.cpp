
#include "infokcmmodel.h"

InfoKcmModel::InfoKcmModel(QObject *parent) : QAbstractItemModel(parent), m_root(new KcmTreeItem("Information Modules"))
{
  createTreeItems();
}

InfoKcmModel::~InfoKcmModel()
{
  qDeleteAll(m_categories);
}

void InfoKcmModel::createTreeItems() 
{ 
  // Automatically creates categories from X-KDE-KInfoCenter-Category.
  // Category tree item should never have more than one child due to usability reason.
 
  KService::List moduleList = KServiceTypeTrader::self()->query("KCModule", "[X-KDE-ParentApp] == 'kinfocenter'");
 
  foreach(const KService::Ptr &module, moduleList)
  {
    if (module->isType(KST_KService) == true)
    {   
      QString category = module->property("X-KDE-KInfoCenter-Category").toString().trimmed();
      if(!category.isEmpty() || !category.isNull()) 
      {
	if(m_categories.contains(category) == true) 
	{
	  KcmTreeItem *categoryRoot = m_categories.value(category);
	  categoryRoot->addChild(new KcmTreeItem(module,categoryRoot));
	}
	else 
	{
	  KcmTreeItem *newCategory = new KcmTreeItem(category,m_root);
	  m_categories.insert(category,newCategory);
	  
	  m_root->addChild(newCategory);
	  newCategory->addChild(new KcmTreeItem(module,newCategory));
	}
      }
      else
      {
	m_root->addChild(new KcmTreeItem(module,m_root));
      }
    }
  }
  
}

QModelIndex InfoKcmModel::index(int row, int column, const QModelIndex &parent) const
{
  KcmTreeItem *parentItem;

  if (!parent.isValid())
  {
    parentItem = m_root;
  }
  else
  {
    parentItem = static_cast<KcmTreeItem*>(parent.internalPointer());
  }
  
  KcmTreeItem *childItem = parentItem->child(row);
  
  if (childItem)
  {
    return createIndex(row, column, childItem);
  }
  else
  {
    return QModelIndex();
  }
}

QModelIndex InfoKcmModel::parent(const QModelIndex &index) const
{
  if (!index.isValid())
  {
    return QModelIndex();
  }
  
  KcmTreeItem *child = static_cast<KcmTreeItem*>(index.internalPointer());
  KcmTreeItem *parent = child->parent();

  if (parent == m_root)
  {
    return QModelIndex();
  }
  
  return createIndex(parent->row(), 0, parent);
}
    
int InfoKcmModel::rowCount(const QModelIndex &parent) const
{
  KcmTreeItem *parentItem;

  if (!parent.isValid())
  {
    parentItem = m_root;
  }
  else
  {
    parentItem = static_cast<KcmTreeItem*>(parent.internalPointer());
  }
  
  return parentItem->childCount();
}

int InfoKcmModel::columnCount(const QModelIndex &parent) const
{
  // Hard coded, menu should never have more than one column
  
  Q_UNUSED(parent);
  return 1;
}
    
QVariant InfoKcmModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
  {
    return QVariant();
  }
  
  if (role != Qt::DisplayRole)
  {
    return QVariant();
  }
    
  KcmTreeItem *item = static_cast<KcmTreeItem*>(index.internalPointer());
  return item->data();
}

QVariant InfoKcmModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  Q_UNUSED(section);
  
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
  {
    return m_root->data();
  }
  
  return QVariant();
}

Qt::ItemFlags InfoKcmModel::flags(const QModelIndex &index) const
{
  if (!index.isValid())
  {
    return Qt::ItemIsEnabled;
  }
  
  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}
