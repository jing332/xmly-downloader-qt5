#ifndef DOWNLOADITEMWIDGET_H
#define DOWNLOADITEMWIDGET_H

#include <QLabel>
#include <QProgressBar>
#include <QWidget>

class DownloadTaskItemWidget : public QWidget {
  Q_OBJECT
 public:
  explicit DownloadTaskItemWidget(const QString &fileName, const QString &url,
                                  QWidget *parent = nullptr);

  void SetStatus(const QString &status);
  void UpdateProgressBar(int value);
  void SetProgressBarVisible(bool visible);
  QString GetFileNameText();

 private:
  QLabel *statusLabel_;
  QLabel *fileNameLabel_;
  QLabel *urlLabel_;
  QProgressBar *progressBar;
};

#endif  // DOWNLOADITEMWIDGET_H
