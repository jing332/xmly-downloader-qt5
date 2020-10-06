#include "audioinfo.h"

AudioInfo::AudioInfo(const AudioInfo &ai)
    : trackID_(ai.trackID_),
      title_(ai.title_),
      mp3URL32_(ai.mp3URL32_),
      mp3URL64_(ai.mp3URL64_),
      m4aURL24_(ai.m4aURL24_),
      m4aURL64_(ai.m4aURL64_) {}

AudioInfo::AudioInfo(QObject *parent) : QObject(parent) {}

AudioInfo::AudioInfo(int trackID, const QString &title, const QString &mp3URL32,
                     const QString &mp3URL64, const QString &m4aURL24,
                     const QString &m4aURL64)
    : trackID_(trackID),
      title_(title),
      mp3URL32_(mp3URL32),
      mp3URL64_(mp3URL64),
      m4aURL24_(m4aURL24),
      m4aURL64_(m4aURL64) {}

int AudioInfo::trackID() const { return trackID_; }

void AudioInfo::setTrackID(int trackID) { trackID_ = trackID; }

QString AudioInfo::title() const { return title_; }

void AudioInfo::setTitle(const QString &title) { title_ = title; }

QString AudioInfo::m4aURL24() const { return m4aURL24_; }

void AudioInfo::setM4aURL24(const QString &m4aURL24) { m4aURL24_ = m4aURL24; }

QString AudioInfo::m4aURL64() const { return m4aURL64_; }

void AudioInfo::setM4aURL64(const QString &m4aURL64) { m4aURL64_ = m4aURL64; }

QString AudioInfo::mp3URL32() const { return mp3URL32_; }

void AudioInfo::setMp3URL32(const QString &mp3URL32) { mp3URL32_ = mp3URL32; }

QString AudioInfo::mp3URL64() const { return mp3URL64_; }

void AudioInfo::setMp3URL64(const QString &mp3URL64) { mp3URL64_ = mp3URL64; }

bool AudioInfo::isEmptyURL() {
  return m4aURL24_.isEmpty() && m4aURL64_.isEmpty() && mp3URL32_.isEmpty() &&
         mp3URL64_.isEmpty();
}

void AudioInfo::ClearAllURL() {
  mp3URL32_.clear();
  mp3URL64_.clear();
  m4aURL24_.clear();
  m4aURL64_.clear();
}
