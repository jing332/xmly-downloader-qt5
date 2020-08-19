#ifndef AUDIOINFO_H
#define AUDIOINFO_H

#include <QObject>

class AudioInfo : public QObject {
  Q_OBJECT
 public:
  explicit AudioInfo(QObject *parent = nullptr);
  AudioInfo(int trackID, const QString &title, const QString &mp3URL32,
            const QString &mp3URL64, const QString &m4aURL24,
            const QString &m4aURL64);

  int trackID() const;
  void setTrackID(int trackID);

  QString title() const;
  void setTitle(const QString &title);

  QString mp3URL32() const;
  void setMp3URL32(const QString &mp3URL32);

  QString mp3URL64() const;
  void setMp3URL64(const QString &mp3URL64);

  QString m4aURL24() const;
  void setM4aURL24(const QString &m4aURL24);

  QString m4aURL64() const;
  void setM4aURL64(const QString &m4aURL64);

  bool isEmptyURL();

 private:
  int trackID_;
  QString title_;
  QString mp3URL32_;
  QString mp3URL64_;
  QString m4aURL24_;
  QString m4aURL64_;
};

// Q_DECLARE_METATYPE(AudioInfo);

#endif  // AUDIOINFO_H
