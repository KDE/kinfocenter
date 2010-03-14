
#ifndef __DEFAULTBOXWIDGET__
#define __DEFAULTBOXWIDGET__

//QT
#include <QFrame>
#include <QGridLayout>
#include <QLabel>

//KDE
#include <KIcon>

class DefaultBoxWidget : public QFrame
{
  Q_OBJECT
 
  public:
    DefaultBoxWidget();

    void setLabelTitles(const QString &, const QString &, const QString &);
    void setLabelOne(const QString &);
    void setLabelTwo(const QString &);
    void setLabelThree(const QString &);
    void setIcon(const KIcon &icon); 
    
  private:
    void createDisplay(); 
    
    QGridLayout *m_layout;
    QLabel *m_iconLabel;
    
    QLabel *m_info0Label;
    QLabel *m_info0NameLabel;
    
    QLabel *m_info1Label;
    QLabel *m_info1NameLabel;
    
    QLabel *m_info2Label;
    QLabel *m_info2NameLabel;
};

#endif // __DEFAULTBOXWIDGET__
