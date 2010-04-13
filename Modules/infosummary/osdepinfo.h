
/*
 *  osdepinfo.h
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

#ifndef __OSDEPINFO__
#define __OSDEPINFO__

#include <QString>

class OsDepInfo
{
  public:
    OsDepInfo();
    
    const QString hostName();
    const QString osVersion();
    
  private:
    void setDepInfo();
    
    QString m_hostName;
    QString m_osVersion;
};

#endif // __OSDEPINFO__
