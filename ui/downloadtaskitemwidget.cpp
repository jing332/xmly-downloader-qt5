#include "downloadtaskitemwidget.h"

#include <QBoxLayout>

DownloadTaskItemWidget::DownloadTaskItemWidget(const QString &fileName,
                                               const QString &url,
                                               QWidget *parent)
    : QWidget(parent) {
  this->setFont(QFont("Microsoft YaHei", 9));

  fileNameLabel = new QLabel(fileName, this);
  urlLabel = new QLabel(url, this);
  statusLabel = new QLabel("<font color='#0000FF'>正在等待...</font>", this);
  statusLabel->setStyleSheet("font-size: 10pt; color: green");

  fileNameLabel->setStyleSheet("font: bold 10pt");
  fileNameLabel->setFixedHeight(18);
  urlLabel->setStyleSheet("font: 9pt; color: gray");
  urlLabel->setFixedHeight(18);

  auto vLayout = new QVBoxLayout();
  vLayout->setSpacing(5);
  vLayout->setMargin(5);
  vLayout->addWidget(fileNameLabel, 0, Qt::AlignLeft | Qt::AlignTop);
  vLayout->addStrut(5);
  vLayout->addWidget(urlLabel, 0, Qt::AlignLeft | Qt::AlignBottom);

  auto hLauout = new QHBoxLayout(this);
  hLauout->setSpacing(0);
  hLauout->setMargin(0);
  hLauout->addLayout(vLayout);
  hLauout->addWidget(statusLabel, 20, Qt::AlignRight);
  hLauout->addSpacerItem(new QSpacerItem(10, 1, QSizePolicy::Expanding));
}

void DownloadTaskItemWidget::setStatus(const QString &status) {
  this->statusLabel->setText(status);
}
