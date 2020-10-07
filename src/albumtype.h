#ifndef ALBUMTYPE_H
#define ALBUMTYPE_H

#include <QString>
namespace AlbumType {
enum { FREE, VIP, PAID };
QString ToString(int type) {
  switch (type) {
    case AlbumType::FREE:
      return "免费";
    case AlbumType::VIP:
      return "VIP";
    default:
    case AlbumType::PAID:
      return "付费";
  }
}
}  // namespace AlbumType

#endif  // ALBUMTYPE_H
