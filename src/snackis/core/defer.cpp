#include "snackis/core/defer.hpp"

namespace snackis {
  Defer::Defer(const Fn &fn): fn(fn) { }
  
  Defer::~Defer() { fn(); }
}
