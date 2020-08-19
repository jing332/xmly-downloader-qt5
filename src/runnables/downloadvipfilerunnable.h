//#ifndef DOWNLOADVIPFILERUNNABLE_H
//#define DOWNLOADVIPFILERUNNABLE_H

//#include <QObject>
//#include <QRunnable>

//#include "audioinfo.h"
//#include "xmlydownloader.h"

// class DownloadVipFileRunnable : public QObject, public QRunnable {
//  Q_OBJECT
// public:
//  DownloadVipFileRunnable(int trackID, const QString &cookie,
//                          const QString &filePath, AudioInfo *ai);

// protected:
//  void run() override;

// signals:
//  void StartGetInfo(int trackID);
//  void StartDownload(int trackID, AudioInfo *ai);
//  void DownloadError(int trackID, AudioInfo *ai, const QString &err);
//  void GetInfoError(int trackID, AudioInfo *ai, const QString &err);
//  void Finished(int trackID);

// private:
//  int trackID_;
//  QString cookie_;
//  QString filePath_;
//  AudioInfo *ai_ = nullptr;
//};

//#endif  // DOWNLOADVIPFILERUNNABLE_H
