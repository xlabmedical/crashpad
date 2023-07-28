#ifndef X_QT_CALLBACK_DISPATCHER_INTERNAL_HPP
#define X_QT_CALLBACK_DISPATCHER_INTERNAL_HPP
#include "./callback_respondable.hpp"
#include <QObject>
namespace xQt {
namespace detail {
class callback_dispatcher_internal
    : public xQt::callback_respondable<callback_dispatcher_internal, QObject> {
  Q_OBJECT
public:
  callback_dispatcher_internal();
  virtual ~callback_dispatcher_internal();
};
} // namespace detail
} // namespace xQt
#endif
