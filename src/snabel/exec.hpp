#ifndef SNABEL_EXEC_HPP
#define SNABEL_EXEC_HPP

#include <atomic>
#include <map>

#include "snabel/fiber.hpp"
#include "snabel/sym.hpp"
#include "snackis/core/uid.hpp"

namespace snabel {
  struct Type;
  
  struct Exec {
    std::map<UId, Fiber> fibers;
    Fiber &main;
    Type &meta_type,
      &func_type, &i64_type, &lambda_type, &str_type,
      &undef_type, &void_type;
    std::atomic<Sym> next_sym;
    
    Exec();
    Exec(const Exec &) = delete;
    const Exec &operator =(const Exec &) = delete;
  };

  Sym gensym(Exec &exe);
}

#endif
