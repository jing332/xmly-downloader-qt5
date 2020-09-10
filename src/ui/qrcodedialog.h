#ifndef QRCODEDIALOG_H
#define QRCODEDIALOG_H

#include <QDialog>

#include "runnables/checkqrcoderunnable.h"

namespace Ui {
class QRCodeDialog;
}

class QRCodeDialog : public QDialog {
  Q_OBJECT

 public:
  explicit QRCodeDialog(QWidget *parent = nullptr);
  ~QRCodeDialog();

  void ShowQRCode();

  QString Cookie() const;

 private slots:
  void GetQRCodeSucceed(QString qrID, QString imgBase64);

 private:
  Ui::QRCodeDialog *ui_;

  CheckQRCodeRunnable *checkQRCodeRun = nullptr;
  QString cookie_;
  QString qrID_;
};

#endif  // QRCODEDIALOG_H
