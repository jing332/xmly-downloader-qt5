#include "qrcodedialog.h"

#include <QThreadPool>

#include "runnables/getqrcoderunnable.h"
#include "ui_qrcodedialog.h"

QRCodeDialog::QRCodeDialog(QWidget *parent)
    : QDialog(parent), ui_(new Ui::QRCodeDialog) {
  ui_->setupUi(this);
}

QRCodeDialog::~QRCodeDialog() {
  delete ui_;
  if (checkQRCodeRun) {
    checkQRCodeRun->Stop();
  }
}

void QRCodeDialog::ShowQRCode() {
  auto getQRCodeRun = new GetQRCodeRunnable();
  connect(getQRCodeRun, &GetQRCodeRunnable::Succeed, this,
          &QRCodeDialog::GetQRCodeSucceed);
  connect(getQRCodeRun, &GetQRCodeRunnable::Failed, [&](QString err) {
    ui_->tipLabel->setText("无法获取二维码: " + err);
  });

  QThreadPool::globalInstance()->start(getQRCodeRun);
}

QString QRCodeDialog::Cookie() const { return cookie_; }

void QRCodeDialog::GetQRCodeSucceed(QString qrID, QString imgBase64) {
  qrID_ = qrID;
  QPixmap pixmap;
  pixmap.loadFromData(QByteArray::fromBase64(imgBase64.toLocal8Bit()));
  pixmap.scaled(ui_->imgLabel->size(), Qt::KeepAspectRatio);
  ui_->imgLabel->setPixmap(pixmap);
  ui_->tipLabel->setText("请使用喜马拉雅手机APP扫描上方二维码");

  checkQRCodeRun = new CheckQRCodeRunnable(qrID_);
  connect(checkQRCodeRun, &CheckQRCodeRunnable::Succeed, [&](QString cookie) {
    cookie_ = cookie;
    accept();
  });
  QThreadPool::globalInstance()->start(checkQRCodeRun);
}
