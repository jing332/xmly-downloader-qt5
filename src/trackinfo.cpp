#include "trackinfo.h"

TrackInfo::TrackInfo(QObject *parent) : QObject(parent) {}

TrackInfo::TrackInfo(const TrackInfo &ai)
    : trackID_(ai.trackID_),
      title_(ai.title_),
      duration_(ai.duration_),
      mp3URL32_(ai.mp3URL32_),
      mp3URL64_(ai.mp3URL64_),
      m4aURL24_(ai.m4aURL24_),
      m4aURL64_(ai.m4aURL64_) {}

TrackInfo::TrackInfo(int trackID, const QString &title, int duration,
                     const QString &mp3URL32, const QString &mp3URL64,
                     const QString &m4aURL24, const QString &m4aURL64)
    : trackID_(trackID),
      title_(title),
      duration_(duration),
      mp3URL32_(mp3URL32),
      mp3URL64_(mp3URL64),
      m4aURL24_(m4aURL24),
      m4aURL64_(m4aURL64) {}

int TrackInfo::trackID() const { return trackID_; }

void TrackInfo::setTrackID(int trackID) { trackID_ = trackID; }

QString TrackInfo::title() const { return title_; }

void TrackInfo::setTitle(const QString &title) { title_ = title; }

int TrackInfo::duration() const { return duration_; }

void TrackInfo::setDuration(int duration) { duration_ = duration; }

QString TrackInfo::m4aURL24() const { return m4aURL24_; }

void TrackInfo::setM4aURL24(const QString &m4aURL24) { m4aURL24_ = m4aURL24; }

QString TrackInfo::m4aURL64() const { return m4aURL64_; }

void TrackInfo::setM4aURL64(const QString &m4aURL64) { m4aURL64_ = m4aURL64; }

QString TrackInfo::mp3URL32() const { return mp3URL32_; }

void TrackInfo::setMp3URL32(const QString &mp3URL32) { mp3URL32_ = mp3URL32; }

QString TrackInfo::mp3URL64() const { return mp3URL64_; }

void TrackInfo::setMp3URL64(const QString &mp3URL64) { mp3URL64_ = mp3URL64; }

bool TrackInfo::isEmptyURL() {
  return m4aURL24_.isEmpty() && m4aURL64_.isEmpty() && mp3URL32_.isEmpty() &&
         mp3URL64_.isEmpty();
}

void TrackInfo::ClearAllURL() {
  mp3URL32_.clear();
  mp3URL64_.clear();
  m4aURL24_.clear();
  m4aURL64_.clear();
}
