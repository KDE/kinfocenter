
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