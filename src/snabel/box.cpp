#include "snabel/box.hpp"
#include "snabel/exec.hpp"

namespace snabel {
  const Undef undef;

  bool Undef::operator ()(const Box &box) const {
    return box.val.index() == 0;
  }

  Box::Box(Type &t, const Val &v):
    type(t), val(v)
  { }
}

namespace snackis {
  template <>
  str fmt_arg(const snabel::Box &arg) {
    return fmt("%0:%1",
	       snabel::undef(arg) ? "undef" : arg.type.fmt(arg),
	       arg.type.name);
  }
}
