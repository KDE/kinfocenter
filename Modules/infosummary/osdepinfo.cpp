
/*
 *  osdepinfo.cpp
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

#include "osdepinfo.h"

OsDepInfo::OsDepInfo() 
{
  setDepInfo();
}

const QString OsDepInfo::hostName() 
{
  return m_hostName;
}

const QString OsDepInfo::osVersion()
{
  return m_osVersion;
}

#if defined(Q_OS_LINUX)
  #include "osdepinfo_linux.cpp"
#else

#ifdef __GNUC__
  #warning OsDepInfo setting info to blank
  #warning Add OsDepInfo for your OS
#endif // __GNUC__

void OsDepInfo::setDepInfo() 
{
  m_osVersion.clear();
  m_hostname.clear();
}

#endif // Q_OS_LINUX

