
/*
 *  progressboxwidget.h
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

#ifndef PROGRESSBOXWIDGET
#define PROGRESSBOXWIDGET

//QT
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QProgressBar>

//KDE
#include <KIcon>

class ProgressBoxWidget : public QFrame
{
  Q_OBJECT
  
  public:
    ProgressBoxWidget();

    void setLabelTitles(const QString &, const QString &);
    void setLabelOne(const QString &);
    void setIcon(const KIcon &icon); 
    void setRange(int, int) const;
    void setValue(int) const;
    
  private:
    void createDisplay(); 
    
    QGridLayout *m_layout;
    QLabel *m_iconLabel;
    
    QLabel *m_info0Label;
    QLabel *m_info0NameLabel;
    
    QLabel *m_info1Label;
    QProgressBar *m_progressBar;
};

#endif //PROGRESSBOXWIDGET
