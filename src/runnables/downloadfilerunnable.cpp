#include "downloadfilerunnable.h"

#include <QDateTime>
#include <QDebug>
#include <QRandomGenerator>
#include <QThread>

#include "xmlydownloader.h"

DownloadFileRunnable::DownloadFileRunnable(int id, const QString &url,
                                           const QString &filePath)
    : id_(id), url_(url), filePath_(filePath) {}

void DownloadFileRunnable::run() {
  emit Start(id_);
  auto *err =
      CgoDownloadFile(const_cast<char *>(url_.toStdString().c_str()),
                      const_cast<char *>(filePath_.toStdString().c_str()), id_);

  if (err) {
    qWarning()
        << QStringLiteral("download fail: {url: %1, error: %2}").arg(url_, err);
    emit Finished(id_, QString(err));
    delete err;
  } else
    emit Finished(id_, QStringLiteral(""));
}
