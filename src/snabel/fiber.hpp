#ifndef SNABEL_FIBER_HPP
#define SNABEL_FIBER_HPP

#include "snabel/coro.hpp"

namespace snabel {
  struct Fiber: Coro {
    Fiber(Exec &exe, int64_t id):
      Coro(exe, id)
    { }
  };
}

#endif
