
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

#warning OsDepInfo setting info to blank
#warning Add OsDepInfo for your OS

void OsDepInfo::setDepInfo() 
{
  m_osVersion.clear();
  m_hostname.clear();
}

#endif