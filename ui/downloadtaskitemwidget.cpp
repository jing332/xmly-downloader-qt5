#include "downloadtaskitemwidget.h"

#include <QBoxLayout>

DownloadTaskItemWidget::DownloadTaskItemWidget(const QString &fileName,
                                               QWidget *parent)
    : QWidget(parent) {
  fileNameLabel_ = new QLabel(fileName, this);
  statusLabel_ = new QLabel(this);
  statusLabel_->setStyleSheet("font-size: 10pt; color:green");

  progressBar_ = new QProgressBar(this);
  progressBar_->setVisible(false);
  progressBar_->setFixedHeight(4);
  progressBar_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  progressBar_->setTextVisible(false);

  fileNameLabel_->setStyleSheet("font: bold 10pt");

  auto vLayout = new QVBoxLayout();
  vLayout->setSpacing(5);
  vLayout->setMargin(5);
  vLayout->addWidget(fileNameLabel_);
  vLayout->addWidget(progressBar_);

  auto hLayout = new QHBoxLayout(this);
  hLayout->setSpacing(0);
  hLayout->setMargin(0);
  hLayout->addLayout(vLayout);
  hLayout->addSpacerItem(new QSpacerItem(10, 0, QSizePolicy::Maximum));
  hLayout->addWidget(statusLabel_);
  hLayout->addSpacerItem(new QSpacerItem(5, 0, QSizePolicy::Maximum));
}

void DownloadTaskItemWidget::setProgressBarVisible(bool visible) {
  progressBar_->setVisible(visible);
}

void DownloadTaskItemWidget::SetStatus(const QString &status) {
  statusLabel_->setText(status);
}

void DownloadTaskItemWidget::UpdateProgressBar(int value) {
  progressBar_->setValue(value);
}

void DownloadTaskItemWidget::SetProgressBarVisible(bool visible) {
  progressBar_->setVisible(visible);
}

QString DownloadTaskItemWidget::GetFileNameText() {
  return fileNameLabel_->text();
}
