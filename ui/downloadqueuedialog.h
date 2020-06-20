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
  explicit DownloadQueueDialog(const QString &cookie,
                               QWidget *parent = nullptr);

  ~DownloadQueueDialog();

  void SetMaxThreadCount(int count);
  void DownloadFile(int id, const QString &url, const QString &fileName);
  void DownloadFile(AudioItem *audioItem);
  void DownloadVipFile(AudioItem *ai);

  void StartDownload(QList<AudioItem *> &audioItems, int maxTaskCount,
                     const QString &downloadDir, const QString suffixName);
  void AddDownloadingItemWidget(int id, const QString &url,
                                const QString &filePath);
  void AddDownloadFailedItemWidget(int trackId, AudioItem *ai,
                                   const QString &err);
  bool HasTask();

 protected:
  void closeEvent(QCloseEvent *) override;

 private:
  DownloadTaskItemWidget *GetDownloadTaskItemWidget(int trackID);
  void SetDownloadFailedCount(int count);

 private slots:
  void OnDownloadFinished(int id, const QString &error);
  void OnDownloadStart(int id);
  void OnUpdateFileLength(int id, long contentLength, long currentLength);

  void on_retryBtn_clicked();
  void on_selectAllBtn_clicked();
  void on_downloadFailedListWidget_itemSelectionChanged();

 private:
  Ui::DownloadQueueDialog *ui_;
  QThreadPool *pool_ = nullptr;

  QMap<int, QListWidgetItem *> downloadingListWidgetItems_;
  QList<AudioItem *> audioItems_;

  int downloadFailedCount_ = 0;
  int taskCount_ = 0;
  int maxTaskCount_ = 1;
  QString cookie_;
  QString downloadDir_;
  QString suffixName_;
};

#endif  // DOWNLOADQUEUEDIALOG_H
