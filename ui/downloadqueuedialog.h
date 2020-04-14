#ifndef DOWNLOADQUEUEDIALOG_H
#define DOWNLOADQUEUEDIALOG_H

#include <QDialog>
#include <QMap>
#include <QThreadPool>

#include "cgo.h"
#include "downloadtaskitemwidget.h"
#include "ui_downloadqueuedialog.h"

namespace Ui {
class DownloadQueueDialog;
}

class DownloadQueueDialog : public QDialog {
  Q_OBJECT

 public:
  explicit DownloadQueueDialog(QWidget *parent = nullptr);
  ~DownloadQueueDialog();

  void setMaxThreadCount(int count);
  void downloadFile(int id, const QString &url, const QString &fileName);
  void downloadFile(AudioItem *audioItem);
  void startDownload(QList<AudioItem *> &audioItems, int maxTaskCount,
                     const QString &downloadDir, const QString suffixName);
  void addItemWidget(int id, const QString &url, const QString &filePath);
  bool hasTask();

 protected:
  void closeEvent(QCloseEvent *) override;

 private slots:
  void downloadFinished(int id, const QString &error);
  void downloadStart(int id);
  void getFileLengthFinished(int id, long length);

  void on_downloadFailedListWidget_itemSelectionChanged();
  void on_retrySelectedBtn_clicked();

 private:
  Ui::DownloadQueueDialog *ui;
  QThreadPool *pool = nullptr;

  QMap<int, QListWidgetItem *> downloadingListWidgetItems;
  QList<AudioItem *> audioItems_;

  int taskCount_ = 0;
  int maxTaskCount_ = 1;
  QString downloadDir_;
  QString suffixName_;
};

#endif  // DOWNLOADQUEUEDIALOG_H
