#include "downloaditemdata.h"

DownloadItemData::DownloadItemData(QObject *parent) : QObject(parent) {}

DownloadItemData::DownloadItemData(const QString &name, const QString &uri,
                                   const QString &saveDir, int number)
    : number_(number), name_(name), uri_(uri), saveDir_(saveDir) {}

int DownloadItemData::number() const { return number_; }

void DownloadItemData::setNumber(int number) { number_ = number; }

QString DownloadItemData::name() const { return name_; }

void DownloadItemData::setName(const QString &name) { name_ = name; }

QString DownloadItemData::uri() const { return uri_; }

void DownloadItemData::setUri(const QString &uri) { uri_ = uri; }

QString DownloadItemData::saveDir() const { return saveDir_; }

void DownloadItemData::setSaveDir(const QString &saveDir) {
  saveDir_ = saveDir;
}
