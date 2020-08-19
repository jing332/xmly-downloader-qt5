#ifndef DOWNLOADITEMDATA_H
#define DOWNLOADITEMDATA_H

#include <QObject>

class DownloadItemData : public QObject {
  Q_OBJECT
 public:
  explicit DownloadItemData(QObject *parent = nullptr);
  DownloadItemData(const QString &name, const QString &uri,
                   const QString &saveDir, int number);

  int number() const;
  void setNumber(int number);

  QString name() const;
  void setName(const QString &name);

  QString uri() const;
  void setUri(const QString &uri);

  QString saveDir() const;
  void setSaveDir(const QString &saveDir);

 private:
  int number_;
  QString name_;
  QString uri_;
  QString saveDir_;
};

// Q_DECLARE_METATYPE(DownloadItemData);
// Q_DECLARE_METATYPE(DownloadItemData *);

#endif  // DOWNLOADITEMDATA_H
