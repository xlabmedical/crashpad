#ifndef X_QT_EVENT_DISPATCHER_HPP
#define X_QT_EVENT_DISPATCHER_HPP
#include "./callback_event.hpp"
#include "./post_callback.hpp"
#include "./send_callback.hpp"
#include <QApplication>
#include <QEvent>
namespace xQt {
/*
 * event_dispatcher is capable of dispatching events
 * to QObjects. It is to be used when a class has
 * to dispatch events to some QObject, without having
 * to know who it is dispatching to:
 */
template <typename _QObject> class event_dispatcher {
private:
  _QObject *_object;

public:
  event_dispatcher(_QObject *_object = nullptr) : _object{_object} {}
  void set_object(_QObject *q_object) { this->_object = q_object; }
  /*
   * For posting accross threads
   */
  auto post(QEvent *q_event, int priority = Qt::NormalEventPriority) -> void {
    QApplication::postEvent(_object, q_event, priority);
  }
  /*
   * For sending on the same thread
   */
  auto send(QEvent *q_event) -> void {
    QApplication::sendEvent(_object, q_event);
  }
  /*
   * For posting a callback across threads
   */
  template <typename F>
  auto post_callback(F &&f, int priority = Qt::NormalEventPriority) -> void {
    xQt::post_callback<_QObject>(_object, static_cast<F &&>(f), priority);
  }
  /*
   * For sending a callback on the same thread
   */
  template <typename F> auto send_callback(F &&f) -> void {
    xQt::send_callback<_QObject>(_object, static_cast<F &&>(f));
  }
};

template <typename _QObject>
auto make_event_dispatcher(_QObject *q_object) -> event_dispatcher<_QObject> {
  return event_dispatcher<_QObject>(q_object);
}

} // namespace xQt
#endif
