#ifndef SNABEL_FIBER_HPP
#define SNABEL_FIBER_HPP

#include "snabel/coro.hpp"
#include "snackis/core/uid.hpp"

namespace snabel {
  struct Fiber: Coro {
    UId id;
    
    Fiber(Exec &exe, UId id):
      Coro(exe), id(id)
    { }
  };
}

#endif
