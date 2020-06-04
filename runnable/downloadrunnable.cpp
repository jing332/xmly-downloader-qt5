#include "downloadrunnable.h"

#include <QDateTime>
#include <QDebug>
#include <QRandomGenerator>
#include <QThread>

#include "cgo.h"

DownloadRunnable::DownloadRunnable(int id, const QString &url,
                                   const QString &filePath) {
  this->id_ = id;
  this->url_ = url;
  this->filePath_ = filePath;
}

void DownloadRunnable::run() {
  emit start(id_);

  auto *cstr = Cgo::getInstance()->cgo_downloadFile(
      url_.toStdString().c_str(), filePath_.toStdString().c_str(), id_);

  if (cstr) {
    QString error(cstr);
    qWarning() << QStringLiteral("download fail: {url: %1, error: %2}")
                      .arg(url_, error);
    emit finished(id_, error);
    delete cstr;
  } else
    emit finished(id_, QStringLiteral(""));
}
