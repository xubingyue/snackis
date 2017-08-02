#ifndef SNABEL_BOX_HPP
#define SNABEL_BOX_HPP

#include <variant>

#include "snabel/op.hpp"
#include "snackis/core/fmt.hpp"

namespace snabel {  
  using namespace snackis;

  struct Func;
  struct Exec;
  struct Type;
  
  using Val = std::variant<int64_t, str, Func *, Type *, Op, OpSeq>;

  struct Box {
    Type &type;
    Val val;

    Box(Type &t, const Val &v);
  };

  template <typename T>
  T get(const Box &b) { return std::get<T>(b.val); }
}

namespace snackis {
  template <>
  str fmt_arg(const snabel::Box &arg);
}

#endif
