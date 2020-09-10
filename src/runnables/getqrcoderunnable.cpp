#include "getqrcoderunnable.h"

GetQRCodeRunnable::GetQRCodeRunnable() {}

void GetQRCodeRunnable::run() {
  DataError *data = CgoGetQRCode();
  if (data->error) {
    emit Failed(QString(data->error));
    return;
  }
  auto qrCode = static_cast<CgoQRCode *>(data->data);
  if (0 == qrCode->ret) {
    emit Succeed(qrCode->qrID, qrCode->img);
    return;
  }
  emit Failed(qrCode->msg);
}
