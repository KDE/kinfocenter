
/*
 *  kcmtreeitem.h
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

#ifndef KCMTREEITEM
#define KCMTREEITEM

//KDE
#include <KCModuleInfo>
#include <KIcon>

//QT
#include <QList>
#include <QString>

class KcmTreeItem
{
  public:
    /**
    * Tree Item for InfoKcmModel. Holds information about a KCM or 
    * folder object.
    *
    * @param module pointer to KCM module
    * @param parent objects parent
    */
    explicit KcmTreeItem(const KService::Ptr module, KcmTreeItem *parent=0);
    
    /**
    * Tree Item for InfoKcmModel. Holds information about a KCM or 
    * folder object. 
    *
    * @note Used for root folder objects
    *
    * @param category Categories name
    * @param parent objects parent
    */
    explicit KcmTreeItem(QString category, KcmTreeItem *parent=0);
    
    /**
    * Destory KcmTreeItem
    */
    ~KcmTreeItem();
    
    /**
    * Add child tree item to parent
    */
    void addChild(KcmTreeItem *);
    
    /** 
    * Get child tree item
    *
    * @param row row where child is located
    */
    KcmTreeItem *child(int row);
    
    /**
    * Get amount of children
    */
    int childCount();
    
    /**
    * Get parent of current tree item
    */
    KcmTreeItem *parent();
    
    /**
    * Get index of tree item
    */
    int indexOf(KcmTreeItem *);
    
    /** 
    * Get amount of columns that tree item contains.
    * Hardcoded to 1
    */
    int columnCount();
    
    /**
    * Get row of tree item
    */
    int row();
    
    /**
    * Get data of tree item
    */
    QString data();
    
    /**
    * Get category of tree item.
    * Set in X-KDE-KInfoCenter-Category
    */
    QString category();
    
    /**
    * Shows if item holds KCM information 
    */
    bool isValid() const;
    
    /**
    * Check if children of tree item contains a category.
    * Used in search implementation
    */
    KcmTreeItem *containsCategory(QString); 
    
    /**
    * Get tree item KCMs Data
    */
    const KCModuleInfo kcm() const;
    
    /**
    * Get tree items KCM's weight
    */
    int weight();
    
    /**
    * Get icon tied to KCM
    */
    KIcon icon() const;
    
    /**
    * Get whatsThis information from KCM
    */
    QString whatsThis() const;
    
    /**
    * Check if there are any children tree items keywords that
    * have a certain regexp pattern
    */
    bool childrenRegExp(QRegExp pattern);
    
    /**
    * Get KCM tree item keywords
    */
    QStringList keywords();
    
  private:
    
     QList<KcmTreeItem *> m_children;
     KcmTreeItem *m_parent;
     const KService::Ptr m_module;
     
     const QString m_category;
     const KCModuleInfo *m_moduleInfo;
     bool m_isValid;
};

#endif // KCMTREEITEM
