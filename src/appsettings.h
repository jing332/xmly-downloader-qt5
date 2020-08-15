#ifndef APPSETTINGS_H
#define APPSETTINGS_H
#include <QCoreApplication>
#include <QSettings>

class AppSettings : public QObject {
  Q_OBJECT
 public:
  explicit AppSettings(QObject *parent = nullptr);
  ~AppSettings();

  int theme() const;
  void setTheme(int theme);

  int albumID() const;
  void setAlbumID(int albumID);

  QString downloadDir() const;
  void setDownloadDir(const QString &downloadDir);

  QString cookie() const;
  void setCookie(const QString &cookie);

 private:
  QSettings *settings_;
};

#endif  // APPSETTINGS_H
