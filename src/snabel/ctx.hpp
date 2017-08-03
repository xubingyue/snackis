#ifndef SNABEL_CTX_HPP
#define SNABEL_CTX_HPP

#include <list>
#include <map>

#include "snabel/box.hpp"
#include "snabel/func.hpp"
#include "snabel/type.hpp"
#include "snackis/core/str.hpp"

namespace snabel {
  using namespace snackis;
  
  struct Coro;
  
  struct Ctx {
    using CmpType = func<bool (const Type &, const Type &)>;
    
    Coro &coro;
    std::list<Type> types;
    std::list<Func> funcs;
    std::map<str, int64_t> labels;
    std::map<str, Box> env;
    
    Ctx(const Ctx &src);
    Ctx(Coro &cor);
    const Ctx &operator =(const Ctx &) = delete;
  };

  opt<Box> find_env(Ctx &ctx, const str &n);

  Box get_env(Ctx &ctx, const str &n);
  void put_env(Ctx &ctx, const str &n, const Box &val);

  Type &add_type(Ctx &ctx, const str &n);
  Func &add_func(Ctx &ctx, const str &n);
}

#endif
