#include "mainwindow.h"

#include <QClipboard>
#include <QDateTime>
#include <QDebug>
#include <QDesktopServices>
#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QMetaType>
#include <QMouseEvent>
#include <QToolTip>

#include "runnable/getaudiobookinforunnable.h"
#include "runnable/getaudioinforunnable.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui_(new Ui::MainWindow) {
  ui_->setupUi(this);
  qRegisterMetaType<QList<AudioItem *>>("QList<AudioItem*>");
  qRegisterMetaType<QVector<int>>("QVector<int>");

  timer_ = new QTimer(this);
  pool_ = new QThreadPool(this);
  pool_->setMaxThreadCount(1);

  downloadDir_ = qApp->applicationDirPath().append(QStringLiteral("/download"));
  ui_->downloadDirLineEdit->setText(downloadDir_);

  connect(timer_, &QTimer::timeout, this, &MainWindow::Timeout);
  connect(ui_->tableWidget->selectionModel(),
          &QItemSelectionModel::selectionChanged, this, [&]() {
            int size =
                ui_->tableWidget->selectionModel()->selectedRows().size();
            ui_->selectedCountLabel->setText(
                QStringLiteral("已选中: <b>%1</b>").arg(QString::number(size)));
            if (size > 0) {
              ui_->startDownloadBtn->setEnabled(true);
              ui_->unselectBtn->setEnabled(true);
            } else {
              ui_->startDownloadBtn->setEnabled(false);
              ui_->unselectBtn->setEnabled(false);
            }
          });
  connect(ui_->statusbar, &QStatusBar::customContextMenuRequested, this, [&]() {
    QMenu menu;
    QAction action("复制");
    connect(&action, &QAction::triggered, this, [&]() {
      qDebug() << "copy statusBar message:" << ui_->statusbar->currentMessage();
      qApp->clipboard()->setText(ui_->statusbar->currentMessage());
    });
    menu.addAction(&action);
    menu.exec(QCursor::pos());
  });

  ui_->statusbar->setContextMenuPolicy(Qt::CustomContextMenu);
  ui_->statusbar->setStyleSheet("color: DodgerBlue");

  ui_->idLineEdit->setValidator(new QIntValidator(1, 100000000, this));
  ui_->tableWidget->setColumnCount(3);
  ui_->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
  ui_->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui_->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui_->tableWidget->verticalHeader()->setDisabled(true);

  QStringList header = {QStringLiteral("标题"), QStringLiteral("音频ID"),
                        QStringLiteral("音频地址")};
  ui_->tableWidget->setHorizontalHeaderLabels(header);
  ui_->tableWidget->horizontalHeader()->setStretchLastSection(true);
  ui_->tableWidget->horizontalHeader()->setSectionResizeMode(
      0, QHeaderView::ResizeToContents);
  ui_->tableWidget->horizontalHeader()->setSectionResizeMode(
      1, QHeaderView::ResizeToContents);
  ui_->tableWidget->horizontalHeader()->setSectionResizeMode(
      2, QHeaderView::ResizeToContents);
}

MainWindow::~MainWindow() {
  delete ui_;
  qDeleteAll(audioItems_);
}

void MainWindow::Timeout() { ui_->parseBtn->setEnabled(true); }

void MainWindow::on_selectDirBtn_clicked() {
  auto dir = QFileDialog::getExistingDirectory(this);
  if (!dir.isEmpty()) {
    downloadDir_ = dir;
    ui_->downloadDirLineEdit->setText(downloadDir_);
  }
}

void MainWindow::on_parseBtn_clicked() {
  auto audiobookId = ui_->idLineEdit->text().toInt();
  if (0 >= audiobookId) {
    ui_->statusbar->showMessage("请输入有声小说ID");
    ui_->idLineEdit->setFocus();
    return;
  }

  qDeleteAll(audioItems_);
  audioItems_.clear();
  ui_->tableWidget->clearContents();
  ui_->tableWidget->setRowCount(0);

  ui_->parseBtn->setEnabled(false);
  ui_->statusbar->showMessage("获取小说信息...", 2000);

  auto getAudioBookInfoRunnable = new GetAudiobookInfoRunnable(audiobookId);
  connect(getAudioBookInfoRunnable, &GetAudiobookInfoRunnable::finished, this,
          &MainWindow::GetAudiobookInfoFinished);
  pool_->start(getAudioBookInfoRunnable);
}

void MainWindow::on_selectAllBtn_clicked() {
  ui_->tableWidget->selectAll();
  ui_->tableWidget->setFocus();
}

void MainWindow::on_unselectBtn_clicked() {
  ui_->tableWidget->clearSelection();
  ui_->selectAllBtn->setFocus();
}

void MainWindow::on_startDownloadBtn_clicked() {
  qDebug() << "download dir: " << downloadDir_;

  DownloadQueueDialog downloadQueueDialog(this);
  QList<AudioItem *> selectedItems;
  for (auto &index : ui_->tableWidget->selectionModel()->selectedRows(0)) {
    int row = index.row();
    downloadQueueDialog.AddItemWidget(audioItems_.at(row)->id,
                                      audioItems_.at(row)->url,
                                      audioItems_.at(row)->title + suffixName_);
    selectedItems.append(audioItems_.at(row));
  }

  //添加任务到下载队列
  downloadQueueDialog.StartDownload(selectedItems,
                                    ui_->maxTaskCountSpinBox->value(),
                                    downloadDir_, suffixName_);
  if (downloadQueueDialog.exec() == QDialog::Accepted) {
    ui_->statusbar->showMessage("所选文件下载完成!");
  };
}

void MainWindow::on_tableWidget_customContextMenuRequested(const QPoint &pos) {
  QMenu menu;
  QAction action("复制");
  connect(&action, &QAction::triggered, this, [&]() {
    auto item = ui_->tableWidget->itemAt(pos);
    if (item) {
      qDebug() << "copy text: " << item->text();
      qApp->clipboard()->setText(item->text());
    }
  });
  menu.addAction(&action);
  menu.exec(QCursor::pos());
}

void MainWindow::on_mp3RadioBtn_clicked() {
  suffixName_ = QStringLiteral(".mp3");
}

void MainWindow::on_m4aRadioBtn_clicked() {
  suffixName_ = QStringLiteral(".m4a");
}

void MainWindow::GetAudiobookInfoFinished(AudioBookInfo *info,
                                          int audiobookId) {
  QString error(info->error);
  if (error.isEmpty()) {
    auto text =
        QStringLiteral(
            "小说名称: <a href='%3'><span style='text-decoration: underline; "
            "color:black;'>%1</span></a>\t音频数量: <b>%2</b>")
            .arg(info->title)
            .arg(info->audioCount)
            .arg(QStringLiteral("https://www.ximalaya.com/youshengshu/")
                     .append(QString::number(audiobookId)));
    ui_->titleLabel->setText(text);

    int number = info->audioCount / 100;
    int j = info->audioCount % 100;
    if (j > 0) number++;
    for (int i = 1; i < number + 1; i++) {
      auto runnable = new GetAudioInfoRunnable(audiobookId, i, 100);
      connect(runnable, &GetAudioInfoRunnable::finished, this,
              &MainWindow::GetAudioInfoFinished);
      connect(runnable, &GetAudioInfoRunnable::error, this,
              &MainWindow::GetAudioInfoError);
      pool_->start(runnable);
    }
  } else {
    qWarning() << "get audiobook info fail:" << error;
    ui_->statusbar->showMessage(
        QStringLiteral("获取小说信息失败: ").append(error));
    ui_->parseBtn->setEnabled(true);
  }

  delete info;
}

void MainWindow::GetAudioInfoFinished(const QList<AudioItem *> &audioItemList) {
  timer_->start(1000);
  for (auto &audioItem : audioItemList) {
    if (!QString(audioItem->url).isEmpty()) {
      ui_->statusbar->showMessage(audioItem->title, 2000);
      audioItems_.append(audioItem);

      int rowCount = ui_->tableWidget->rowCount();
      ui_->tableWidget->insertRow(rowCount);

      ui_->tableWidget->setItem(rowCount, 0,
                                new QTableWidgetItem(audioItem->title));
      ui_->tableWidget->setItem(
          rowCount, 1, new QTableWidgetItem(QString::number(audioItem->id)));
      ui_->tableWidget->setItem(rowCount, 2,
                                new QTableWidgetItem(audioItem->url));
    }
  }
}

void MainWindow::GetAudioInfoError(const QString reason, int audiobookId,
                                   int page, int pageSize) {
  auto err = QStringLiteral(
                 "get audiobook info fail: {reason: %1, audiobookId: %2, "
                 "page: %3, pageSize: %4}")
                 .arg(reason)
                 .arg(audiobookId, page, pageSize);
  qWarning() << err;
  ui_->statusbar->showMessage(
      QStringLiteral("获取音频列表失败: %1").arg(reason));
  ui_->parseBtn->setEnabled(true);
}

void MainWindow::on_titleLabel_linkActivated(const QString &link) {
  auto btn = QMessageBox::warning(
      this, "是否打开浏览器?",
      QStringLiteral("即将打开链接 %1, 是否继续?").arg(link),
      QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
  if (btn == QMessageBox::Yes) {
    qDebug() << "open url:" << link;
    QDesktopServices::openUrl(QUrl(link));
  }
}

qint64 firstTime = 0;
void MainWindow::on_downloadDirLabel_linkActivated(const QString &) {
  qint64 secondTime = QDateTime::currentMSecsSinceEpoch();
  //双击
  if (800 < secondTime - firstTime) {
    firstTime = secondTime;
  } else {
    QDesktopServices::openUrl(downloadDir_);
  }
}
