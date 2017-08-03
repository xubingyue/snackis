#ifndef SNABEL_SCOPE_HPP
#define SNABEL_SCOPE_HPP

#include <list>
#include <map>

#include "snabel/box.hpp"
#include "snabel/func.hpp"
#include "snabel/type.hpp"
#include "snackis/core/str.hpp"

namespace snabel {
  using namespace snackis;
  
  struct Coro;

  struct Label {
    int64_t depth, pc;
    
    Label(int64_t depth, int64_t pc);
  };
  
  struct Scope {
    using CmpType = func<bool (const Type &, const Type &)>;
    
    Coro &coro;
    std::list<Type> types;
    std::list<Func> funcs;
    std::map<str, Label> labels;
    std::map<str, Box> env;
    
    Scope(const Scope &src);
    Scope(Coro &cor);
    const Scope &operator =(const Scope &) = delete;
  };

  opt<Box> find_env(Scope &scp, const str &n);

  Box get_env(Scope &scp, const str &n);
  void put_env(Scope &scp, const str &n, const Box &val);

  Type &add_type(Scope &scp, const str &n);
  Func &add_func(Scope &scp, const str &n);
}

#endif
