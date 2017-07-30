#ifndef SNABEL_CTX_HPP
#define SNABEL_CTX_HPP

#include <map>
#include "snabel/box.hpp"
#include "snabel/type.hpp"
#include "snackis/core/str.hpp"

namespace snabel {
  using namespace snackis;
  
  struct Coro;
  
  struct Ctx {
    Coro &coro;
    Ctx *parent;
    std::map<str, Type> types;
    std::map<str, Box> env;
    
    Ctx(Coro &cor, Ctx *par=nullptr);
  };

  opt<Box> find_env(Ctx &ctx, const str &n);
  Box get_env(Ctx &ctx, const str &n);
  Type &add_type(Ctx &ctx, const str &n);
}

#endif
