#include "snabel/box.hpp"
#include "snabel/exec.hpp"

namespace snabel {
  Box::Box(Type &t, const Val &v):
    type(t), val(v)
  { }
}

namespace snackis {
  template <>
  str fmt_arg(const snabel::Box &arg) {
    return fmt("%0|%1", arg.type.fmt(arg), arg.type.name);
  }
}
