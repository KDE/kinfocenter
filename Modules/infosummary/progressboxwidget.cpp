
#include "progressboxwidget.h"

ProgressBoxWidget::ProgressBoxWidget()
{
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
  setObjectName("OuterFrame");
  
  m_layout = new QGridLayout(this);
  m_layout->setAlignment(Qt::AlignTop|Qt::AlignLeft);
  m_layout->setSpacing(10);
  
  setStyleSheet("QFrame#OuterFrame { border: 2px solid grey; border-radius: 10px; };");
  
  createDisplay();
}

void ProgressBoxWidget::createDisplay() 
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
  m_progressBar = new QProgressBar();
  
  m_layout->addWidget(m_iconLabel,1,1,2,1,Qt::AlignCenter);
  
  m_layout->addWidget(m_info0Label,1,2,1,1);
  m_layout->addWidget(m_info0NameLabel,1,3,1,1);
  
  m_layout->addWidget(m_info1Label,2,2,1,1);
  m_layout->addWidget(m_progressBar,2,3,1,1);
}

void ProgressBoxWidget::setLabelTitles(const QString &first, const QString &second)
{
  m_info0Label->setText(first);
  m_info1Label->setText(second);
}

void ProgressBoxWidget::setIcon(const KIcon &icon) 
{
  m_iconLabel->setPixmap(icon.pixmap(64));
}

void ProgressBoxWidget::setLabelOne(const QString &info) 
{
  m_info0NameLabel->setText(info);
}

void ProgressBoxWidget::setRange(int s, int e) const
{
  m_progressBar->setRange(s,e);
}

void ProgressBoxWidget::setValue(int s) const
{
  m_progressBar->setValue(s);
}
