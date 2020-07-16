#include "cookieinputdialog.h"

#include <QDebug>
#include <QThreadPool>

#include "runnable/getuserinforunnable.h"
#include "ui_cookieinputdialog.h"

CookieInputDialog::CookieInputDialog(const QString &cookie, QWidget *parent)
    : QDialog(parent), ui_(new Ui::CookieInputDialog), cookie_(cookie) {
  ui_->setupUi(this);
  setWindowFlag(Qt::WindowContextHelpButtonHint, false);

  ui_->plainTextEdit->setPlainText(cookie_);
}

CookieInputDialog::~CookieInputDialog() { delete ui_; }

QString CookieInputDialog::GetCookie() { return cookie_; }

void CookieInputDialog::on_checkBtn_clicked() {
  ui_->userInfoLabel->clear();
  ui_->okBtn->setEnabled(false);
  ui_->checkBtn->setEnabled(false);

  auto runnable = new GetUserInfoRunnable(ui_->plainTextEdit->toPlainText());
  connect(runnable, &GetUserInfoRunnable::Finisehd, this,
          [&](UserInfo *userInfo) {
            if (200 == userInfo->ret) {
              ui_->userInfoLabel->setText(
                  QStringLiteral(
                      "用户名: <b>%1</b><br/>UID: <b>%2</b><br/>VIP: <b>%3</b>")
                      .arg(userInfo->nickName)
                      .arg(userInfo->uid)
                      .arg(bool(userInfo->isVip) ? "是" : "否"));

              ui_->checkBtn->setEnabled(true);
              ui_->okBtn->setEnabled(true);
              ui_->okBtn->setFocus();
            } else {
              ui_->userInfoLabel->setText(
                  QStringLiteral("Cookie无效: %1").arg(userInfo->msg));

              ui_->checkBtn->setEnabled(true);
              delete userInfo;
            }
          });

  connect(runnable, &GetUserInfoRunnable::Error, this, [&](const QString &err) {
    ui_->userInfoLabel->setText(QStringLiteral("请求失败: %1").arg(err));
    ui_->checkBtn->setEnabled(true);
  });

  QThreadPool::globalInstance()->start(runnable);
}

void CookieInputDialog::on_okBtn_clicked() {
  cookie_ = ui_->plainTextEdit->toPlainText();
  emit accept();
}

void CookieInputDialog::on_plainTextEdit_textChanged() {
  if (ui_->plainTextEdit->toPlainText().contains("1&_token=")) {
    ui_->checkBtn->setEnabled(true);
  } else {
    ui_->checkBtn->setDisabled(true);
  }
}
