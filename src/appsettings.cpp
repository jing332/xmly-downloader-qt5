#include "appsettings.h"

AppSettings::AppSettings(QObject *parent) : QObject(parent) {
  auto fileName = qApp->applicationDirPath() + "/config.ini";
  settings_ = new QSettings(fileName, QSettings::IniFormat, this);
  settings_->setIniCodec("UTF-8");
}

AppSettings::~AppSettings() {}

int AppSettings::theme() const { return settings_->value("theme").toInt(); }

void AppSettings::setTheme(int theme) { settings_->setValue("theme", theme); }

int AppSettings::albumID() const { return settings_->value("albumID").toInt(); }

void AppSettings::setAlbumID(int albumID) {
  settings_->setValue("albumID", albumID);
};

QString AppSettings::downloadDir() const {
  return settings_->value("downloadDir").toString();
}

void AppSettings::setDownloadDir(const QString &downloadDir) {
  settings_->setValue("downloadDir", downloadDir);
}

QString AppSettings::cookie() const {
  return settings_->value("cookie").toString();
}

void AppSettings::setCookie(const QString &cookie) {
  settings_->setValue("cookie", cookie);
}
