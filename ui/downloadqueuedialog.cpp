#include "downloadqueuedialog.h"

#include <QCloseEvent>
#include <QDebug>
#include <QMessageBox>

#include "appevent.h"
#include "cgo.h"
#include "runnable/downloadrunnable.h"
#include "ui/mainwindow.h"

DownloadQueueDialog::DownloadQueueDialog(QWidget *parent)
    : QDialog(parent), ui_(new Ui::DownloadQueueDialog) {
  ui_->setupUi(this);

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

void DownloadQueueDialog::DownloadFile(int id, const QString &url,
                                       const QString &fileName) {
  auto downloadRunnable = new DownloadRunnable(
      id, url, downloadDir_ + "/" + fileName + suffixName_);
  connect(downloadRunnable, &DownloadRunnable::Start, this,
          &DownloadQueueDialog::DownloadStart);
  connect(downloadRunnable, &DownloadRunnable::Finished, this,
          &DownloadQueueDialog::DownloadFinished);

  pool_->start(downloadRunnable);
}

void DownloadQueueDialog::DownloadFile(AudioItem *audioItem) {
  DownloadFile(audioItem->id, audioItem->url, audioItem->title);
  audioItems_.append(audioItem);
}

double scale = 0;
void DownloadQueueDialog::StartDownload(QList<AudioItem *> &audioItems,
                                        int maxTaskCount,
                                        const QString &downloadDir,
                                        const QString suffixName) {
  pool_->setMaxThreadCount(maxTaskCount);
  maxTaskCount_ = maxTaskCount;
  downloadDir_ = downloadDir;
  suffixName_ = suffixName;

  pool_->setMaxThreadCount(maxTaskCount);
  for (auto &audioItem : audioItems) {
    DownloadFile(audioItem);
  }
  scale = double(100) / double(audioItems.size());
}
void DownloadQueueDialog::AddItemWidget(int id, const QString &url,
                                        const QString &fileName) {
  auto itemWidget = new DownloadTaskItemWidget(fileName, url);
  auto item = new QListWidgetItem(ui_->downloadingListWidget);
  item->setSizeHint(QSize(0, 50));
  item->setData(Qt::UserRole, QVariant::fromValue(itemWidget));
  ui_->downloadingListWidget->addItem(item);
  ui_->downloadingListWidget->setItemWidget(item, itemWidget);
  downloadingListWidgetItems_.insert(id, item);
  qDebug() << item->sizeHint();
}

bool DownloadQueueDialog::HasTask() {
  return ui_->downloadingListWidget->model()->rowCount() > 0;
}

void DownloadQueueDialog::closeEvent(QCloseEvent *event) {
  auto rb = QMessageBox::warning(
      this, "警告", "是否暂停下载并删除所有任务(不包括已下载文件)?",
      QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
  if (rb == QMessageBox::Yes) {
    pool_->clear();
  } else {
    event->ignore();
  }
}

void DownloadQueueDialog::DownloadFinished(int id, const QString &error) {
  /*从 downloadingListWidget 中移除Item*/
  auto listItem = downloadingListWidgetItems_.value(id);
  ui_->downloadingListWidget->takeItem(
      ui_->downloadingListWidget->row(listItem));
  downloadingListWidgetItems_.remove(id);

  if (error.isEmpty()) {
    qDebug() << QStringLiteral("download finished: {id: %1}").arg(id);
  } else {
    qWarning() << QStringLiteral("download fail: {id: %1, reason: %2}")
                      .arg(id)
                      .arg(error);

    /*通过id找到AudioItem并添加到 下载失败列表*/
    for (int i = 0; i < audioItems_.size(); i++) {
      auto &item = audioItems_.at(i);
      if (item->id == id) {
        auto listWidgetItem = new QListWidgetItem(item->title);
        listWidgetItem->setToolTip(error);
        listWidgetItem->setData(Qt::UserRole, QVariant::fromValue(item));
        ui_->downloadFailedListWidget->addItem(listWidgetItem);

        downloadFailedCount++;
        ui_->tabWidget->setTabText(
            1, QStringLiteral("下载失败(%1)").arg(downloadFailedCount));

        break;
      }
    }
  }

  /*更新进度条*/
  int completed =
      audioItems_.size() - ui_->downloadingListWidget->model()->rowCount();
  int value = completed * scale;
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

void DownloadQueueDialog::DownloadStart(int id) {
  auto &listItem = downloadingListWidgetItems_[id];
  auto variant = listItem->data(Qt::UserRole);
  auto itemWidget = variant.value<DownloadTaskItemWidget *>();
  itemWidget->SetStatus("获取文件\n大小...");
  itemWidget->SetProgressBarVisible(true);
}

void DownloadQueueDialog::OnUpdateFileLength(int id, long contentLength,
                                             long currentLength) {
  auto &listItem = downloadingListWidgetItems_[id];
  auto variant = listItem->data(Qt::UserRole);
  auto itemWidget = variant.value<DownloadTaskItemWidget *>();

  itemWidget->UpdateProgressBar(
      int((double(100) / contentLength) * currentLength));
  itemWidget->SetStatus(
      QStringLiteral("(%1MB/%2MB)") /*1024k * 1024k = 1048576k = 1MB*/
          .arg(QString::number(double(currentLength) / double(1048576), 'f', 2))
          .arg(QString::number(double(contentLength) / double(1048576), 'f',
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
      auto audioItem = variant.value<AudioItem *>();
      selectedItems.append(audioItem);
      AddItemWidget(audioItem->id, audioItem->url, audioItem->title);
    }

    downloadFailedCount -= selectedItems.size();
    ui_->tabWidget->setTabText(
        1, QStringLiteral("下载失败(%1)").arg(downloadFailedCount));

    StartDownload(selectedItems, maxTaskCount_, downloadDir_, suffixName_);
  }

  if (ui_->downloadFailedListWidget->model()->rowCount() == 0) {
    ui_->retryBtn->setEnabled(false);
  }
}

void DownloadQueueDialog::on_selectAllBtn_clicked() {
  ui_->downloadFailedListWidget->selectAll();
}
