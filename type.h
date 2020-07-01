#ifndef TYPE_H
#define TYPE_H

#include <QMetaType>
#include <QObject>

typedef struct {
  int id;
  QString title;
  QString url;
  QString number;
} AudioItem;

Q_DECLARE_METATYPE(AudioItem*);

#endif  // TYPE_H
