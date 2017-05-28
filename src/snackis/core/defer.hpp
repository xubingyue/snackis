#ifndef SNACKIS_DEFER_HPP
#define SNACKIS_DEFER_HPP

#define DEFER(fn)				\
  Defer UNIQUE(defer)([&]() fn)			\

#include <snackis/core/func.hpp>
#include <snackis/core/macros.hpp>

namespace snackis {
  struct Defer {
    using Fn = func<void ()>;
    
    const Fn fn;
    
    Defer(const Fn &fn);
    ~Defer();
  };
}

#endif
