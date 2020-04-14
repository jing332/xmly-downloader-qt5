#include "mainwindow.h"

#include <QClipboard>
#include <QDebug>
#include <QDesktopServices>
#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QMetaType>
#include <QToolTip>

#include "runnable/getaudiobookinforunnable.h"
#include "runnable/getaudioinforunnable.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  qRegisterMetaType<QList<AudioItem *>>("QList<AudioItem*>");
  qRegisterMetaType<QVector<int>>("QVector<int>");

  timer = new QTimer(this);
  pool = new QThreadPool(this);
  pool->setMaxThreadCount(1);

  downloadDir = qApp->applicationDirPath().append(QStringLiteral("/download"));
  ui->downloadDirLineEdit->setText(downloadDir);

  connect(timer, &QTimer::timeout, this, &MainWindow::timeout);
  connect(ui->tableWidget->selectionModel(),
          &QItemSelectionModel::selectionChanged, this, [&]() {
            int size = ui->tableWidget->selectionModel()->selectedRows().size();
            ui->selectedCountLabel->setText(
                QStringLiteral("已选中: <b>%1</b>").arg(QString::number(size)));
            if (size > 0) {
              ui->startDownloadBtn->setEnabled(true);
              ui->unselectBtn->setEnabled(true);
            } else {
              ui->startDownloadBtn->setEnabled(false);
              ui->unselectBtn->setEnabled(false);
            }
          });
  connect(ui->statusbar, &QStatusBar::customContextMenuRequested, this, [&]() {
    QMenu menu;
    QAction action("复制");
    connect(&action, &QAction::triggered, this, [&]() {
      qDebug() << "copy statusBar message:" << ui->statusbar->currentMessage();
      qApp->clipboard()->setText(ui->statusbar->currentMessage());
    });
    menu.addAction(&action);
    menu.exec(QCursor::pos());
  });

  ui->statusbar->setContextMenuPolicy(Qt::CustomContextMenu);
  ui->statusbar->setStyleSheet("color: DodgerBlue");

  ui->idLineEdit->setValidator(new QIntValidator(1, 100000000, this));
  ui->tableWidget->setColumnCount(3);
  ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
  ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->tableWidget->verticalHeader()->setDisabled(true);

  QStringList header = {QStringLiteral("标题"), QStringLiteral("音频ID"),
                        QStringLiteral("音频地址")};
  ui->tableWidget->setHorizontalHeaderLabels(header);
  ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
  ui->tableWidget->horizontalHeader()->setSectionResizeMode(
      0, QHeaderView::ResizeToContents);
  ui->tableWidget->horizontalHeader()->setSectionResizeMode(
      1, QHeaderView::ResizeToContents);
  ui->tableWidget->horizontalHeader()->setSectionResizeMode(
      2, QHeaderView::ResizeToContents);
}

MainWindow::~MainWindow() {
  delete ui;
  qDeleteAll(audioItems);
}

void MainWindow::timeout() { ui->parseBtn->setEnabled(true); }

void MainWindow::on_selectDirBtn_clicked() {
  auto dir = QFileDialog::getExistingDirectory(this);
  if (!dir.isEmpty()) {
    downloadDir = dir;
    ui->downloadDirLineEdit->setText(dir);
  }
}

void MainWindow::on_parseBtn_clicked() {
  audioItems.clear();
  ui->tableWidget->clearContents();
  ui->tableWidget->setRowCount(0);

  ui->parseBtn->setEnabled(false);
  ui->statusbar->showMessage("获取小说信息...", 2000);
  auto audiobookId = ui->idLineEdit->text().toInt();
  auto getAudioBookInfoRunnable = new GetAudiobookInfoRunnable(audiobookId);
  connect(getAudioBookInfoRunnable, &GetAudiobookInfoRunnable::finished, this,
          &MainWindow::getAudiobookInfoFinished);
  pool->start(getAudioBookInfoRunnable);
}

void MainWindow::on_selectAllBtn_clicked() {
  ui->tableWidget->selectAll();
  ui->tableWidget->setFocus();
}

void MainWindow::on_unselectBtn_clicked() { ui->tableWidget->clearSelection(); }

void MainWindow::on_startDownloadBtn_clicked() {
  downloadDir = ui->downloadDirLineEdit->text();
  qDebug() << "download dir: " << downloadDir;

  DownloadQueueDialog downloadQueueDialog(this);
  QList<AudioItem *> selectedItems;
  for (auto &index : ui->tableWidget->selectionModel()->selectedRows(0)) {
    int row = index.row();
    downloadQueueDialog.addItemWidget(audioItems.at(row)->id,
                                      audioItems.at(row)->url,
                                      audioItems.at(row)->title + suffixName);
    selectedItems.append(audioItems.at(row));
  }

  //添加任务到下载队列
  downloadQueueDialog.startDownload(
      selectedItems, ui->maxTaskCountSpinBox->value(), downloadDir, suffixName);
  if (downloadQueueDialog.exec() == QDialog::Accepted) {
    ui->statusbar->showMessage("所选文件下载完成!", 5000);
    QToolTip::showText(mapToGlobal(ui->startDownloadBtn->pos()),
                       "所选文件下载完成！", this);
  };
}
void MainWindow::on_tableWidget_customContextMenuRequested(const QPoint &pos) {
  QMenu menu;
  QAction action("复制");
  connect(&action, &QAction::triggered, this, [&]() {
    auto item = ui->tableWidget->itemAt(pos);
    if (item) {
      qDebug() << "copy text: " << item->text();
      qApp->clipboard()->setText(item->text());
    }
  });
  menu.addAction(&action);
  menu.exec(QCursor::pos());
}

void MainWindow::on_mp3RadioBtn_clicked() {
  suffixName = QStringLiteral(".mp3");
}

void MainWindow::on_m4aRadioBtn_clicked() {
  suffixName = QStringLiteral(".m4a");
}

void MainWindow::getAudiobookInfoFinished(AudioBookInfo *info,
                                          int audiobookId) {
  QString error(info->error);
  if (error.isEmpty()) {
    auto text =
        QStringLiteral("小说名称: <a href=%3>%1</a>\t音频数量: <i>%2</i>")
            .arg(info->title)
            .arg(info->audioCount)
            .arg(QStringLiteral("https://www.ximalaya.com/youshengshu/")
                     .append(QString::number(audiobookId)));
    ui->titleLabel->setText(text);

    int number = info->audioCount / 100;
    int j = info->audioCount % 100;
    if (j > 0) number++;
    for (int i = 1; i < number + 1; i++) {
      auto runnable = new GetAudioInfoRunnable(audiobookId, i, 100);
      connect(runnable, &GetAudioInfoRunnable::finished, this,
              &MainWindow::getAudioInfoFinished);
      connect(runnable, &GetAudioInfoRunnable::error, this,
              &MainWindow::getAudioInfoError);
      pool->start(runnable);
    }
  } else {
    qWarning() << "get audiobook info fail:" << error;
    ui->statusbar->showMessage(
        QStringLiteral("获取小说信息失败: ").append(error), 1000 * 10);
    ui->parseBtn->setEnabled(true);
  }

  delete info;
}

void MainWindow::getAudioInfoFinished(const QList<AudioItem *> &audioItemList) {
  for (auto &audioItem : audioItemList) {
    if (!QString(audioItem->url).isEmpty()) {
      timer->start(3000);
      ui->statusbar->showMessage(audioItem->title, 2000);
      audioItems.append(audioItem);

      int rowCount = ui->tableWidget->rowCount();
      ui->tableWidget->insertRow(rowCount);

      ui->tableWidget->setItem(rowCount, 0,
                               new QTableWidgetItem(audioItem->title));
      ui->tableWidget->setItem(
          rowCount, 1, new QTableWidgetItem(QString::number(audioItem->id)));
      ui->tableWidget->setItem(rowCount, 2,
                               new QTableWidgetItem(audioItem->url));
    }
  }
}

void MainWindow::getAudioInfoError(const QString reason, int audiobookId,
                                   int page, int pageSize) {
  auto err = QStringLiteral(
                 "get audiobook info fail: {reason: %1, audiobookId: %2, "
                 "page: %3, pageSize: %4}")
                 .arg(reason)
                 .arg(audiobookId, page, pageSize);
  qWarning() << err;
  ui->statusbar->showMessage(err, 1000 * 10);
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
