#ifndef X_QT_SEND_CALLBACK_HPP
#define X_QT_SEND_CALLBACK_HPP
#include "./callback_event.hpp"
#include <QApplication>
#include <QEvent>
#include <QObject>
namespace xQt {

/*
 * Sends a callback_event to q_object on the same thread
 * **NOTE**:
 * Only makes sense when QObject was casted from
 * callback_respondable<T>
 * for some T
 */
template <typename _QObject, typename F>
auto send_callback(_QObject *q_object, F &&f) -> void {
  QApplication::sendEvent(
      q_object,
      make_callback_event_ptr<_QObject>(static_cast<F &&>(f)).release());
}
template <typename F> auto send_callback(QObject *q_object, F &&f) -> void {
  send_callback(q_object, static_cast<F &&>(f));
}
} // namespace xQt
#endif
