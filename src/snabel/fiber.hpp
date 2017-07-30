#ifndef SNABEL_FIBER_HPP
#define SNABEL_FIBER_HPP

#include <vector>

namespace snabel {
  struct Fiber {
    std::vector<Op> ops;
    size_t pc;

    Fiber();
  };
}

#endif
