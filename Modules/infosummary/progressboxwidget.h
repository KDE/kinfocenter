
#ifndef __PROGRESSBOXWIDGET__
#define __PROGRESSBOXWIDGET__

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

#endif // __PROGRESSBOXWIDGET__
