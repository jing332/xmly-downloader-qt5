#include "downloadqueuedialog.h"

#include <QCloseEvent>
#include <QDebug>
#include <QMessageBox>

#include "cgo.h"
#include "runnable/downloadrunnable.h"
#include "ui/mainwindow.h"

DownloadQueueDialog::DownloadQueueDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::DownloadQueueDialog) {
  ui->setupUi(this);

  pool = new QThreadPool(this);
  ui->progressBar->setValue(0);

  ui->downloadingListWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->downloadFailedListWidget->setEditTriggers(
      QAbstractItemView::NoEditTriggers);
  ui->downloadFailedListWidget->setSelectionMode(
      QAbstractItemView::ExtendedSelection);
}

DownloadQueueDialog::~DownloadQueueDialog() { delete ui; }

void DownloadQueueDialog::setMaxThreadCount(int count) {
  pool->setMaxThreadCount(count);
}

void DownloadQueueDialog::downloadFile(int id, const QString &url,
                                       const QString &fileName) {
  auto downloadRunnable = new DownloadRunnable(
      id, url, downloadDir_ + "/" + fileName + suffixName_);
  connect(downloadRunnable, &DownloadRunnable::fileLength, this,
          &DownloadQueueDialog::getFileLengthFinished);
  connect(downloadRunnable, &DownloadRunnable::finished, this,
          &DownloadQueueDialog::downloadFinished);
  connect(downloadRunnable, &DownloadRunnable::start, this,
          &DownloadQueueDialog::downloadStart);
  pool->start(downloadRunnable);
}

void DownloadQueueDialog::downloadFile(AudioItem *audioItem) {
  downloadFile(audioItem->id, audioItem->url, audioItem->title);
  audioItems_.append(audioItem);
}

double scale = 0;
void DownloadQueueDialog::startDownload(QList<AudioItem *> &audioItems,
                                        int maxTaskCount,
                                        const QString &downloadDir,
                                        const QString suffixName) {
  this->pool->setMaxThreadCount(maxTaskCount);
  this->maxTaskCount_ = maxTaskCount;
  this->downloadDir_ = downloadDir;
  this->suffixName_ = suffixName;

  pool->setMaxThreadCount(maxTaskCount);
  for (auto &audioItem : audioItems) {
    downloadFile(audioItem);
  }
  scale = double(100) / double(audioItems.size());
}
void DownloadQueueDialog::addItemWidget(int id, const QString &url,
                                        const QString &fileName) {
  auto itemWidget = new DownloadTaskItemWidget(fileName, url);
  auto item = new QListWidgetItem(ui->downloadingListWidget);
  item->setData(Qt::UserRole, QVariant::fromValue(itemWidget));
  item->setSizeHint(QSize(item->sizeHint().width(), 40));
  ui->downloadingListWidget->addItem(item);
  ui->downloadingListWidget->setItemWidget(item, itemWidget);
  downloadingListWidgetItems.insert(id, item);
}

bool DownloadQueueDialog::hasTask() {
  return ui->downloadingListWidget->model()->rowCount() > 0;
}

void DownloadQueueDialog::closeEvent(QCloseEvent *event) {
  auto rb = QMessageBox::warning(
      this, "警告", "是否暂停下载并删除所有任务(不包括已下载文件)?",
      QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
  if (rb == QMessageBox::Yes) {
    pool->clear();
  } else {
    event->ignore();
  }
}

void DownloadQueueDialog::downloadFinished(int id, const QString &error) {
  //移除条目
  auto &listItem = downloadingListWidgetItems[id];
  ui->downloadingListWidget->takeItem(ui->downloadingListWidget->row(listItem));
  downloadingListWidgetItems.remove(id);

  //更新进度条
  int completed =
      audioItems_.size() - ui->downloadingListWidget->model()->rowCount();
  int value = completed * scale;
  ui->progressBar->setValue(value);
  if (value == 100) {  //所有文件下载完成
    audioItems_.clear();
    //无下载失败 自动关闭对话框
    if (ui->downloadFailedListWidget->model()->rowCount() == 0) this->accept();
  }

  if (error.isEmpty()) {
    qDebug() << QStringLiteral("download finished: {id: %1, %2%}")
                    .arg(id)
                    .arg(value);
  } else {
    qWarning() << QStringLiteral("download fail: {id: %1, reason: %2}")
                      .arg(id)
                      .arg(error);

    //通过id找到AudioItem并添加到 下载失败列表
    for (int i = 0; i < audioItems_.size(); i++) {
      auto &item = audioItems_.at(i);
      if (item->id == id) {
        auto listWidgetItem = new QListWidgetItem(item->title);
        listWidgetItem->setData(Qt::UserRole, QVariant::fromValue(item));
        ui->downloadFailedListWidget->addItem(listWidgetItem);
        ui->retryAllBtn->setEnabled(true);
        break;
      }
    }
  }
}

void DownloadQueueDialog::downloadStart(int id) {
  auto &listItem = downloadingListWidgetItems[id];
  auto variant = listItem->data(Qt::UserRole);
  auto itemWidget = variant.value<DownloadTaskItemWidget *>();
  itemWidget->setStatus("正在获取文件\n大小...");
}

void DownloadQueueDialog::getFileLengthFinished(int id, long length) {
  auto &listItem = downloadingListWidgetItems[id];
  auto variant = listItem->data(Qt::UserRole);
  auto itemWidget = variant.value<DownloadTaskItemWidget *>();
  itemWidget->setStatus(
      QStringLiteral("正在下载...\n(%1MB)")  // 1024*1024 = 1048576
          .arg(QString::number(double(length / long(1048576)))));
}

void DownloadQueueDialog::on_downloadFailedListWidget_itemSelectionChanged() {
  if (ui->downloadFailedListWidget->model()->rowCount() > 0) {
    ui->retrySelectedBtn->setEnabled(true);
  } else {
    ui->retrySelectedBtn->setEnabled(false);
  }
}

void DownloadQueueDialog::on_retrySelectedBtn_clicked() {
  //重新下载选中条目
  if (!ui->downloadFailedListWidget->selectedItems().isEmpty()) {
    QList<AudioItem *> selectedItems;
    for (auto &selectedItem : ui->downloadFailedListWidget->selectedItems()) {
      ui->downloadFailedListWidget->takeItem(
          ui->downloadFailedListWidget->row(selectedItem));

      auto variant = selectedItem->data(Qt::UserRole);
      auto audioItem = variant.value<AudioItem *>();
      selectedItems.append(audioItem);
      addItemWidget(audioItem->id, audioItem->url, audioItem->title);
    }
    startDownload(selectedItems, maxTaskCount_, downloadDir_, suffixName_);
  }

  if (ui->downloadFailedListWidget->model()->rowCount() == 0) {
    ui->retryAllBtn->setEnabled(false);
  }
}
