#ifndef X_QT_CALLBACK_DISPATCHER_HPP
#define X_QT_CALLBACK_DISPATCHER_HPP
#include "./callback_dispatcher_internal.hpp"
namespace xQt {
/*
 * Object that dispatches callbacks in a thread safe manner.
 * In can be held as a value.
 * **NOTE**: the callbacks should take no arguments.
 */
class callback_dispatcher {
public:
  template <typename F>
  auto dispatch(F &&f, int priority = Qt::NormalEventPriority) {
    object.post_callback([f = std::make_shared<F>(static_cast<F &&>(f))](
                             detail::callback_dispatcher_internal *) { (*f)(); },
                         priority);
  }
  template <typename F>
  auto operator()(F &&f, int priority = Qt::NormalEventPriority) {
    dispatch(static_cast<F &&>(f), priority);
  }

private:
  detail::callback_dispatcher_internal object;
};
} // namespace xQt
#endif
