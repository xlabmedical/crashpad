#ifndef X_QT_CALLBACK_EVENT_HPP
#define X_QT_CALLBACK_EVENT_HPP
#include <QEvent>
#include <QObject>
#include <functional>
#include <memory>
namespace xQt {
/*
 * Defining the type of the QEvent
 */
static constexpr QEvent::Type callback_event_type =
    static_cast<QEvent::Type>(2000);

/*
 * A Qevent holding a callback.
 * It can be constructed from a lambda, or any other callable
 */
template <typename _QObject> class callback_event : public QEvent {
public:
  template <typename F>
  callback_event(F f) : QEvent{callback_event::type()}, f{std::move(f)} {}
  virtual ~callback_event() {}
  static constexpr QEvent::Type event_type = callback_event_type;
  static constexpr QEvent::Type type() { return event_type; }

  auto get_callback() -> std::function<void(_QObject *)> & { return f; }
  auto get_callback() const -> const std::function<void(_QObject *)> & {
    return f;
  }
  auto execute(_QObject *q_object) const -> void { f(q_object); };
  auto execute(_QObject *q_object) -> void { f(q_object); };

  /*
   * casts a Qevent to a callback_event
   */
  static auto cast(QEvent *q_event) -> callback_event * {
    return dynamic_cast<callback_event<_QObject> *>(q_event);
  }

private:
  std::function<void(_QObject *)> f;
};
/*
 * Creates a unique_ptr to a callback_event from a callable
 */
template <typename _QObject, typename F>
auto make_callback_event_ptr(F &&f)
    -> std::unique_ptr<callback_event<_QObject>> {
  return std::make_unique<callback_event<_QObject>>(static_cast<F &&>(f));
}
template <typename F>
auto make_callback_event_ptr(F &&f)
    -> std::unique_ptr<callback_event<QObject>> {
  return std::make_unique<callback_event<QObject>>(static_cast<F &&>(f));
}
} // namespace xQt
#endif
