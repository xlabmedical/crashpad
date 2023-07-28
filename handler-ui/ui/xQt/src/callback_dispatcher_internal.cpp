#include "xQt/callback_dispatcher_internal.hpp"
namespace xQt {
namespace detail {
callback_dispatcher_internal::callback_dispatcher_internal()
    : xQt::callback_respondable<callback_dispatcher_internal, QObject>{
          nullptr} {};
callback_dispatcher_internal::~callback_dispatcher_internal() = default;
} // namespace detail
} // namespace xQt
