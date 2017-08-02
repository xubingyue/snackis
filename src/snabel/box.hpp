#ifndef SNABEL_BOX_HPP
#define SNABEL_BOX_HPP

#include <variant>

#include "snabel/op.hpp"
#include "snabel/type.hpp"

#include "snackis/core/fmt.hpp"

namespace snabel {  
  using namespace snackis;

  struct Exec;
  
  struct Box {
    using Val = std::variant<snabel::Val, Op, OpSeq>;
    Type &type;
    Val val;

    Box(Type &t, const snabel::Val &v);
    Box(Exec &exe, const Op &v);
    Box(Exec &exe, const OpSeq &v);
    Box(Exec &exe, Func &v);
    Box(Exec &exe, Type &v);
  };

  template <typename T>
  T get(const Box &b) { return std::get<T>(std::get<snabel::Val>(b.val)); }

  template <>
  Op get(const Box &b);

  template <>
  OpSeq get(const Box &b);
}

namespace snackis {
  template <>
  str fmt_arg(const snabel::Box &arg);
}

#endif
