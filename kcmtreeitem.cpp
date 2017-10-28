
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

//Local
#include "kcmtreeitem.h"

KcmTreeItem::KcmTreeItem(const KService::Ptr &module, KcmTreeItem *parent) : m_parent(parent), m_module(module),
  m_moduleInfo(new KCModuleInfo(m_module))
{
}

KcmTreeItem::KcmTreeItem() : m_parent(nullptr), m_moduleInfo(new KCModuleInfo())
{
}

KcmTreeItem::~KcmTreeItem()
{
    qDeleteAll(m_children);
    delete m_moduleInfo;
}

void KcmTreeItem::addChild(KcmTreeItem *child)
{
    m_children.append(child);
}

KcmTreeItem *KcmTreeItem::child(const int row)
{
    if(childCount() > row) {
        return m_children.value(row);
    }
    return nullptr;
}

int KcmTreeItem::childCount() const
{
    return m_children.count();
}

int KcmTreeItem::columnCount() const
{
    // Hard coded, menu should never have more than one column
    return 1;
}

KcmTreeItem *KcmTreeItem::parent() const
{
    return m_parent;
}

int KcmTreeItem::row()
{
    if (m_parent) {
        return indexOf(const_cast<KcmTreeItem*>(this));
    }

    return 0;
}

int KcmTreeItem::indexOf(KcmTreeItem *item)
{
    if (m_parent) {
        return m_parent->m_children.indexOf(item);
    }

    return 0;
}

QString KcmTreeItem::data() const
{
    return m_moduleInfo->moduleName();
}

QString KcmTreeItem::category() const
{
    return m_module->property(QStringLiteral("X-KDE-KInfoCenter-Category")).toString().trimmed();
}

KcmTreeItem::itemType KcmTreeItem::type() const
{
    return KCM;
}

KcmTreeItem *KcmTreeItem::containsCategory(const QString& category)
{
    foreach(KcmTreeItem *item, m_children) {
        if(item->type() == CATEGORY) {
            if(item->category().contains(category)) {
                return item;
            }
            if(item->containsCategory(category)) {
                return item;
            }
        }
    }
    return nullptr;
}

KCModuleInfo KcmTreeItem::kcm() const
{
    return *m_moduleInfo;
}

int KcmTreeItem::weight()
{
    return m_moduleInfo->weight();
}

QIcon KcmTreeItem::icon() const
{
    return QIcon::fromTheme(m_moduleInfo->icon());
}

QString KcmTreeItem::whatsThis() const
{
    return m_moduleInfo->comment();
}

bool KcmTreeItem::childrenRegExp(const QRegExp& pattern)
{
    foreach(KcmTreeItem *item, m_children) {
        if(item->keywords().filter(pattern).count() > 0) {
            return true;
        }
    }
    return false;
}

QStringList KcmTreeItem::keywords() const
{
	if(m_moduleInfo->keywords().isEmpty()) {
        return QStringList(data());
    }
	return m_moduleInfo->keywords();
}
