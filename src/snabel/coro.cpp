#include "snabel/coro.hpp"
#include "snackis/core/error.hpp"

namespace snabel {
  void push(Coro &cor, const Box &val) {
    cor.stack.push_back(val);
  }

  Box pop(Coro &cor) {
    CHECK(!cor.stack.empty(), _);
    auto res(cor.stack.back());
    cor.stack.pop_back();
    return res;
  }
}
