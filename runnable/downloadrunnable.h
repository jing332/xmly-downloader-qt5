#ifndef DOWNLOADRUNNABLE_H
#define DOWNLOADRUNNABLE_H

#include <QObject>
#include <QRunnable>

class DownloadRunnable : public QObject, public QRunnable {
  Q_OBJECT
 public:
  DownloadRunnable(int id, const QString &url, const QString &filePath);

  void run() override;

 signals:
  void Start(int id);
  void Finished(int id, const QString &error);

 private:
  int id_ = -1;
  QString url_;
  QString filePath_;
};

#endif  // DOWNLOADRUNNABLE_H
