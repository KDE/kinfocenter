
/*
 *  defaultboxwidget.cpp
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

#include "defaultboxwidget.h"

DefaultBoxWidget::DefaultBoxWidget()
{
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
  setObjectName( QLatin1String("OuterFrame" ));
  
  m_layout = new QGridLayout(this);
  m_layout->setAlignment(Qt::AlignTop|Qt::AlignLeft);
  m_layout->setSpacing(10);
  
  createDisplay();
}

void DefaultBoxWidget::createDisplay() 
{
  m_iconLabel = new QLabel();
  m_iconLabel->setAlignment(Qt::AlignCenter);
  m_iconLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
  
  QFont bFont;
  bFont.setBold(true);
  
  m_info0Label = new QLabel();
  m_info0Label->setFont(bFont);
  m_info0NameLabel = new QLabel();
  
  m_info1Label = new QLabel();
  m_info1Label->setFont(bFont);
  m_info1NameLabel = new QLabel();
 
  m_info2Label = new QLabel();
  m_info2Label->setFont(bFont);
  m_info2NameLabel = new QLabel();
  
  m_layout->addWidget(m_iconLabel,1,1,3,1,Qt::AlignCenter);
  
  m_layout->addWidget(m_info0Label,1,2,1,1);
  m_layout->addWidget(m_info0NameLabel,1,3,1,1);
  
  m_layout->addWidget(m_info1Label,2,2,1,1);
  m_layout->addWidget(m_info1NameLabel,2,3,1,1);
  
  m_layout->addWidget(m_info2Label,3,2,1,1);
  m_layout->addWidget(m_info2NameLabel,3,3,1,1);
}

void DefaultBoxWidget::setLabelTitles(const QString &first, const QString &second, const QString &third)
{
  m_info0Label->setText(first);
  m_info1Label->setText(second);
  m_info2Label->setText(third);
}

void DefaultBoxWidget::setIcon(const KIcon &icon) 
{
  m_iconLabel->setPixmap(icon.pixmap(64));
}

void DefaultBoxWidget::setLabelOne(const QString &info) 
{
  m_info0NameLabel->setText(info);
}

void DefaultBoxWidget::setLabelTwo(const QString &info) 
{
  m_info1NameLabel->setText(info);
}

void DefaultBoxWidget::setLabelThree(const QString &info) 
{
  m_info2NameLabel->setText(info);
}
