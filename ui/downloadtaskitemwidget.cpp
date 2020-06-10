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
  statusLabel_->setStyleSheet("font-size: 10pt; color:green");
  progressBar = new QProgressBar(this);
  progressBar->setVisible(false);
  progressBar->setFixedSize(width(), 4);
  progressBar->setFormat("");

  fileNameLabel_->setStyleSheet("font: bold 10pt");
  fileNameLabel_->setFixedHeight(18);
  urlLabel_->setStyleSheet("font: 9pt; color: gray");
  urlLabel_->setFixedHeight(18);

  auto vLayout = new QVBoxLayout();
  vLayout->setSpacing(5);
  vLayout->setMargin(5);
  vLayout->addWidget(fileNameLabel_, 0, Qt::AlignLeft | Qt::AlignTop);
  vLayout->addStrut(5);
  vLayout->addWidget(urlLabel_, 0, Qt::AlignLeft);
  vLayout->addWidget(progressBar, 2, Qt::AlignLeft | Qt::AlignBottom);

  auto hLayout = new QHBoxLayout(this);
  hLayout->setSpacing(0);
  hLayout->setMargin(0);
  hLayout->addLayout(vLayout);
  hLayout->addWidget(statusLabel_, 20, Qt::AlignRight);
  hLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Fixed));
}

void DownloadTaskItemWidget::SetStatus(const QString &status) {
  statusLabel_->setText(status);
}

void DownloadTaskItemWidget::UpdateProgressBar(int value) {
  progressBar->setValue(value);
}

void DownloadTaskItemWidget::SetProgressBarVisible(bool visible) {
  progressBar->setVisible(visible);
}
