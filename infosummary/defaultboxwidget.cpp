
#include "defaultboxwidget.h"

DefaultBoxWidget::DefaultBoxWidget()
{
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
  setObjectName("OuterFrame");
  
  m_layout = new QGridLayout(this);
  m_layout->setAlignment(Qt::AlignTop|Qt::AlignLeft);
  m_layout->setSpacing(10);
  
  setStyleSheet("QFrame#OuterFrame { border: 2px solid grey; border-radius: 10px; };");
  
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
