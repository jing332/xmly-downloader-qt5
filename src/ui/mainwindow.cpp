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
    : QMainWindow(parent),
      ui_(new Ui::MainWindow),
      appSettings_(new AppSettings(this)) {
  ui_->setupUi(this);

  qRegisterMetaType<QList<AudioInfo *>>("QList<AudioInfo*>");
  qRegisterMetaType<QVector<int>>("QVector<int>");

  timer_ = new QTimer(this);
  pool_ = new QThreadPool(this);
  pool_->setMaxThreadCount(1);

  if (appSettings_->downloadDir().isEmpty()) {
    appSettings_->setDownloadDir(
        qApp->applicationDirPath().append(QStringLiteral("/download")));
  }
  ui_->downloadDirLineEdit->setText(appSettings_->downloadDir());

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
  qDeleteAll(audioList_);
}

/*先显示主界面再应用配置文件, 不然主题设置会出问题*/
bool firstShow = true;
void MainWindow::showEvent(QShowEvent *) {
  if (firstShow) {
    ApplySettings();
    firstShow = false;
  }
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
void MainWindow::ApplySettings() {
  if (!appSettings_->cookie().isEmpty()) {
    ui_->cookieBtn->setText("已设置Cookie");
  }

  int albumID = appSettings_->albumID();
  if (0 < albumID && 100000000 > albumID) {
    ui_->idLineEdit->setText(QString::number(albumID));
  }

  int theme = appSettings_->theme();
  if (0 <= theme && 3 >= theme) {
    ui_->themeComboBox->setCurrentIndex(theme);
  }

  ui_->downloadDirLineEdit->setText(appSettings_->downloadDir());
}

void MainWindow::Timeout() { ui_->parseBtn->setEnabled(true); }

/*选择目录按钮的点击事件*/
void MainWindow::on_selectDirBtn_clicked() {
  auto dir = QFileDialog::getExistingDirectory(this);
  if (!dir.isEmpty()) {
    appSettings_->setDownloadDir(dir);
    ui_->downloadDirLineEdit->setText(dir);
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

  appSettings_->setAlbumID(albumID);

  qDeleteAll(audioList_);
  audioList_.clear();
  ui_->tableWidget->clearContents();
  ui_->tableWidget->setRowCount(0);

  ui_->parseBtn->setEnabled(false);
  ui_->statusbar->showMessage("获取专辑信息...", 2000);

  auto runnable = new GetAlbumInfoRunnable(albumID);
  connect(runnable, &GetAlbumInfoRunnable::Succeed, this,
          &MainWindow::OnGetAlbumInfoFinished);
  connect(runnable, &GetAlbumInfoRunnable::Failed, this,
          &MainWindow::OnGetAlbumInfoFailed);
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
  DownloadQueueDialog dlQueueDialog(appSettings_->cookie(), this);
  dlQueueDialog.InitValue(ui_->maxTaskCountSpinBox->value(),
                          appSettings_->downloadDir() + "/" + albumName_,
                          extName, isAddNum,
                          GetIntWidth(audioList_.size() + 1));
  for (auto &index : ui_->tableWidget->selectionModel()->selectedRows(0)) {
    dlQueueDialog.AddDownloadTask(index.row() + 1, audioList_.at(index.row()));
  }

  if (QDialog::Accepted == dlQueueDialog.exec()) {
    ui_->statusbar->showMessage("下载完成！");
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
void MainWindow::on_mp3RadioBtn_clicked() { extName = QStringLiteral("mp3"); }
void MainWindow::on_m4aRadioBtn_clicked() { extName = QStringLiteral("m4a"); }

/*获取专辑信息成功*/
void MainWindow::OnGetAlbumInfoFinished(int albumID, AlbumInfo *ai) {
  auto text =
      QStringLiteral(
          "专辑名称: <a href='%3'><span style='text-decoration: underline; "
          "color:black;'>%1</span></a>\t音频数量: <b>%2</b>")
          .arg(ai->title)
          .arg(ai->audioCount)
          .arg(QStringLiteral("https://www.ximalaya.com/youshengshu/")
                   .append(QString::number(albumID)));
  ui_->titleLabel->setText(text);
  albumName_ = QString(ai->title).replace(fileNameReg_, " ");

  auto runnable = new GetAudioInfoRunnable(albumID, 1);
  connect(runnable, &GetAudioInfoRunnable::Succeed, this,
          [&](int albumID, int maxPageID, const QList<AudioInfo *> &list) {
            AddAudioInfoItem(list);
            for (int i = 2; i <= maxPageID; i++) {
              auto run = new GetAudioInfoRunnable(albumID, i);
              connect(run, &GetAudioInfoRunnable::Succeed, this,
                      [&](int albumID, int maxPageID,
                          const QList<AudioInfo *> &list) {
                        AddAudioInfoItem(list);
                      });
              connect(run, &GetAudioInfoRunnable::Failed, this,
                      &MainWindow::OnGetAudioInfoFailed);
              pool_->start(run);
            }
          });
  connect(runnable, &GetAudioInfoRunnable::Failed, this,
          &MainWindow::OnGetAudioInfoFailed);
  pool_->start(runnable);

  delete ai;
}

/*获取专辑信息失败*/
void MainWindow::OnGetAlbumInfoFailed(const QString &err) {
  qWarning() << err;
  ui_->statusbar->showMessage(QStringLiteral("获取专辑信息失败: ").append(err));
  ui_->parseBtn->setEnabled(true);
}

void MainWindow::AddAudioInfoItem(const QList<AudioInfo *> &list) {
  timer_->start(1000);
  for (auto &ai : list) {
    ui_->statusbar->showMessage(ai->title(), 2000);
    audioList_.append(ai);

    int rowCount = ui_->tableWidget->rowCount();
    ui_->tableWidget->insertRow(rowCount);

    ui_->tableWidget->setItem(rowCount, 0, new QTableWidgetItem(ai->title()));
    ui_->tableWidget->setItem(
        rowCount, 1, new QTableWidgetItem(QString::number(ai->trackID())));

    if ("mp3" == extName) {
      ui_->tableWidget->setItem(rowCount, 2,
                                new QTableWidgetItem(ai->mp3URL64()));
    } else {
      ui_->tableWidget->setItem(rowCount, 2,
                                new QTableWidgetItem(ai->m4aURL64()));
    }
  }
}

/*获取音频信息失败*/
void MainWindow::OnGetAudioInfoFailed(int albumID, const QString &err) {
  qWarning() << err;
  ui_->statusbar->showMessage(QStringLiteral("获取音频列表失败: %1").arg(err));
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
    QDesktopServices::openUrl(appSettings_->downloadDir());
  }
}

void MainWindow::on_cookieBtn_clicked() {
  CookieInputDialog inputDlg(appSettings_->cookie(), this);

  if (QDialog::Accepted == inputDlg.exec()) {
    auto cookie = inputDlg.GetCookie();
    if (cookie.isEmpty()) {
      ui_->cookieBtn->setText("未设置Cookie");
      ui_->cookieBtn->setToolTip("");
      appSettings_->setCookie("");
    } else {
      ui_->cookieBtn->setText("已设置Cookie");
      ui_->cookieBtn->setToolTip(cookie);
      appSettings_->setCookie(cookie);
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
  appSettings_->setTheme(index);
}
