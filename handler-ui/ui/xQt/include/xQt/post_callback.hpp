#ifndef X_QT_POST_CALLBACK_HPP
#define X_QT_POST_CALLBACK_HPP
#include "./callback_event.hpp"
#include <QApplication>
#include <QEvent>
#include <QObject>
namespace xQt {

/*
 * Posts a callback_event to q_object across threads
 * **NOTE**:
 * Only makes sense when QObject was casted from
 * callback_respondable<T>
 * for some T
 */
template <typename _QObject, typename F>
auto post_callback(_QObject *q_object, F &&f,
                   int priority = Qt::NormalEventPriority) -> void {
  QApplication::postEvent(
      q_object,
      make_callback_event_ptr<_QObject>(static_cast<F &&>(f)).release(),
      priority);
}

template <typename F>
auto post_callback(QObject *q_object, F &&f,
                   int priority = Qt::NormalEventPriority) -> void {
  post_callback<QObject>(q_object, static_cast<F &&>(f), priority);
}

} // namespace xQt
#endif
