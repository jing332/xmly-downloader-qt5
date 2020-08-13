#include "mainwindow.h"

#include <QClipboard>
#include <QDateTime>
#include <QDebug>
#include <QDesktopServices>
#include <QFileDialog>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QMetaType>
#include <QMouseEvent>
#include <QScrollBar>
#include <QTextEdit>

#include "cookieinputdialog.h"
#include "runnables/getalbuminforunnable.h"
#include "runnables/getaudioinforunnable.h"
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
    QAction action("复制", this);
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

  QStringList header = {QStringLiteral("音频名称"), QStringLiteral("音频ID"),
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

/*先显示主程序再读取配置文件, 不然主题设置会出问题*/
bool firstShow = true;
void MainWindow::showEvent(QShowEvent *) {
  if (firstShow) {
    ReadConfig();
    firstShow = false;
  }
}

void MainWindow::closeEvent(QCloseEvent *) {
  CgoWriteConfig(ui_->idLineEdit->text().toInt(),
                 ui_->maxTaskCountSpinBox->value(),
                 ui_->themeComboBox->currentIndex(),
                 const_cast<char *>(cookie_.toStdString().c_str()),
                 const_cast<char *>(downloadDir_.toStdString().c_str()));
}

/*读取文件并设置样式表*/
void MainWindow::SetStyleSheet(const QString &filePath) {
  QFile f(filePath);
  f.open(QIODevice::ReadOnly | QIODevice::Text);
  setStyleSheet(f.readAll());
  f.close();
}

/*获取int的位数*/
int MainWindow::GetIntWidth(int n) {
  int count = 0;
  while (n != 0) {
    n = n / 10;
    count++;
  }

  return count;
}

/*读取应用配置文件*/
void MainWindow::ReadConfig() {
  auto appCfg = CgoReadConfig();
  if (appCfg) {
    cookie_ = appCfg->cookie;
    if (!cookie_.isEmpty()) {
      ui_->cookieBtn->setText("已设置Cookie");
    }

    if (0 < appCfg->albumID && 100000000 > appCfg->albumID) {
      ui_->idLineEdit->setText(QString::number(appCfg->albumID));
    }

    if (0 <= appCfg->theme && 3 >= appCfg->theme) {
      ui_->themeComboBox->setCurrentIndex(appCfg->theme);
    }

    downloadDir_ = appCfg->downloadDir;
    ui_->downloadDirLineEdit->setText(downloadDir_);
    ui_->maxTaskCountSpinBox->setValue(appCfg->maxTaskCount);
    delete appCfg;
  }
}

void MainWindow::Timeout() { ui_->parseBtn->setEnabled(true); }

/*选择目录按钮的点击事件*/
void MainWindow::on_selectDirBtn_clicked() {
  auto dir = QFileDialog::getExistingDirectory(this);
  if (!dir.isEmpty()) {
    downloadDir_ = dir;
    ui_->downloadDirLineEdit->setText(downloadDir_);
  }
}

/*解析按钮的点击事件*/
void MainWindow::on_parseBtn_clicked() {
  auto albumID = ui_->idLineEdit->text().toInt();
  if (0 >= albumID) {
    ui_->statusbar->showMessage("请输入专辑ID");
    ui_->idLineEdit->setFocus();
    return;
  }

  qDeleteAll(audioItems_);
  audioItems_.clear();
  ui_->tableWidget->clearContents();
  ui_->tableWidget->setRowCount(0);

  ui_->parseBtn->setEnabled(false);
  ui_->statusbar->showMessage("获取专辑信息...", 2000);

  auto runnable = new GetAlbumInfoRunnable(albumID);
  connect(runnable, &GetAlbumInfoRunnable::Finished, this,
          &MainWindow::OnGetAlbumInfoFinished);
  connect(runnable, &GetAlbumInfoRunnable::Error, this,
          &MainWindow::OnGetAlbumInfoError);
  pool_->start(runnable);
}

void MainWindow::on_selectAllBtn_clicked() {
  ui_->tableWidget->selectAll();
  ui_->tableWidget->setFocus();
}

void MainWindow::on_unselectBtn_clicked() {
  ui_->tableWidget->clearSelection();
  ui_->selectAllBtn->setFocus();
}

/*开始下载按钮的点击事件*/
void MainWindow::on_startDownloadBtn_clicked() {
  qInfo() << "download dir: " << downloadDir_ + "/" + albumName_;

  DownloadQueueDialog downloadQueueDialog(cookie_, this);
  QList<AudioItem *> selectedItems;
  for (auto &index : ui_->tableWidget->selectionModel()->selectedRows(0)) {
    int row = index.row();

    downloadQueueDialog.AddDownloadingItemWidget(audioItems_.at(row)->id,
                                                 audioItems_.at(row)->title);

    /*设置序号*/
    audioItems_.at(row)->number = QString("%1").arg(
        row + 1, GetIntWidth(audioItems_.size()) + 1, 10, QLatin1Char('0'));

    selectedItems.append(audioItems_.at(row));
  }

  /*添加任务到下载队列*/
  downloadQueueDialog.StartDownload(
      selectedItems, ui_->maxTaskCountSpinBox->value(),
      downloadDir_ + "/" + albumName_, suffixName_, isAddNum);

  if (QDialog::Accepted == downloadQueueDialog.exec()) {
    ui_->statusbar->showMessage("所选文件下载完成!");
  };
}

/*表格的右键菜单*/
void MainWindow::on_tableWidget_customContextMenuRequested(const QPoint &pos) {
  QMenu menu;
  QAction action("复制", this);

  connect(&action, &QAction::triggered, this, [&]() {
    auto item = ui_->tableWidget->itemAt(pos);
    if (item) {
      qApp->clipboard()->setText(item->text());
    }
  });
  menu.addAction(&action);
  menu.exec(QCursor::pos());
}

void MainWindow::on_addNumCheckBox_clicked() {
  isAddNum = ui_->addNumCheckBox->isChecked();
}

/*文件后缀名修改*/
void MainWindow::on_mp3RadioBtn_clicked() {
  suffixName_ = QStringLiteral("mp3");
}
void MainWindow::on_m4aRadioBtn_clicked() {
  suffixName_ = QStringLiteral("m4a");
}

/*获取专辑信息成功*/
void MainWindow::OnGetAlbumInfoFinished(AlbumInfo *ai, int albumID) {
  auto text =
      QStringLiteral(
          "小说名称: <a href='%3'><span style='text-decoration: underline; "
          "color:black;'>%1</span></a>\t音频数量: <b>%2</b>")
          .arg(ai->title)
          .arg(ai->audioCount)
          .arg(QStringLiteral("https://www.ximalaya.com/youshengshu/")
                   .append(QString::number(albumID)));
  ui_->titleLabel->setText(text);
  albumName_ = QString(ai->title).replace(fileNameReg_, " ");

  int number = ai->audioCount / 100;
  int j = ai->audioCount % 100;
  if (j > 0) number++;
  for (int i = 1; i < number + 1; i++) {
    auto runnable = new GetAudioInfoRunnable(albumID, i, 100);
    connect(runnable, &GetAudioInfoRunnable::Finished, this,
            &MainWindow::OnGetAudioInfoFinished);
    connect(runnable, &GetAudioInfoRunnable::Error, this,
            &MainWindow::OnGetAudioInfoError);
    pool_->start(runnable);
  }

  delete ai;
}

/*获取专辑信息失败*/
void MainWindow::OnGetAlbumInfoError(const QString &err) {
  qWarning() << "get album info fail:" << err;
  ui_->statusbar->showMessage(QStringLiteral("获取专辑信息失败: ").append(err));
  ui_->parseBtn->setEnabled(true);
}

/*获取音频信息完成*/
void MainWindow::OnGetAudioInfoFinished(
    const QList<AudioItem *> &audioItemList) {
  timer_->start(1000);
  for (auto &ai : audioItemList) {
    ui_->statusbar->showMessage(ai->title, 2000);
    audioItems_.append(ai);

    int rowCount = ui_->tableWidget->rowCount();
    ui_->tableWidget->insertRow(rowCount);

    ui_->tableWidget->setItem(rowCount, 0, new QTableWidgetItem(ai->title));
    ui_->tableWidget->setItem(rowCount, 1,
                              new QTableWidgetItem(QString::number(ai->id)));

    if (QString(ai->url).isEmpty()) {
      ui_->tableWidget->setItem(rowCount, 2,
                                new QTableWidgetItem("无音频地址"));
    } else {
      ui_->tableWidget->setItem(rowCount, 2, new QTableWidgetItem(ai->url));
    }
  }
}

/*获取音频信息失败*/
void MainWindow::OnGetAudioInfoError(const QString reason, int albumID,
                                     int page, int pageSize) {
  auto err = QStringLiteral(
                 "get audio info failed: {reason: %1, audiobookId: %2, "
                 "page: %3, pageSize: %4}")
                 .arg(reason)
                 .arg(albumID, page, pageSize);
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

/*打开下载目录*/
qint64 firstTime = 0;
void MainWindow::on_downloadDirLabel_linkActivated(const QString &) {
  qint64 secondTime = QDateTime::currentMSecsSinceEpoch();
  /*判断双击*/
  if (800 < secondTime - firstTime) {
    firstTime = secondTime;
  } else {
    QDesktopServices::openUrl(downloadDir_);
  }
}

void MainWindow::on_cookieBtn_clicked() {
  CookieInputDialog inputDlg(cookie_, this);

  if (QDialog::Accepted == inputDlg.exec()) {
    auto cookie = inputDlg.GetCookie();
    if (cookie.isEmpty()) {
      ui_->cookieBtn->setText("未设置Cookie");
      ui_->cookieBtn->setToolTip("");
      cookie_.clear();
    } else {
      ui_->cookieBtn->setText("已设置Cookie");
      ui_->cookieBtn->setToolTip(cookie);
      cookie_ = cookie;
    }
  }
}

/*切换主题*/
void MainWindow::on_themeComboBox_currentIndexChanged(int index) {
  /*样式表来自
   * https://github.com/feiyangqingyun/QWidgetDemo/tree/master/styledemo/other/qss
   */
  switch (index) {
    case 1:  //淡蓝
      SetStyleSheet(QStringLiteral(":/qss/lightblue.css"));
      break;
    case 2:  // PS黑
      SetStyleSheet(QStringLiteral(":/qss/psblack.css"));
      break;
    case 3:  //扁平白
      SetStyleSheet(QStringLiteral(":/qss/flatwhite.css"));
      break;
    case 0:  //默认
    default:
      setStyleSheet(QStringLiteral("QWidget{font: 12pt 'Microsoft YaHei'}"));
      break;
  }
}
