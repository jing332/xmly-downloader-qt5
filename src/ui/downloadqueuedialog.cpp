#include "downloadqueuedialog.h"

#include <QCloseEvent>
#include <QDebug>
#include <QMessageBox>

#include "appevent.h"
#include "runnables/downloadrunnable.h"
#include "runnables/downloadvipfilerunnable.h"
#include "ui/mainwindow.h"

DownloadQueueDialog::DownloadQueueDialog(const QString &cookie, QWidget *parent)
    : QDialog(parent), ui_(new Ui::DownloadQueueDialog), cookie_(cookie) {
  ui_->setupUi(this);
  setWindowFlag(Qt::WindowContextHelpButtonHint, false);

  pool_ = new QThreadPool(this);
  ui_->progressBar->setValue(0);

  ui_->downloadingListWidget->setEditTriggers(
      QAbstractItemView::NoEditTriggers);
  ui_->downloadFailedListWidget->setEditTriggers(
      QAbstractItemView::NoEditTriggers);
  ui_->downloadFailedListWidget->setSelectionMode(
      QAbstractItemView::ExtendedSelection);

  connect(AppEvent::getInstance(), &AppEvent::UpdateFileLength, this,
          &DownloadQueueDialog::OnUpdateFileLength);
}

DownloadQueueDialog::~DownloadQueueDialog() { delete ui_; }

void DownloadQueueDialog::SetMaxThreadCount(int count) {
  pool_->setMaxThreadCount(count);
}

/*下载免费音频文件*/
void DownloadQueueDialog::DownloadFile(AudioItem *ai) {
  QString filePath;
  if (isAddNum_) {
    filePath = QStringLiteral("%1/%2 %3.%4")
                   .arg(downloadDir_, ai->number, ai->title, suffixName_);
  } else {
    filePath =
        QStringLiteral("%1/%3.%4").arg(downloadDir_, ai->title, suffixName_);
  }

  auto downloadRunnable = new DownloadRunnable(ai->id, ai->url, filePath);
  connect(downloadRunnable, &DownloadRunnable::Start, this,
          &DownloadQueueDialog::OnDownloadStart);
  connect(downloadRunnable, &DownloadRunnable::Finished, this,
          &DownloadQueueDialog::OnDownloadFinished);
  pool_->start(downloadRunnable);

  audioItems_.append(ai);
}

/*下载Vip音频文件*/
void DownloadQueueDialog::DownloadVipFile(AudioItem *ai) {
  QString filePath;
  if (isAddNum_) {
    filePath = QStringLiteral("%1/%2 %3.%4")
                   .arg(downloadDir_, ai->number, ai->title, suffixName_);
  } else {
    filePath =
        QStringLiteral("%1/%3.%4").arg(downloadDir_, ai->title, suffixName_);
  }

  auto dlVipFileRun =
      new DownloadVipFileRunnable(ai->id, cookie_, filePath, ai);

  connect(dlVipFileRun, &DownloadVipFileRunnable::StartGetInfo, this,
          [&](int trackID) {
            auto item = GetDownloadTaskItemWidget(trackID);
            item->setProgressBarVisible(true);
            item->SetStatus("获取下载地址...");
          });

  connect(dlVipFileRun, &DownloadVipFileRunnable::StartDownload, this,
          [&](int trackID, AudioItem *) { OnDownloadStart(trackID); });

  connect(dlVipFileRun, &DownloadVipFileRunnable::Finished, this,
          [&](int trackID) { OnDownloadFinished(trackID, ""); });

  connect(dlVipFileRun, &DownloadVipFileRunnable::DownloadError, this,
          [&](int trackID, AudioItem *ai, const QString &err) {
            qWarning() << err;
            AddDownloadFailedItemWidget(trackID, ai, err);

            auto listItem = downloadingListWidgetItems_.value(trackID);
            ui_->downloadingListWidget->takeItem(
                ui_->downloadingListWidget->row(listItem));
            downloadingListWidgetItems_.remove(trackID);

            SetDownloadFailedCount(downloadFailedCount_ + 1);
          });

  connect(dlVipFileRun, &DownloadVipFileRunnable::GetInfoError, this,
          [&](int trackID, AudioItem *ai, const QString &err) {
            qWarning() << err;
            AddDownloadFailedItemWidget(trackID, ai,
                                        "获取下载地址失败: " + err);

            auto listItem = downloadingListWidgetItems_.value(trackID);
            ui_->downloadingListWidget->takeItem(
                ui_->downloadingListWidget->row(listItem));
            downloadingListWidgetItems_.remove(trackID);

            SetDownloadFailedCount(downloadFailedCount_ + 1);
          });

  pool_->start(dlVipFileRun);
  audioItems_.append(ai);
}

/*开始下载队列中的文件*/
void DownloadQueueDialog::StartDownload(QList<AudioItem *> &aiList,
                                        int maxTaskCount,
                                        const QString &downloadDir,
                                        const QString suffixName,
                                        bool isAddNum) {
  isAddNum_ = isAddNum;
  pool_->setMaxThreadCount(maxTaskCount);
  maxTaskCount_ = maxTaskCount;
  downloadDir_ = downloadDir;
  suffixName_ = suffixName;

  pool_->setMaxThreadCount(maxTaskCount);
  for (auto &ai : aiList) {
    if (QString(ai->url).isEmpty()) {
      DownloadVipFile(ai);
    } else {
      DownloadFile(ai);
    }
  }
}

/*添加下载中ItemWidget*/
void DownloadQueueDialog::AddDownloadingItemWidget(int id,
                                                   const QString &fileName) {
  auto itemWidget = new DownloadTaskItemWidget(fileName);
  auto item = new QListWidgetItem(ui_->downloadingListWidget);
  item->setSizeHint(QSize(0, 45));
  item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
  item->setData(Qt::UserRole, QVariant::fromValue(itemWidget));
  ui_->downloadingListWidget->addItem(item);
  ui_->downloadingListWidget->setItemWidget(item, itemWidget);
  downloadingListWidgetItems_.insert(id, item);
}

void DownloadQueueDialog::AddDownloadFailedItemWidget(int trackId,
                                                      AudioItem *ai,
                                                      const QString &err) {
  Q_UNUSED(trackId);
  auto listWidgetItem = new QListWidgetItem(ai->title);
  listWidgetItem->setToolTip(err);
  listWidgetItem->setData(Qt::UserRole, QVariant::fromValue(ai));
  ui_->downloadFailedListWidget->addItem(listWidgetItem);
}

bool DownloadQueueDialog::HasTask() {
  return ui_->downloadingListWidget->model()->rowCount() > 0;
}

void DownloadQueueDialog::closeEvent(QCloseEvent *event) {
  if (downloadingListWidgetItems_.isEmpty() && downloadFailedCount_ == 0) {
    event->accept();
  } else {
    auto rb = QMessageBox::warning(this, "警告", "是否取消下载?",
                                   QMessageBox::Yes | QMessageBox::No,
                                   QMessageBox::No);
    if (rb == QMessageBox::Yes) {
      pool_->clear();
    } else {
      event->ignore();
    }
  }
}

/*使用id获取条目Widget*/
DownloadTaskItemWidget *DownloadQueueDialog::GetDownloadTaskItemWidget(
    int trackID) {
  if (downloadingListWidgetItems_.contains(trackID)) {
    auto widgetItem = downloadingListWidgetItems_.value(trackID, nullptr);
    return widgetItem->data(Qt::UserRole).value<DownloadTaskItemWidget *>();
  }

  return nullptr;
}

void DownloadQueueDialog::SetDownloadFailedCount(int count) {
  downloadFailedCount_ = count;
  ui_->tabWidget->setTabText(
      1, QStringLiteral("下载失败(%1)").arg(downloadFailedCount_));
}

void DownloadQueueDialog::OnDownloadFinished(int id, const QString &error) {
  /*从 downloadingListWidget 中移除Item*/
  auto listItem = downloadingListWidgetItems_.value(id);
  ui_->downloadingListWidget->takeItem(
      ui_->downloadingListWidget->row(listItem));
  downloadingListWidgetItems_.remove(id);

  int rowCount = ui_->downloadingListWidget->model()->rowCount();
  ui_->tabWidget->setTabText(0, QStringLiteral("正在下载(%1)").arg(rowCount));

  if (error.isEmpty()) {
    qDebug() << QStringLiteral("download finished: {id: %1}").arg(id);
    downloadedCount_++;
    setWindowTitle(
        QStringLiteral("下载管理 (已下载: %1)").arg(downloadedCount_));
  } else {
    qWarning() << QStringLiteral("download fail: {id: %1, reason: %2}")
                      .arg(id)
                      .arg(error);

    /*通过id找到AudioItem并添加到 下载失败列表*/
    for (int i = 0; i < audioItems_.size(); i++) {
      auto &item = audioItems_.at(i);
      if (item->id == id) {
        AddDownloadFailedItemWidget(id, item, error);
        SetDownloadFailedCount(downloadFailedCount_ + 1);
        break;
      }
    }
  }

  /*更新进度条*/
  int completed =
      audioItems_.size() - ui_->downloadingListWidget->model()->rowCount();
  int value = (float)completed / audioItems_.size() * 100.0;

  ui_->progressBar->setValue(value);
  if (value == 100) { /*所有文件下载完成*/
    audioItems_.clear();
    if (ui_->downloadFailedListWidget->model()->rowCount() == 0) {
      this->accept(); /*无下载失败 自动关闭对话框*/
    } else {
      ui_->tabWidget->setCurrentIndex(1);
    }
  }
}

void DownloadQueueDialog::OnDownloadStart(int id) {
  auto &listItem = downloadingListWidgetItems_[id];
  auto variant = listItem->data(Qt::UserRole);
  auto itemWidget = variant.value<DownloadTaskItemWidget *>();
  itemWidget->SetStatus("获取文件\n大小...");
  itemWidget->SetProgressBarVisible(true);
}

void DownloadQueueDialog::OnUpdateFileLength(int id, long *contentLength,
                                             long *currentLength) {
  auto &listItem = downloadingListWidgetItems_[id];
  auto variant = listItem->data(Qt::UserRole);
  auto itemWidget = variant.value<DownloadTaskItemWidget *>();

  itemWidget->UpdateProgressBar(float(*currentLength) / *contentLength * 100.0);
  itemWidget->SetStatus(
      QStringLiteral("(%1MB/%2MB)") /*1024k * 1024k = 1048576k = 1MB*/
          .arg(
              QString::number(double(*currentLength) / double(1048576), 'f', 2))
          .arg(QString::number(double(*contentLength) / double(1048576), 'f',
                               2)));
}

void DownloadQueueDialog::on_downloadFailedListWidget_itemSelectionChanged() {
  if (ui_->downloadFailedListWidget->model()->rowCount() > 0) {
    ui_->retryBtn->setEnabled(true);
  } else {
    ui_->retryBtn->setEnabled(false);
  }
}

void DownloadQueueDialog::on_retryBtn_clicked() {
  /*重新下载选中条目*/
  if (!ui_->downloadFailedListWidget->selectedItems().isEmpty()) {
    ui_->progressBar->setValue(0);

    QList<AudioItem *> selectedItems;
    for (auto &selectedItem : ui_->downloadFailedListWidget->selectedItems()) {
      ui_->downloadFailedListWidget->takeItem(
          ui_->downloadFailedListWidget->row(selectedItem));

      auto variant = selectedItem->data(Qt::UserRole);
      auto ai = variant.value<AudioItem *>();
      selectedItems.append(ai);
      AddDownloadingItemWidget(ai->id, ai->title);
    }

    SetDownloadFailedCount(downloadFailedCount_ - selectedItems.size());
    StartDownload(selectedItems, maxTaskCount_, downloadDir_, suffixName_,
                  isAddNum_);
  }

  if (ui_->downloadFailedListWidget->model()->rowCount() == 0) {
    ui_->retryBtn->setEnabled(false);
  }
}

void DownloadQueueDialog::on_selectAllBtn_clicked() {
  ui_->downloadFailedListWidget->selectAll();
}
