
#include "osdepinfo.h"

//Linux
#include <sys/utsname.h>

void OsDepInfo::setDepInfo() 
{
  struct utsname linInfo;
  if(uname(&linInfo)) return;
  
  m_osVersion = QString(linInfo.sysname) + " " + QString(linInfo.release);
  m_hostName = QString(linInfo.nodename);
}