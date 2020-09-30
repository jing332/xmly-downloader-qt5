#ifndef UTILS_H
#define UTILS_H
#include <QObject>

class Utils {
 public:
  Utils();

  static int GetIntWidth(int n);
  static QString GetJvmDlPath();
};

#endif  // UTILS_H
