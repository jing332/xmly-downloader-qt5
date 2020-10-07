#ifndef DOWNLOADQUEUEDIALOG_H
#define DOWNLOADQUEUEDIALOG_H

#include <QDialog>
#include <QMap>
#include <QThreadPool>

#include "downloaditemdata.h"
#include "downloadtaskitemwidget.h"
#include "trackinfo.h"
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

  void InitValue(int maxTaskCount, const QString &downloadDir,
                 const QString suffixName, bool isAddNum, int numWidth);
  void AddDownloadTask(int number, TrackInfo *ai);
  bool HasTask();

 protected:
  void closeEvent(QCloseEvent *) override;

 private:
  void DownloadFile(DownloadItemData *data);
  void DownloadVipFile(int trackID, const QString &cookie,
                       DownloadItemData *data);

  void AddDownloadingItemWidget(DownloadItemData *data);
  void AddDownloadFailedItemWidget(DownloadItemData *data, const QString &err);

  int GetItemIndexByNumber(int number);
  DownloadTaskItemWidget *GetDownloadingItemWidget(int number);
  DownloadTaskItemWidget *GetFailedItemWidget(int number);

  void SetDownloadFailedCount(int count);

 private slots:
  void OnDownloadStart(int id);
  void OnDownloadFinished(int id, const QString &error);

  void OnUpdateFileLength(int id, long *contentLength, long *currentLength);

  void on_retryBtn_clicked();
  void on_selectAllBtn_clicked();
  void on_downloadFailedListWidget_itemSelectionChanged();

 private:
  Ui::DownloadQueueDialog *ui_;

  QThreadPool *pool_ = nullptr;
  QList<DownloadItemData *> downloadItemDatas_;

  bool isAddNum_ = false;
  int numberWidth_ = 0;
  int downloadedCount_ = 0;
  int downloadFailedCount_ = 0;
  int taskCount_ = 0;
  int maxTaskCount_ = 1;
  QString cookie_;
  QString saveDir_;
  QString extName_;
};

#endif  // DOWNLOADQUEUEDIALOG_H
