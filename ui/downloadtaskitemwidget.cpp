#include "downloadtaskitemwidget.h"

#include <QBoxLayout>

DownloadTaskItemWidget::DownloadTaskItemWidget(const QString &fileName,
                                               const QString &url,
                                               QWidget *parent)
    : QWidget(parent) {
  setFont(QFont("Microsoft YaHei", 9));

  fileNameLabel_ = new QLabel(fileName, this);
  urlLabel_ = new QLabel(url, this);
  statusLabel_ = new QLabel("<font color='#0000FF'>正在等待...</font>", this);
  statusLabel_->setStyleSheet("font-size: 10pt; color: green");

  fileNameLabel_->setStyleSheet("font: bold 10pt");
  fileNameLabel_->setFixedHeight(18);
  urlLabel_->setStyleSheet("font: 9pt; color: gray");
  urlLabel_->setFixedHeight(18);

  auto vLayout = new QVBoxLayout();
  vLayout->setSpacing(5);
  vLayout->setMargin(5);
  vLayout->addWidget(fileNameLabel_, 0, Qt::AlignLeft | Qt::AlignTop);
  vLayout->addStrut(5);
  vLayout->addWidget(urlLabel_, 0, Qt::AlignLeft | Qt::AlignBottom);

  auto hLauout = new QHBoxLayout(this);
  hLauout->setSpacing(0);
  hLauout->setMargin(0);
  hLauout->addLayout(vLayout);
  hLauout->addWidget(statusLabel_, 20, Qt::AlignRight);
  hLauout->addSpacerItem(new QSpacerItem(10, 1, QSizePolicy::Expanding));
}

void DownloadTaskItemWidget::SetStatus(const QString &status) {
  statusLabel_->setText(status);
}
