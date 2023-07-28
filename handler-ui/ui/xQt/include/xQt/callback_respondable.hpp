#ifndef X_QT_CALLBACK_RESPONDABLE_HPP
#define X_QT_CALLBACK_RESPONDABLE_HPP
#include "./callback_event.hpp"
#include "./event_dispatcher.hpp"
#include "./post_callback.hpp"
#include "./send_callback.hpp"
#include <QWidget>
namespace xQt {
/*
 * Wraps a Qt widget, and endows it with the abbility
 * to receive callback events
 */
template <typename Derived, typename WidgetBase>
class callback_respondable : public WidgetBase {
public:
  using callback_event_t = xQt::callback_event<Derived>;
  using event_dispatcher_t = xQt::event_dispatcher<Derived>;

  callback_respondable(QWidget *parent = nullptr) : WidgetBase{parent} {}
  virtual ~callback_respondable() = default;
  /*
   * This method is thread-safe, used
   * to post a callback to this object across threads
   */
  template <typename F>
  auto post_callback(F &&f, int priority = Qt::NormalEventPriority) -> void {
    xQt::post_callback(static_cast<Derived *>(this), static_cast<F &&>(f),
                       priority);
  }
  /*
   * This method is NOT thread-safe, used
   * to send a callback to this object on the same thread
   */
  template <typename F> auto send_callback(F &&f) -> void {
    xQt::send_callback(static_cast<Derived *>(this), static_cast<F &&>(f));
  }
  /*
   * Returns an event dispatcher for this
   */
  auto make_event_dispatcher() {
    return event_dispatcher_t(static_cast<Derived *>(this));
  }

protected:
  auto customEvent(QEvent *q_event) -> void override {
    if (q_event->type() == xQt::callback_event_type)
      callback_event_t::cast(q_event)->execute(static_cast<Derived *>(this));
    else
      WidgetBase::customEvent(q_event);
  }
};

template <typename Derived, typename _QObject>
auto make_event_dispatcher(callback_respondable<Derived, _QObject> *q_object)
    -> event_dispatcher<Derived> {
  return q_object->make_event_dispatcher();
}

} // namespace xQt
#endif
