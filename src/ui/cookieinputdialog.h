#ifndef COOKIEINPUTDIALOG_H
#define COOKIEINPUTDIALOG_H

#include <QDialog>

namespace Ui {
class CookieInputDialog;
}

class CookieInputDialog : public QDialog {
  Q_OBJECT

 public:
  explicit CookieInputDialog(const QString &cookie, QWidget *parent = nullptr);
  ~CookieInputDialog();

  QString GetCookie();

 private slots:
  void on_qrCodeBtn_clicked();
  void on_checkBtn_clicked();
  void on_okBtn_clicked();
  void on_plainTextEdit_textChanged();

 private:
  Ui::CookieInputDialog *ui_;

  QString cookie_;
};

#endif  // COOKIEINPUTDIALOG_H
