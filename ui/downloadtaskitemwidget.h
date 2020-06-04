#ifndef DOWNLOADITEMWIDGET_H
#define DOWNLOADITEMWIDGET_H

#include <QLabel>
#include <QWidget>

class DownloadTaskItemWidget : public QWidget {
  Q_OBJECT
 public:
  explicit DownloadTaskItemWidget(const QString &fileName, const QString &url,
                                  QWidget *parent = nullptr);

  void SetStatus(const QString &status);

 private:
  QLabel *statusLabel_;
  QLabel *fileNameLabel_;
  QLabel *urlLabel_;
};

#endif  // DOWNLOADITEMWIDGET_H
