#include "snabel/box.hpp"
#include "snabel/exec.hpp"

namespace snabel {
  Box::Box(Type &t, const snabel::Val &v):
    type(t), val(v)
  { }

  Box::Box(Exec &exe, const Op &v):
    type(exe.op_type), val(v)
  { }
    
  Box::Box(Exec &exe, const OpSeq &v):
    type(exe.op_seq_type), val(v)
  { }

  Box::Box(Exec &exe, Func &v):
    type(exe.func_type), val(&v)
  { }
  
  Box::Box(Exec &exe, Type &v):
    type(exe.meta_type), val(&v)
  { }

  template <>
  Op get(const Box &b) { return std::get<Op>(b.val); }

  template <>
  OpSeq get(const Box &b) { return std::get<OpSeq>(b.val); }
}

namespace snackis {
  template <>
  str fmt_arg(const snabel::Box &arg) {
    return fmt("%0|%1", arg.type.fmt(arg), arg.type.name);
  }
}
