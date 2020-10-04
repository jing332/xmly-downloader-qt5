#ifndef GETDOWNLOADURLDIALOG_H
#define GETDOWNLOADURLDIALOG_H

#include <QDialog>

namespace Ui {
class GetDownloadUrlDialog;
}

class GetDownloadUrlDialog : public QDialog {
  Q_OBJECT

 public:
  explicit GetDownloadUrlDialog(int trackID, const QString &cookie,
                                QWidget *parent = nullptr);
  ~GetDownloadUrlDialog();

 private slots:
  void on_copyFileNameBtn_clicked();

  void on_copyFileUrlBtn_clicked();

 private:
  Ui::GetDownloadUrlDialog *ui_;
};

#endif  // GETDOWNLOADURLDIALOG_H
