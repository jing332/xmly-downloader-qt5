#include "getdownloadurldialog.h"

#include <QClipboard>
#include <QThreadPool>

#include "runnables/getchargetrackinforunnable.h"
#include "ui_getdownloadurldialog.h"

GetDownloadUrlDialog::GetDownloadUrlDialog(int trackID, const QString &cookie,
                                           QWidget *parent)
    : QDialog(parent), ui_(new Ui::GetDownloadUrlDialog) {
  ui_->setupUi(this);

  auto runnable = new GetChargeTrackInfoRunnable(trackID, cookie);
  connect(runnable, &GetChargeTrackInfoRunnable::Succeed, this,
          [&](const TrackInfo ai) {
            ui_->tipLabel->setText("");
            ui_->fileNameLineEdit->setText(ai.title());
            ui_->fileNameLineEdit->home(false);
            ui_->fileUrlLineEdit->setText(ai.m4aURL64());
            ui_->fileUrlLineEdit->home(false);
          });
  connect(runnable, &GetChargeTrackInfoRunnable::Failed, this,
          [&](int trackID, QString err) { ui_->tipLabel->setText(err); });
  QThreadPool::globalInstance()->start(runnable);
}

GetDownloadUrlDialog::~GetDownloadUrlDialog() { delete ui_; }

void GetDownloadUrlDialog::on_copyFileNameBtn_clicked() {
  qApp->clipboard()->setText(ui_->fileNameLineEdit->text());
}

void GetDownloadUrlDialog::on_copyFileUrlBtn_clicked() {
  qApp->clipboard()->setText(ui_->fileUrlLineEdit->text());
}
