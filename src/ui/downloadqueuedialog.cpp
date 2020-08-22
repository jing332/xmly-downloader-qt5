#include "downloadqueuedialog.h"

#include <QCloseEvent>
#include <QDebug>
#include <QMessageBox>

#include "appevent.h"
#include "runnables/downloadfilerunnable.h"
#include "runnables/downloadvipfilerunnable.h"
#include "runnables/getvipaudioinforunnable.h"
#include "ui/mainwindow.h"

DownloadQueueDialog::DownloadQueueDialog(const QString &cookie, QWidget *parent)
    : QDialog(parent),
      ui_(new Ui::DownloadQueueDialog),
      pool_(new QThreadPool(this)),
      cookie_(cookie) {
  ui_->setupUi(this);
  setWindowFlag(Qt::WindowContextHelpButtonHint, false);

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

void DownloadQueueDialog::InitValue(int maxTaskCount,
                                    const QString &downloadDir,
                                    const QString extName, bool isAddNum,
                                    int numberWidth) {
  isAddNum_ = isAddNum;
  pool_->setMaxThreadCount(maxTaskCount);
  maxTaskCount_ = maxTaskCount;
  saveDir_ = downloadDir;
  extName_ = extName;
  numberWidth_ = numberWidth;
  pool_->setMaxThreadCount(maxTaskCount);
}

void DownloadQueueDialog::AddDownloadTask(int number, AudioInfo *ai) {
  auto data = new DownloadItemData(this);
  data->setNumber(number);
  data->setName(ai->title() + "." + extName_);
  data->setSaveDir(saveDir_);
  data->setProperty("ai", QVariant::fromValue(ai));

  auto numStr =
      QString("%1").arg(data->number(), numberWidth_, 10, QLatin1Char('0'));

  if (ai->isEmptyURL()) {
    data->setName((isAddNum_ ? numStr + " " : "") + ai->title() + ".m4a");
    DownloadVipFile(ai->trackID(), cookie_, data);
  } else {
    data->setUri("m4a" == extName_ ? ai->m4aURL64() : ai->mp3URL64());
    data->setName((isAddNum_ ? numStr + " " : "") + ai->title() + "." +
                  extName_);
    DownloadFile(data);
  }

  if (!downloadItemDatas_.contains(data)) {
    downloadItemDatas_.append(data);
  }
  AddDownloadingItemWidget(data);
}

/*下载文件*/
void DownloadQueueDialog::DownloadFile(DownloadItemData *data) {
  QString filePath = QStringLiteral("%1/%2").arg(data->saveDir(), data->name());

  auto downloadRunnable =
      new DownloadFileRunnable(data->number(), data->uri(), filePath);
  connect(downloadRunnable, &DownloadFileRunnable::Start, this,
          &DownloadQueueDialog::OnDownloadStart);
  connect(downloadRunnable, &DownloadFileRunnable::Finished, this,
          &DownloadQueueDialog::OnDownloadFinished);
  pool_->start(downloadRunnable);
}

/*下载VIP文件*/
void DownloadQueueDialog::DownloadVipFile(int trackID, const QString &cookie,
                                          DownloadItemData *data) {
  auto runnable = new GetVipAudioInfoRunnable(trackID, cookie, data);
  connect(runnable, &GetVipAudioInfoRunnable::Start, this,
          [&](int trackID, int number) {
            Q_UNUSED(trackID);
            auto itemWidget = GetDownloadingItemWidget(number);
            itemWidget->setProgressBarVisible(true);
            itemWidget->SetStatus("获取下载地址...");
          });
  connect(runnable, &GetVipAudioInfoRunnable::Failed, this,
          [&](int trackID, QString err, DownloadItemData *data) {
            qWarning() << err;
            AddDownloadFailedItemWidget(data, "无法获取下载地址: " + err);

            auto listItem = GetItemIndexByNumber(data->number());
            ui_->downloadingListWidget->takeItem(
                GetItemIndexByNumber(data->number()));

            SetDownloadFailedCount(downloadFailedCount_ + 1);
          });

  connect(runnable, &GetVipAudioInfoRunnable::Finished, this,
          [&](AudioInfo *ai, DownloadItemData *data) {
            data->setUri(ai->m4aURL64());
            DownloadFile(data);
          });
  pool_->start(runnable);
}

void DownloadQueueDialog::AddDownloadingItemWidget(DownloadItemData *data) {
  auto ai = data->property("ai").value<AudioInfo *>();
  auto itemWidget = new DownloadTaskItemWidget(ai->title() + "." + extName_);
  itemWidget->setProperty("data", QVariant::fromValue(data));
  auto item = new QListWidgetItem(ui_->downloadingListWidget);
  item->setSizeHint(QSize(0, 45));
  item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
  item->setData(Qt::UserRole, QVariant::fromValue(itemWidget));
  ui_->downloadingListWidget->addItem(item);
  ui_->downloadingListWidget->setItemWidget(item, itemWidget);
}

void DownloadQueueDialog::AddDownloadFailedItemWidget(DownloadItemData *data,
                                                      const QString &err) {
  auto listWidgetItem = new QListWidgetItem(data->name());
  listWidgetItem->setToolTip(err);
  listWidgetItem->setData(Qt::UserRole, QVariant::fromValue(data));
  ui_->downloadFailedListWidget->addItem(listWidgetItem);
}

int DownloadQueueDialog::GetItemIndexByNumber(int number) {
  for (int i = 0; i < ui_->downloadingListWidget->count(); i++) {
    auto item = ui_->downloadingListWidget->item(i);
    auto itemWidget =
        item->data(Qt::UserRole).value<DownloadTaskItemWidget *>();
    auto data = itemWidget->property("data").value<DownloadItemData *>();
    if (data->number() == number) return i;
  }

  return -1;
}

bool DownloadQueueDialog::HasTask() {
  return ui_->downloadingListWidget->model()->rowCount() > 0;
}

void DownloadQueueDialog::closeEvent(QCloseEvent *event) {
  if (ui_->downloadingListWidget->count() == 0 && downloadFailedCount_ == 0) {
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

/*使用number获取DownloadTaskItemWidget*/
DownloadTaskItemWidget *DownloadQueueDialog::GetDownloadingItemWidget(
    int number) {
  for (int i = 0; i < ui_->downloadingListWidget->count(); i++) {
    auto item = ui_->downloadingListWidget->item(i);
    auto itemWidget =
        item->data(Qt::UserRole).value<DownloadTaskItemWidget *>();
    auto data = itemWidget->property("data").value<DownloadItemData *>();
    if (data->number() == number) return itemWidget;
  }

  return nullptr;
}

DownloadTaskItemWidget *DownloadQueueDialog::GetFailedItemWidget(int number) {
  return ui_->downloadFailedListWidget->item(number)
      ->data(Qt::UserRole)
      .value<DownloadTaskItemWidget *>();
}

void DownloadQueueDialog::SetDownloadFailedCount(int count) {
  downloadFailedCount_ = count;
  ui_->tabWidget->setTabText(
      1, QStringLiteral("下载失败(%1)").arg(downloadFailedCount_));
}

void DownloadQueueDialog::OnDownloadStart(int number) {
  auto itemWidget = GetDownloadingItemWidget(number);
  itemWidget->SetStatus("获取文件\n大小...");
  itemWidget->SetProgressBarVisible(true);
}

void DownloadQueueDialog::OnDownloadFinished(int number, const QString &error) {
  ui_->downloadingListWidget->takeItem(GetItemIndexByNumber(number));
  int rowCount = ui_->downloadingListWidget->model()->rowCount();
  ui_->tabWidget->setTabText(0, QStringLiteral("正在下载(%1)").arg(rowCount));

  if (error.isEmpty()) {
    qDebug() << QStringLiteral("download finished: {id: %1}").arg(number);
    downloadedCount_++;
    setWindowTitle(
        QStringLiteral("下载管理 (已下载: %1)").arg(downloadedCount_));
  } else {
    qWarning() << QStringLiteral("download fail: {id: %1, reason: %2}")
                      .arg(number)
                      .arg(error);

    /*通过id找到AudioItem并添加到下载失败列表*/
    for (int i = 0; i < downloadItemDatas_.size(); i++) {
      auto data = downloadItemDatas_.at(i);
      if (data->number() == number) {
        AddDownloadFailedItemWidget(data, error);
        SetDownloadFailedCount(downloadFailedCount_ + 1);
        break;
      }
    }
  }

  /*更新进度条*/
  int completed = downloadItemDatas_.size() -
                  ui_->downloadingListWidget->model()->rowCount();
  int value = (float)completed / downloadItemDatas_.size() * 100.0;
  ui_->progressBar->setValue(value);
  if (value == 100) { /*所有文件下载完成*/
    downloadItemDatas_.clear();
    if (ui_->downloadFailedListWidget->model()->rowCount() == 0) {
      this->accept(); /*无下载失败 自动关闭对话框*/
    } else {
      ui_->tabWidget->setCurrentIndex(1);
    }
  }
}

void DownloadQueueDialog::OnUpdateFileLength(int id, long *contentLength,
                                             long *currentLength) {
  auto itemWidget = GetDownloadingItemWidget(id);
  if (itemWidget) {
    itemWidget->UpdateProgressBar(float(*currentLength) / *contentLength *
                                  100.0);
    itemWidget->SetStatus(
        QStringLiteral("(%1MB/%2MB)") /*1024k * 1024k = 1048576k = 1MB*/
            .arg(QString::number(double(*currentLength) / double(1048576), 'f',
                                 2))
            .arg(QString::number(double(*contentLength) / double(1048576), 'f',
                                 2)));
  }
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

    int count = ui_->downloadFailedListWidget->selectedItems().size();
    for (auto &selectedItem : ui_->downloadFailedListWidget->selectedItems()) {
      auto data = selectedItem->data(Qt::UserRole).value<DownloadItemData *>();
      auto ai = data->property("ai").value<AudioInfo *>();
      AddDownloadTask(data->number(), ai);
      ui_->downloadFailedListWidget->takeItem(
          ui_->downloadFailedListWidget->row(selectedItem));
    }

    SetDownloadFailedCount(downloadFailedCount_ - count);
  }

  if (ui_->downloadFailedListWidget->model()->rowCount() == 0) {
    ui_->retryBtn->setEnabled(false);
  }
}

void DownloadQueueDialog::on_selectAllBtn_clicked() {
  ui_->downloadFailedListWidget->selectAll();
}
