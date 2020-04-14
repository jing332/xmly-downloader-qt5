#ifndef DOWNLOADITEMWIDGET_H
#define DOWNLOADITEMWIDGET_H

#include <QLabel>
#include <QWidget>

class DownloadTaskItemWidget : public QWidget {
  Q_OBJECT
 public:
  explicit DownloadTaskItemWidget(const QString &fileName, const QString &url,
                                  QWidget *parent = nullptr);

  void setStatus(const QString &status);

 private:
  QLabel *statusLabel;
  QLabel *fileNameLabel;
  QLabel *urlLabel;
};

#endif  // DOWNLOADITEMWIDGET_H
