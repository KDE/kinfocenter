
/*
 *  kcmcategoryitem.cpp
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

#include "kcmcategoryitem.h"

KcmCategoryItem::KcmCategoryItem(const KService::Ptr module, KcmTreeItem *parent) :
  KcmTreeItem(module, parent)
{
}

KcmCategoryItem::KcmCategoryItem(const QString& categoryName) :
  KcmTreeItem(),
  m_category(categoryName)
{
}

QString KcmCategoryItem::data() const
{
	if(m_category.isEmpty())
	{
		return m_moduleInfo->moduleName();
	}
	else
	{
		return category();
	}
}

KcmTreeItem::itemType KcmCategoryItem::type() const
{
	return CATEGORY;
}

QString KcmCategoryItem::category() const
{
	if(m_category.isEmpty())
	{
		return m_module->property("X-KDE-KInfoCenter-Category").toString().trimmed();
	}
	else
	{
		return m_category;
	}
}

KCModuleInfo KcmCategoryItem::kcm() const
{
	return KCModuleInfo();
}

int KcmCategoryItem::weight()
{
	return (category().count() + 1000);
}

KIcon KcmCategoryItem::icon() const
{
	if(m_category.isEmpty())
	{
		return KIcon(m_moduleInfo->icon());
	}
	else
	{
		return KIcon("");
	}
}

QString KcmCategoryItem::whatsThis() const
{
	return QString();
}

QStringList KcmCategoryItem::keywords() const
{
	return QStringList();
}
