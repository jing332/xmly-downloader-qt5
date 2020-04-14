#include "getaudiobookinforunnable.h"

#include "cgo.h"

GetAudiobookInfoRunnable::GetAudiobookInfoRunnable(int audioBookId)
    : audioBookId_(audioBookId) {}

void GetAudiobookInfoRunnable::run() {
  auto info = Cgo::getInstance()->cgo_getAudiobookInfo(audioBookId_);
  emit finished(info, audioBookId_);
}
